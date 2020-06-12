#include <stdlib.h>
#include <stdio.h>
#include "contiki.h"
#include "coap-engine.h"

static double humidity			= 85;			//humidity percentage
static double soil_ph			= 6;
static double soil_salinity		= 1;			//Expressed in dS/m
static double temperature		= 25;
static int temp_increasing_sign	= 1;			//Either 1 or -1 depending on the period of the day (can be modified through a put on the resource)


static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_soil,
         "title=\"Soil Measurements\";obs",
         res_get_handler,
         res_post_handler,
         NULL,
         NULL,
         res_event_handler);
         
static void res_event_handler(void) {
	/* HUMIDITY */
	static double random_value;
	random_value = rand() % 11;
	random_value *= 0.1;				//random value is a value n*0.1, where n = {0, 1, ..., 10}

	humidity -= random_value;			//humidity naturally decreases	
	
	// Keeping humidity as a percentage measure
	if(humidity > 100)
		humidity = 100;
	else if(humidity < 0)
		humidity = 0;
	
	/* PH */
	random_value = rand() % 3;				
	random_value *= 0.1;				//returns a random value between 0 and 0.2
	static double ph_increasing_sign;
	ph_increasing_sign = rand() % 2;	//returns either 0 or 1

	if(ph_increasing_sign > 0)
		soil_ph += random_value;									//soil_ph can increase of up to 0.2
	else
		soil_ph -= random_value;									//soil_ph can decrease of up to 0.2

	// Keeping soil_ph in the right range of the scale
	if(soil_ph > 14)
		soil_ph = 14;
	else if(soil_ph < 0)
		soil_ph = 0;
		
	/* SALINITY */
	random_value = rand() % 101;
	random_value *= 0.001;						//returns a random value between 0 and 0.1

	soil_salinity -= random_value;				//soil_salinity can diminuish up to 1 dS/m

	// Keeping soil_salinity in a reasonable range of the scale
	if(soil_salinity > 20)
		soil_salinity = 20;
	else if(soil_salinity < 0)
		soil_salinity = 0;
		
	/* TEMPERATURE */
	random_value = rand() % 21;
	random_value *= 0.1;										//random value is n*0.1°C and between (0°C,2°C), where n = {0, 1, ..., 20}
	
	temperature += (temp_increasing_sign * random_value);		//temperature can change up to 2°C
	
	if(temperature > 40)
		temperature = 40;
	else if(temperature < 0)
		temperature = 0;
	
	coap_notify_observers(&res_soil);
}

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {	
	coap_set_header_content_format(response, TEXT_PLAIN);
	coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size,
		"{\"Hum\":%f,\"pH\":%f,\"Sal\":%f,\"Tmp\":%f}",
		humidity, soil_ph, soil_salinity, temperature));
}

/* For simulation purposes, a post call will adjust the humidity resource, such as its increase or decrease */
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	const char *humidity_increase = NULL;
	const char *ph_increase = NULL;
	const char *ph_decrease = NULL;
	const char *salinity_increase = NULL;
	const char *temp_sign = NULL;
	static int length0;
	static int length1;
	static int length2;
	static int length3;
	static int length4;
	
	length0 = coap_get_post_variable(request, "humidity_increase", &humidity_increase);
	length1 = coap_get_post_variable(request, "ph_increase", &ph_increase);
	length2 = coap_get_post_variable(request, "ph_decrease", &ph_decrease);
	length3 = coap_get_post_variable(request, "salinity_increase", &salinity_increase);
	length4 = coap_get_post_variable(request, "temp_sign", &temp_sign);
	
	/* Increasing humidity of a given value */
	if(length0) {
		static int amount;
		amount = atoi(humidity_increase);
		humidity += amount;
		coap_set_status_code(response, CHANGED_2_04);
	}	
	/* Increasing the pH of a given value */
	else if(length1) {
		static float amount;
		amount = atof(ph_increase);
		soil_ph += amount;
		coap_set_status_code(response, CHANGED_2_04);
	}
	/* Decreasing the pH of a given value */
	else if(length2) {
		static float amount;
		amount = atof(ph_decrease);
		soil_ph -= amount;
		coap_set_status_code(response, CHANGED_2_04);
	}
	/* Increasing the salinity of a given value */
	else if(length3) {
		static float amount;
		amount = atof(salinity_increase);
		soil_salinity += amount;
		coap_set_status_code(response, CHANGED_2_04);
	}
	/* Changing the increasing sign of the temperature */
	else if(length4) {
		
		static char msg[2];
		memset(msg, 0, sizeof(msg));
		memcpy(msg, temp_sign, length3);

		if(!strcmp(msg, "+")) {
			temp_increasing_sign = 1;
		} else if(!strcmp(msg, "-")) {
			temp_increasing_sign = -1;
		} else
			coap_set_status_code(response, BAD_REQUEST_4_00);
	}
	
	else {
		coap_set_status_code(response, BAD_REQUEST_4_00);
	}
}
