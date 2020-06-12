# 🌳 SmartOrchard 💡
The project contains the implementation of both mote devices and of a cloud application used to interact with them. The motes were developed exploiting the contiki-ng operative system, while the cloud application was developed in java.

## The motes
There are three types of sensors in the Low Power and Lossy Network (LLN), which act as CoAP servers:
1. Air Monitor: monitors air humidity and temperature;
2. Soil Monitor: monitors soil humidity, temperature, pH levels and salinity;
3. Luminosity Monitor: monitors luminosity.

There are three types of actuators as well, acting as CoAP servers:
1. GreenHouse Heating: controls the heating system of the greenhouse;
2. GreenHouse Roller Shutter: controls the roller shutter of the greenhouse;
3. Irrigator: controls the irrigation system.

The motes first register to the cloud application and then are free to run as CoAP servers.

## The Cloud Application
The cloud application was developed in java, by exploiting the Californium library. It creates a graphic user interface for the user to interact with the orchid environment, by monitoring the sensors information and by giving the chance to control the actuators, by interacting with the CoAP servers.

## The Simulation
The simulation was performed by executing several instances of the motes in cooja simulator.
For more information about the simulation itself and the project details, please see [its report](https://github.com/ariannagavioli/SmartOrchard/blob/master/ProjectDocuments/SmartOrchard.pdf).
