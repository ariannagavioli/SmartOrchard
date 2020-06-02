#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "contiki.h"
#include "coap-engine.h"
#include "sys/etimer.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

PROCESS(luminosity_server, "Luminosity CoAP Server");
AUTOSTART_PROCESSES(&luminosity_server);

extern coap_resource_t res_luminosity;

static int observing_interval = 50;
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(soil_monitor_server, ev, data){
	PROCESS_BEGIN();

	LOG_INFO("Starting Luminosity Server\n");
	
	/* REGISTRATION TO THE CLOUD */
	static coap_endpoint_t server_ep;
	static coap_message_t request[1];
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
	coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
	coap_set_header_uri_path(request, service_url);
	// Set the registration information
	static char msg[128];
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "{\"Functionality\":\"luminosity\",\"Node Number\":%d}",node_id);
	coap_set_payload(request, (uint8_t *)msg, strlen(msg));
	COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
	
	/* ACTIVATION OF THE SERVER */
	coap_activate_resource(&res_luminosity, "luminosity");

	etimer_set(&et, CLOCK_SECOND * observing_interval);

	while(1) {

		PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_TIMER && data == &et){
			res_luminosity.trigger();
			etimer_set(&et, CLOCK_SECOND * observing_interval);
		}

	}

	PROCESS_END();
}
