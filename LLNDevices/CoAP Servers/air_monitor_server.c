#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "contiki.h"
#include "coap-engine.h"
#include "sys/etimer.h"
#include "node-id.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define DEBUG 1

// Registration Server IP and resource path
#define SERVER_EP "coap://127.0.0.1/5683"
char *service_url = "/registration";

// Define a handler to handle the response from the server
void client_chunk_handler(coap_message_t *response) {
	const uint8_t *chunk;
	if(response == NULL) {
		puts("Request timed out");
		return;
	}
	int len = coap_get_payload(response, &chunk);
	printf("|%.*s", len, (char *)chunk);
}

PROCESS(air_monitor_server, "Air Monitor CoAP Server");
AUTOSTART_PROCESSES(&air_monitor_server);

extern coap_resource_t res_air_temp;
extern coap_resource_t res_air_hum;

static int observing_interval = 2;
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(air_monitor_server, ev, data){
	PROCESS_BEGIN();

	LOG_INFO("Starting Air Monitoring Server\n");
	
	/* REGISTRATION TO THE CLOUD */
	static coap_endpoint_t server_ep;
	static coap_message_t request[1];
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
	coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
	coap_set_header_uri_path(request, service_url);
	// Set the registration information
	static char msg[128];
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "{\"Functionality\":\"air_monitor\",\"Node Number\":%d}",node_id);
	coap_set_payload(request, (uint8_t *)msg, strlen(msg));
	COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
	
	/* ACTIVATION OF THE SERVER */
	coap_activate_resource(&res_air_temp, "air_temp");
	coap_activate_resource(&res_air_hum, "air_hum");

	etimer_set(&et, CLOCK_SECOND * observing_interval);

	while(1) {

		PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_TIMER && data == &et){
			res_air_hum.trigger();
			res_air_temp.trigger();
			etimer_set(&et, CLOCK_SECOND * observing_interval);
		}

	}

	PROCESS_END();
}
