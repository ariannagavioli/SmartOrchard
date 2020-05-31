#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double soil_salinity		= 1;			//Expressed in dS/m
static int increasing_sign		= 1;			//Either 1 or -1 depending on the period of the day (can be modified through a put on the resource)

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_soil_salinity,
         "title=\"Soil Salinity\";obs",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value;
	random_value = rand() / RAND_MAX;						//returns a random value between 0 and 1
	
	soil_salinity += (increasing_sign * random_value);					//soil_salinity can change up to 1

	/* Keeping soil_salinity in a reasonable range of the scale */
	if(soil_salinity > 20)
		soil_salinity = 20;
	else if(soil_salinity < 0)
		soil_salinity = 0;
	
	coap_notify_observers(&res_soil_salinity);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Soil Salinity\":%f dS/m}", soil_salinity));
}


