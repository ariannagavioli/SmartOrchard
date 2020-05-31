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

RESOURCE(res_irrigator,
         "title=\"Irrigator;rt=\"Control\"",
		 res_get_handler,
         res_post_handler,
         NULL,
         NULL);

/* Returns the status of the irrigator (on/off) */
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	
	static char msg[128];
	sprintf(msg, "{\"Irrigator Status\":%s}",status);
	static int length;
	length = strlen(msg);
	memcpy(buffer, msg, length);

	coap_set_header_content_format(response, TEXT_PLAIN); 
	coap_set_header_etag(response, (uint8_t *)&length, 1);
	coap_set_payload(response, buffer, length);
}

/* Activates the irrigator for a given interval of time */
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	const char *duration = NULL;
	if(coap_get_post_variable(request, "duration", &duration) && strcmp("on",status)) {		/* If irrigation is asked and the irrigator is not active */
		memset(status, 0, sizeof(status));
		memcpy(status,"on",strlen("on"));
		static int interval;
		interval = atoi(duration);
		coap_set_status_code(response,CREATED_2_01);
		/* Simulation of the irrigation interval */
		leds_on(LEDS_NUM_TO_MASK(LEDS_GREEN));
		sleep(interval);
		leds_off(LEDS_NUM_TO_MASK(LEDS_GREEN));
		memset(status, 0, sizeof(status));
		memcpy(status,"off",strlen("off"));
	} else {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}
}
