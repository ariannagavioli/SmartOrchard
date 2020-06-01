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

PROCESS(gh_roller_shutter_server, "GreenHouse Roller Shutters CoAP Server");
AUTOSTART_PROCESSES(&gh_roller_shutter_server);

extern coap_resource_t res_gh_roller_shutter;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gh_roller_shutter_server, ev, data){
	PROCESS_BEGIN();

	LOG_INFO("Starting GreenHouse Roller Shutters Server\n");

	coap_activate_resource(&res_gh_roller_shutter, "gh_roller_shutter");

	leds_on(LEDS_NUM_TO_MASK(LEDS_RED));

	PROCESS_END();
}
