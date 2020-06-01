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

	coap_activate_resource(&res_gh_heating, "gh_heating");

	leds_on(LEDS_NUM_TO_MASK(LEDS_RED));

	PROCESS_END();
}
