#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double temperature	= 25;
static int increasing_sign	= 1;		//Either 1 or -1 depending on the period of the day (can be modified through a put on the resource)

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_soil_temp,
         "title=\"Soil Temperature\";obs",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value = rand() / RAND_MAX;			//returns a random value between 0 and 1
	
	temperature += (increasing_sign * random_value);			//temperature can change up to 1°C
	
	if(temperature > 40)
		temperature = 40;
	else if(temperature < 0)
		temperature = 0;
	
	coap_notify_observers(&res_soil_temp);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	
	double req_temperature = temperature;			// Default in °C
	
	//Is conversion needed?
	if() {
		req_temperature = req_temperature * 9 / 5 + 32;
	}
	
	coap_set_header_content_format(response, TEXT_PLAIN);
	if()
		coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Soil temperature\":%f °C}", req_temperature));
	else
		coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Soil temperature\":%f °F}", req_temperature));
}
