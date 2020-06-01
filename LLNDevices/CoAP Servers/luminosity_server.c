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

	coap_activate_resource(&res_luminosity, "soil_luminosity");

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
