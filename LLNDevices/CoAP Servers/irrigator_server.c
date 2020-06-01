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

PROCESS(air_monitor_server, "Irrigator CoAP Server");
AUTOSTART_PROCESSES(&air_monitor_server);

extern coap_resource_t res_irrigator;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(air_monitor_server, ev, data){
	PROCESS_BEGIN();

	LOG_INFO("Starting Irrigator Server\n");

	coap_activate_resource(&res_irrigator, "irrigator");

	leds_on(LEDS_NUM_TO_MASK(LEDS_RED));

	PROCESS_END();
}
