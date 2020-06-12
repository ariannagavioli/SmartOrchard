#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double luminosity	= 5000;			//Expressed in lux
static int increasing_sign	= 1;			//Either 1 or -1 depending on the period of the day (can be modified through a put on the resource)

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_luminosity,
         "title=\"Environment Luminosity\";obs",
         res_get_handler,
         res_post_handler,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value;
	random_value = rand() % 60000;				
	random_value *= 0.1;				//returns a random value between 0 and 6000
	
	luminosity += (increasing_sign * random_value);						//luminosity can change up to 300 lux

	/* Keeping luminosity as a reasonable lux measure - most probably never 0 nor 107000*/
	if(luminosity > 107000)
		luminosity = 107000;
	else if(luminosity < 0)
		luminosity = 0;
	
	coap_notify_observers(&res_luminosity);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Luminosity\":%f}", luminosity));
}

/* For simulation purposes, a post call will adjust the lighting resource,
 * making it possible to increase it or decrease it */
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	const char *sign = NULL;
	static int length;
	
	length = coap_get_post_variable(request, "sign", &sign);
	
	if(length) {
		
		static char msg[2];
		memset(msg, 0, sizeof(msg));
		memcpy(msg, sign, length);
		
		/* If the client asks for the decreasing of the lighting
		 */
		if(!strcmp(msg, "+") && increasing_sign < 0) {
			increasing_sign = 1;
		}
		
		/* If the client asks for the increasing of the lighting
		 */
		else if(!strcmp(msg, "-") && increasing_sign > 0) {
			increasing_sign = -1;
		}

		coap_set_status_code(response, CHANGED_2_04);
			
	} else {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}
}




