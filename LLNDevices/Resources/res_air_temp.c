#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double temperature	= 25;
static int increasing_sign	= 1;

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_air_temp,
         "title=\"Air Temperature\";obs",
         res_get_handler,
         res_post_handler,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value;
	random_value = rand() % 21;
	random_value *= 0.1;									//random value is n*0.1°C and between (0°C,2°C), where n = {0, 1, ..., 20}
	
	temperature += (increasing_sign * random_value);		//temperature can change up to 2°C

	if(temperature > 35)
		temperature = 35;
	else if(temperature < 0)
		temperature = 0;
	
	coap_notify_observers(&res_air_temp);
}


/* Returns the temperature value either expressed in Celsius or Fahrenheit */
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	const char *degree = NULL;
	static double req_temperature;
	req_temperature = temperature;			// Default in °C
	static int isCelsius = 1;
	
	//Is conversion needed?
	if(coap_get_query_variable(request, "degree", &degree)) {
		static char degree_val[1];
		memcpy(degree_val, degree, 1);
		if(!strcmp("F",degree_val))	{					// If client requested it to be in °F
			req_temperature = req_temperature * 9 / 5 + 32;
			isCelsius = 0;
		}
	}
	
	coap_set_header_content_format(response, TEXT_PLAIN);
	if(isCelsius)
		coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Air temperature\":%f}", req_temperature));
	else
		coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Air temperature\":%f}", req_temperature));
}

/* For simulation purposes, a post call will adjust the temperature resource, such as its increase or decrease */
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	const char *sign = NULL;
	static int length;
	
	length = coap_get_post_variable(request, "sign", &sign);
	
	if(length) {
		
		static char msg[2];
		memset(msg, 0, sizeof(msg));
		memcpy(msg, sign, length);
		
		/* If the client asks to for the decreasing temperature
		 * to start increase, then change the increasing sign
		 */
		if(!strcmp(msg, "+") && increasing_sign < 0) {
			increasing_sign = 1;
		}
		
		/* If the client asks to for the increasing temperature
		 * to start decrease, then change the increasing sign
		 */
		else if(!strcmp(msg, "-") && increasing_sign > 0) {
			increasing_sign = -1;
		}

		coap_set_status_code(response, CHANGED_2_04);
			
	} else {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}
}


