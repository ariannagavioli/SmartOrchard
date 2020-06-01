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

PROCESS(ripeness_camera_server, "Ripeness Camera CoAP Server");
AUTOSTART_PROCESSES(&ripeness_camera_server);

extern coap_resource_t res_ripeness_camera;

static int observing_interval = 60;
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(soil_monitor_server, ev, data){
	PROCESS_BEGIN();

	LOG_INFO("Starting Ripeness Camera Server\n");

	coap_activate_resource(&res_ripeness_camera, "soil_ripeness_camera");

	etimer_set(&et, CLOCK_SECOND * observing_interval);

	while(1) {

		PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_TIMER && data == &et){
			res_ripeness_camera.trigger();
			etimer_set(&et, CLOCK_SECOND * observing_interval);
		}

	}

	PROCESS_END();
}
