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

PROCESS(gh_heating_server, "GreenHouse Heating CoAP Server");
AUTOSTART_PROCESSES(&gh_heating_server);

extern coap_resource_t res_gh_heating;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gh_heating_server, ev, data){
	PROCESS_BEGIN();

	LOG_INFO("Starting GreenHouse Heating Server\n");
	
	/* REGISTRATION TO THE CLOUD */
	static coap_endpoint_t server_ep;
	static coap_message_t request[1];
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
	coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
	coap_set_header_uri_path(request, service_url);
	// Set the registration information
	static char msg[128];
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "{\"Functionality\":\"gh_heating\",\"Node Number\":%d}",node_id);
	coap_set_payload(request, (uint8_t *)msg, strlen(msg));
	COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
	
	/* ACTIVATION OF THE SERVER */
	coap_activate_resource(&res_gh_heating, "gh_heating");

	leds_on(LEDS_NUM_TO_MASK(LEDS_RED));

	PROCESS_END();
}
