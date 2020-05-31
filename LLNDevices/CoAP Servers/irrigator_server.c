#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "contiki.h"
#include "coap-engine.h"

/* Log configuration */
// #include "sys/log.h"
// #define LOG_MODULE "App"
// #define LOG_LEVEL LOG_LEVEL_APP

PROCESS(air_monitor_server, "Irrigator CoAP Server");
AUTOSTART_PROCESSES(&air_monitor_server);

extern coap_resource_t res_irrigator;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(air_monitor_server, ev, data){
	PROCESS_BEGIN();

	//LOG_INFO("Starting Air Monitoring Server\n");

	coap_activate_resource(&res_irrigator, "irrigator");


	PROCESS_END();
}
