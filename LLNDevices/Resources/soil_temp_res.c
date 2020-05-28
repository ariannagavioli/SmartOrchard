#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double temperature = 25;

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

/* A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated */
EVENT_RESOURCE(res_soil_temp,
         "title=\"Soil Temperature\";obs",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value = rand() / RAND_MAX;				//returns a random value between 0 and 1
	static double random_sign = rand()/RAND_MAX - 0.5;			//returns a random value between -0.5 and 0.5
	if(random_sign > 0) {
		temperature += random_value;
	} else {
		temperature -= random_value;
	}
	
	if(temperature > 35)
		temperature = 35;
	else if(temperature < 13)
		temperature = 13;
	
	coap_notify_observers(&res_soil_temp);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{ \"Soil temperature\":%f}", temperature));
}
