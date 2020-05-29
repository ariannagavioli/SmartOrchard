#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double luminosity	= 50;			//Expressed in percentage
static int increasing_sign	= 1;			//Either 1 or -1 depending on the period of the day (can be modified through a put on the resource)

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_luminosity,
         "title=\"Environment Luminosity\";obs",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value = 10 * rand() / RAND_MAX;				//returns a random value between 0 and 10
	
	luminosity += (increasing_sign * random_value);						//luminosity can change up to 10%

	/* Keeping luminosity as a percentage measure */
	if(luminosity > 100)
		luminosity = 100;
	else if(luminosity < 0)
		luminosity = 0;
	
	coap_notify_observers(&res_luminosity);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Luminosity\":%f %}", luminosity));
}

