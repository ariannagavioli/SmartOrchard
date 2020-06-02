#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double humidity		= 85;		//humidity percentage
static int increasing_sign	= -1;		//Either 1 or -1 depending on the period of the day (can be modified through a put on the resource)

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_soil_hum,
         "title=\"Soil Humidity\";obs",
         res_get_handler,
         res_post_handler,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value;
	random_value = rand() % 110;
	random_value *= 0.1;								//random value is a value n*0.1, where n = {0, 1, ..., 10}

	humidity += (increasing_sign * random_value);
	
	/* Keeping humidity as a percentage measure */
	if(humidity > 100)
		humidity = 100;
	else if(humidity < 0)
		humidity = 0;
	
	coap_notify_observers(&res_soil_hum);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Soil humidity\":%f %%}", humidity));
}

/* For simulation purposes, a post call will adjust the humidity resource, such as its increase or decrease */
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	const char *sign = NULL;
	const char *increase = NULL;
	static int length1;
	static int length2;
	
	length1 = coap_get_post_variable(request, "sign", &sign);
	length2 = coap_get_post_variable(request, "increase", &increase);
	
	if(length1) {
		
		static char msg[2];
		memset(msg, 0, sizeof(msg));
		memcpy(msg, sign, length1);
		
		/* If the client asks to for the decreasing humidity
		 * to start increase, then change the increasing sign
		 */
		if(!strcmp(msg, "+") && increasing_sign < 0) {
			increasing_sign = 1;
		}

		/* If the client asks to for the increasing humidity
		 * to start decrease, then change the increasing sign
		 */
		else if(!strcmp(msg, "-") && increasing_sign > 0) {
			increasing_sign = -1;
		}

		coap_set_status_code(response, CHANGED_2_04);

	} else if(length2) {

		static int amount;
		amount = atoi(increase);
		humidity += amount;

		coap_set_status_code(response, CHANGED_2_04);

	} else {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}
}
