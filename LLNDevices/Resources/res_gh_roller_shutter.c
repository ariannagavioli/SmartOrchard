#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "contiki.h"
#include "coap-engine.h"
#include "dev/leds.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static char status[8] = "closed";

RESOURCE(res_gh_roller_shutter,
         "title=\"Greenhouse Roller Shutter;rt=\"Control\"",
		 res_get_handler,
         res_post_handler,
         NULL,
         NULL);

/* Returns the status of the heating system (on/off) */
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	
	static char msg[128];
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "{\"Roller_Shutter_Status\":\"%s\"}",status);
	static int length;
	length = strlen(msg);
	memcpy(buffer, msg, length);

	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_header_etag(response, (uint8_t *)&length, 1);
	coap_set_payload(response, buffer, length);
}

/* Activates or deactivates the heating system */
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	const char *action = NULL;

	if(coap_get_post_variable(request, "action", &action)) {
		if(!strcmp("open",action) && strcmp("open", status)) {		//The client asked to open the roller shutter, which was closed
			memset(status, 0, sizeof(status));
			memcpy(status,"open",strlen("open"));
			leds_on(LEDS_NUM_TO_MASK(LEDS_GREEN));					//The open shutter is represented by the green LED
			leds_off((LEDS_NUM_TO_MASK(LEDS_RED)));
			coap_set_status_code(response,CHANGED_2_04);
		}
		else if(!strcmp("closed",action) && strcmp("closed", status)) {	//The client asked to close the open shutter
			memset(status,0,sizeof(status));
			memcpy(status,"closed",strlen("closed"));
			leds_on(LEDS_NUM_TO_MASK(LEDS_RED));
			leds_off(LEDS_NUM_TO_MASK(LEDS_GREEN));
			coap_set_status_code(response,CHANGED_2_04);
		}
		else {
			coap_set_status_code(response, BAD_REQUEST_4_00);
		}
		
	}

	else {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}
}
