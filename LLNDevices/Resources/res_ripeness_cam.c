#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static char rgb_values[3]		= {0,1,0};		//Percentage of pixels being red, green and blue
static int increasing_color		= 1;			//Either 0, 1 or 2: indicates the color that is increasing in percentage
static int decreasing_color		= 2;			//Either 0, 1 or 2: indicates the color that is decreasing in percentage

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_ripeness_cam,
         "title=\"Ripeness Camera\";obs",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	static double random_value = 0.1 * rand() / RAND_MAX;	//returns a random value between 0 and 10%
	
	if(rgb_values[decreasing_color] >= random_value) {			//The whole random percentage of pixels flows from a color to another
		rgb_values[decreasing_color] -= random_value;
		rgb_values[increasing_color] += random_value;
	}
	else {														//The remaining percentage of pixels of one color turns into another color
		rgb_values[increasing_color] += rgb_values[decreasing_color];
		rgb_values[decreasing_color] = 0;

		static int new_dec_color = rand() % 3;					//Switching the decreasing color
		static int new_inc_color = rand() % 3;					//Switching the increasing color
	}
	
	coap_notify_observers(&res_ripeness_cam);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "{\"Ripeness Camera RGB percentage values\": [%f,%f,%f]}",
																					rgb_values[0], rgb_values[1], rgb_values[2]));
}


