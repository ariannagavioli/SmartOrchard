#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "contiki.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/etimer.h"
#include "node-id.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/uip.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uiplib.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

// Registration Server IP and resource path
#define SERVER_EP "coap://[fd00::1]:5683"
char *service_url = "/registration";

// Define a handler to handle the response from the server
void client_chunk_handler(coap_message_t *response) {
	const uint8_t *chunk;
	if(response == NULL) {
		puts("Request timed out");
		return;
	}
	int len = coap_get_payload(response, &chunk);
	printf("Successfully registered.\n", len, (char *)chunk);
}

PROCESS(air_monitor_server, "Air Monitor CoAP Server");
AUTOSTART_PROCESSES(&air_monitor_server);

extern coap_resource_t res_air;

static int configuration_interval;
static int observing_interval; 
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(air_monitor_server, ev, data){
	PROCESS_BEGIN();

	LOG_INFO("Starting Air Monitoring Server\n");
	
	observing_interval = 5;
	configuration_interval = 0.5;
	
	coap_activate_resource(&res_air, "air");
	
	/* REGISTRATION PHASE */
	//Retrieve global ip
	etimer_set(&et, CLOCK_SECOND * configuration_interval);
	while(!NETSTACK_ROUTING.node_is_reachable()){
		PROCESS_WAIT_EVENT();
		if(ev == PROCESS_EVENT_TIMER && data == &et){
			etimer_set(&et, CLOCK_SECOND * configuration_interval);
		}
	}
	static char ip_address[128];
	memset(ip_address, 0, sizeof(ip_address));
	static int i;
	static int found = 0;
	uint8_t state;
	for(i = 0; i < UIP_DS6_ADDR_NB && !found; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if(uip_ds6_if.addr_list[i].isused && (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
			found++;
			uiplib_ipaddr_snprint(ip_address, sizeof(ip_address), &(uip_ds6_if.addr_list[i].ipaddr));
		}
	}
	// Set the registration information
	static char msg[128];
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "{\"Functionality\":\"air_monitor\",\"ID\":%d,\"IP\":\"%s\"}",node_id,ip_address);
	
	/* Registration to the cloud */
	static coap_endpoint_t server_ep;
	static coap_message_t request[1];
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
	coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
	coap_set_header_uri_path(request, service_url);
	coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
	COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
	

	etimer_set(&et, CLOCK_SECOND * observing_interval);

	while(1) {

		PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_TIMER && data == &et){
			res_air.trigger();
			etimer_set(&et, CLOCK_SECOND * observing_interval);
		}

	}

	PROCESS_END();
}
