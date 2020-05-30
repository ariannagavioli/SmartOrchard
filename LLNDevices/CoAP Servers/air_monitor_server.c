#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "contiki.h"
#include "coap-engine.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

PROCESS(air_monitor_server, "Air Monitor CoAP Server");
AUTOSTART_PROCESSES(&air_monitor_server);

extern coap_resource_t res_air_temp;
extern coap_resource_t res_air_hum;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(air_monitor_server, ev, data){
	PROCESS_BEGIN();

	LOG_INFO("Starting Air Monitoring Server\n");

	coap_activate_resource(&res_air_temp, "air_temp");
	coap_activate_resource(&res_air_hum, "air_hum");

	PROCESS_END();
}