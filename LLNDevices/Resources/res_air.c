#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double temperature	= 25;
static double humidity = 85;		//humidity percentage
static int temp_increasing_sign	= 1;

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_air,
         "title=\"Air Measurements\";obs",
         res_get_handler,
         res_post_handler,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value;
	random_value = rand() % 21;
	random_value *= 0.1;									//random value is n*0.1°C and between (0°C,2°C), where n = {0, 1, ..., 20}
	
	temperature += (temp_increasing_sign * random_value);		//temperature can change up to 2°C

	if(temperature > 35)
		temperature = 35;
	else if(temperature < 0)
		temperature = 0;
	
	random_value = rand() % 11;
	random_value *= 0.1;								//random value is a value n*0.1, where n = {0, 1, ..., 10}

	static int random_sign;
	random_sign = rand() % 2;							//returns either 0 or 1
	
	if(random_sign > 0) {
		humidity += random_value;
	} else {
		humidity -= random_value;
	}
	
	if(humidity > 100)
		humidity = 100;
	else if(humidity < 0)
		humidity = 0;
	
	coap_notify_observers(&res_air);
}


/* Returns the temperature value either expressed in Celsius or Fahrenheit */
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Air temperature\":%f,\"Air humidity\":%f}", temperature,humidity));
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
		
		/* If the client asks for the decreasing temperature
		 * to start increase, then change the increasing sign
		 */
		if(!strcmp(msg, "+") && temp_increasing_sign < 0) {
			temp_increasing_sign = 1;
		}
		
		/* If the client asks for the increasing temperature
		 * to start decrease, then change the increasing sign
		 */
		else if(!strcmp(msg, "-") && temp_increasing_sign > 0) {
			temp_increasing_sign = -1;
		}

		coap_set_status_code(response, CHANGED_2_04);
			
	} else {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}
}


