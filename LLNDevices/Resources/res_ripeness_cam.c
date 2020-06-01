#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static float rgb_values[3]		= {0,1,0};		//Percentage of pixels being red, green and blue
static int increasing_color		= 1;			//Either 0, 1 or 2: indicates the color that is increasing in percentage
static int decreasing_color		= 2;			//Either 0, 1 or 2: indicates the color that is decreasing in percentage

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_ripeness_cam,
         "title=\"Ripeness Camera\";obs",
         res_get_handler,
         res_post_handler,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value;
	random_value = rand() % 101;	
	random_value *= 0.001;				//returns a random value between 0 and 10%
	
	if(rgb_values[decreasing_color] >= random_value) {			//The whole random percentage of pixels flows from a color to another
		rgb_values[decreasing_color] -= random_value;
		rgb_values[increasing_color] += random_value;
	}
	else {														//The remaining percentage of pixels of one color turns into another color
		rgb_values[increasing_color] += rgb_values[decreasing_color];
		rgb_values[decreasing_color] = 0;

		decreasing_color = rand() % 3;					//Switching the decreasing color
		while(increasing_color == decreasing_color)
			increasing_color = rand() % 3;				//Switching the increasing color
	}
	
	coap_notify_observers(&res_ripeness_cam);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Ripeness Camera RGB percentage values\": [%f,%f,%f]}",
																					rgb_values[0], rgb_values[1], rgb_values[2]));
}

/* For simulation purposes, a post call will adjust the rgb average values. E.g. when fruit is picked */
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	const char *r = NULL;
	const char *g = NULL;
	const char *b = NULL;
	static int length1;
	static int length2;
	static int length2;
	
	length1 = coap_get_post_variable(request, "r", &r);
	length2 = coap_get_post_variable(request, "g", &g);
	length3 = coap_get_post_variable(request, "b", &b);
	
	if(length1) {
		
		static int amount = atoi(r);
		if(amount < 0)
			amount = 1;
		amount = amount % 1;
		rgb_values[0] = amount;

		coap_set_status_code(response, CHANGED_2_04);

	}
	if(length2) {

		static int amount = atoi(g);
		if(amount < 0)
			amount = 1;
		amount = amount % 1;
		rgb_values[1] = amount;

		coap_set_status_code(response, CHANGED_2_04);

	}
	if(length3) {

		static int amount = atoi(b);
		if(amount < 0)
			amount = 1;
		amount = amount % 1;
		rgb_values[2] = amount;

		coap_set_status_code(response, CHANGED_2_04);

	} 

	if(!length1 && !length2 && !length3) {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}
}

