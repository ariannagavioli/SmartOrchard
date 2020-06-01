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
	sprintf(msg, "{\"Irrigator Status\":\"%s\"}",status);
	static int length;
	length = strlen(msg);
	memcpy(buffer, msg, length);

	coap_set_header_content_format(response, TEXT_PLAIN); 
	coap_set_header_etag(response, (uint8_t *)&length, 1);
	coap_set_payload(response, buffer, length);
}

/* Activates or deactivates the irrigator */
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	
	const char *mode = NULL;
	static int length;
	
	length = coap_get_post_variable(request, "mode", &mode);
	
	if(length) {		//If post is correctly formulated
		
		static char msg[4];
		memset(msg, 0, sizeof(msg));
		memcpy(msg, mode, length);
		
		/* If the client asks to activate the inactive irrigator
		 * the status is set to active and the green led is lit
		 */
		if(!strcmp(msg, "on") && strcmp(status, "on")) {
			leds_off(LEDS_NUM_TO_MASK(LEDS_RED));
			leds_on(LEDS_NUM_TO_MASK(LEDS_GREEN));
			memset(status, 0, sizeof(status));
			memcpy(status, "on", strlen("on"));
			coap_set_status_code(response, CHANGED_2_04);
		}
		
		/* If the client asks to turn off the active irrigator
		 * the status is set to inactive and the red led is lit
		 */
		else if(!strcmp(msg, "off") && strcmp(status, "off")) {
			leds_on(LEDS_NUM_TO_MASK(LEDS_RED));
			leds_off(LEDS_NUM_TO_MASK(LEDS_GREEN));
			memset(status, 0, sizeof(status));
			memcpy(status, "off", strlen("off"));
			coap_set_status_code(response, CHANGED_2_04);
		}
		
		/* Any other form of request is considered bad request */
		else {
			coap_set_status_code(response, BAD_REQUEST_4_00);
		}
			
	} else {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}
}
