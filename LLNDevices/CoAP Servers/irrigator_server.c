#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "contiki.h"
#include "coap-engine.h"
#include "dev/leds.h"
#include "sys/etimer.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

PROCESS(irrigator_server, "Irrigator CoAP Server");
AUTOSTART_PROCESSES(&irrigator_server);

extern coap_resource_t res_irrigator;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(irrigator_server, ev, data){
	PROCESS_BEGIN();

	LOG_INFO("Starting Irrigator Server\n");
	
	/* REGISTRATION TO THE CLOUD */
	static coap_endpoint_t server_ep;
	static coap_message_t request[1];
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
	coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
	coap_set_header_uri_path(request, service_url);
	// Set the registration information
	static char msg[128];
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "{\"Functionality\":\"irrigator\",\"Node Number\":%d}",node_id);
	coap_set_payload(request, (uint8_t *)msg, strlen(msg));
	COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
	
	/* ACTIVATION OF THE SERVER */
	coap_activate_resource(&res_irrigator, "irrigator");

	leds_on(LEDS_NUM_TO_MASK(LEDS_RED));

	PROCESS_END();
}
