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
	static double random_value;
	random_value = rand() % 3;				
	random_value *= 0.1;				//returns a random value between 0 and 0.2
	static double increasing_sign;
	increasing_sign = rand() % 2;	//returns either 0 or 1

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


/* For simulation purposes, a post call will adjust the pH resource, increasing it or decreasing it*/
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	const char *increase = NULL;
	const char *decrease = NULL;

	static int length1;
	static int length2;
	
	length1 = coap_get_post_variable(request, "increase", &increase);
	length2 = coap_get_post_variable(request, "decrease", &decrease);

	if(length1) {

		static int amount = atoi(increase);
		humidity += amount;

		coap_set_status_code(response, CHANGED_2_04);

	} else if(length2) {

		static int amount = atoi(decrease);
		humidity -= amount;

		coap_set_status_code(response, CHANGED_2_04);

	} else {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}

}


