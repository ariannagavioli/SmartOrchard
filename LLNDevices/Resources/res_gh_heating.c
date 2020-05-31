#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "contiki.h"
#include "coap-engine.h"
#include "dev/leds.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static char status[4] = "off";

RESOURCE(res_gh_heating,
         "title=\"Greenhouse Heating System;rt=\"Control\"",
		 res_get_handler,
         res_post_handler,
         NULL,
         NULL);

/* Returns the status of the heating system (on/off) */
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	
	static char msg[128];
	sprintf(msg, "{\"Heating Status\":%s}",status);
	static int length;
	length = strlen(msg);
	memcpy(buffer, msg, length);

	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_header_etag(response, (uint8_t *)&length, 1);
	coap_set_payload(response, buffer, length);
}

/* Activates or deactivates the heating system */
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	const char *mode = NULL;

	if(coap_get_post_variable(request, "mode", &mode)) {
		if(!strcmp("on",mode) && strcmp("on", status)) {		//The client asked to activate the heating system, which was inactive
			memset(status, 0, sizeof(status));
			memcpy(status, "on", strlen("on"));
			leds_on(LEDS_NUM_TO_MASK(LEDS_RED));				//The active heating system is simulated through a LED
		}
		else if(!strcmp("off",mode) && strcmp("off", status)) {	//The client asked to deactivate the active heating system
			memset(status, 0, sizeof(status));
			memcpy(status, "off", strlen("off"));
			leds_off(LEDS_NUM_TO_MASK(LEDS_RED));
		}
		coap_set_status_code(response,CREATED_2_01);
	}

	else {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}
}
