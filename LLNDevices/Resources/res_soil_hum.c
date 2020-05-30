#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double humidity		= 85;		//humidity percentage
static int increasing_sign	= 1;		//Either 1 or -1 depending on the period of the day (can be modified through a put on the resource)

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_soil_hum,
         "title=\"Soil Humidity\";obs",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value = 3 * rand() / RAND_MAX;				//returns a random value between 0 and 3
	
	humidity += (increasing_sign * random_value);					//humidity can change up to 3%
	
	/* Keeping humidity as a percentage measure */
	if(humidity > 100)
		humidity = 100;
	else if(humidity < 0)
		humidity = 0;
	
	coap_notify_observers(&res_soil_hum);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Soil humidity\":%f %}", humidity));
}

