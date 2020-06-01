#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double humidity = 85;		//humidity percentage

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_air_hum,
         "title=\"Air Humidity\";obs",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value;
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
	
	coap_notify_observers(&res_air_hum);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Air humidity\":%f %%}", humidity));
}

