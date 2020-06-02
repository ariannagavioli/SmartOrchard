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

PROCESS(air_monitor_server, "Soil Monitor CoAP Server");
AUTOSTART_PROCESSES(&air_monitor_server);

extern coap_resource_t res_soil_temp;
extern coap_resource_t res_soil_hum;
extern coap_resource_t res_soil_ph;
extern coap_resource_t res_soil_sal;

static int observing_interval = 30;
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(soil_monitor_server, ev, data){
	PROCESS_BEGIN();

	LOG_INFO("Starting Soil Monitoring Server\n");
	
	/* REGISTRATION TO THE CLOUD */
	static coap_endpoint_t server_ep;
	static coap_message_t request[1];
	coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &server_ep);
	coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
	coap_set_header_uri_path(request, service_url);
	// Set the registration information
	static char msg[128];
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "{\"Functionality\":\"soil_monitor\",\"Node Number\":%d}",node_id);
	coap_set_payload(request, (uint8_t *)msg, strlen(msg));
	COAP_BLOCKING_REQUEST(&server_ep, request, client_chunk_handler);
	
	/* ACTIVATION OF THE SERVER */
	coap_activate_resource(&res_soil_temp, "soil_temp");
	coap_activate_resource(&res_soil_hum, "soil_hum");
	coap_activate_resource(&res_soil_ph, "soil_ph");
	coap_activate_resource(&res_soil_sal, "soil_sal");


	etimer_set(&et, CLOCK_SECOND * observing_interval);

	while(1) {

		PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_TIMER && data == &et){
			res_soil_hum.trigger();
			res_soil_temp.trigger();
			res_soil_ph.trigger();
			res_soil_sal.trigger();
			etimer_set(&et, CLOCK_SECOND * observing_interval);
		}

	}

	PROCESS_END();
}
