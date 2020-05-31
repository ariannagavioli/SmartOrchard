#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double temperature	= 25;
static int increasing_sign	= 1;

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

/* A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated */
EVENT_RESOURCE(res_air_temp,
         "title=\"Air Temperature\";obs",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value = 2 * rand() / RAND_MAX;				//returns a random value between 0 and 2
	
	temperature += (increasing_sign * random_value);				//temperature can change up to 2°C

	if(temperature > 35)
		temperature = 35;
	else if(temperature < 0)
		temperature = 0;
	
	coap_notify_observers(&res_air_temp);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	const char *degree = NULL;
	double req_temperature = temperature;			// Default in °C
	static int isCelsius = 1;
	
	//Is conversion needed?
	if(coap_get_post_variable(request, "degree", &degree)) {
		if(!strcmp("F",degree))	{					// If client requested it to be in °F
			req_temperature = req_temperature * 9 / 5 + 32;
			isCelsius = 0;
		}
	}
	
	coap_set_header_content_format(response, TEXT_PLAIN);
	if(isCelsius)
		coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Air temperature\":%f °C}", req_temperature));
	else
		coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Air temperature\":%f °F}", req_temperature));
}

