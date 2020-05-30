#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double soil_ph		= 6;

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_soil_ph,
         "title=\"Soil pH\";obs",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value = 0.2 * rand() / RAND_MAX;				//returns a random value between 0 and 0.2
	static double increasing_sign = rand() / RAND_MAX - RAND_MAX / 2;	//returns a random value between -0.5 and 0.5

	if(increasing_sign > 0)
		soil_ph += random_value;									//soil_ph can increase of up to 0.2
	else
		soil_ph -= random_value;									//soil_ph can decrease of up to 0.2

	/* Keeping soil_ph in the right range of the scale */
	if(soil_ph > 14)
		soil_ph = 14;
	else if(soil_ph < 0)
		soil_ph = 0;
	
	coap_notify_observers(&res_soil_ph);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Soil pH\":%f}", soil_ph));
}


