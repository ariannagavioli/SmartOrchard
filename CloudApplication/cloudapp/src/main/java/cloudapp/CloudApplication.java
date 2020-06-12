package cloudapp;

import org.json.*;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.CoapServer;
import org.eclipse.californium.core.coap.CoAP.ResponseCode;
import org.eclipse.californium.core.server.resources.CoapExchange;

public class CloudApplication {	
	
	private static CoapServer server = new CoapServer();
	
	private GUI gui = new GUI(this);
	
	/* Clients list based on the server type and ordered based on the server node id */
	private AirMonitorClient[] air_monitor_list = new AirMonitorClient[6];
	private GHHeatingClient[] gh_heating_list = new GHHeatingClient[6];
	private GHRollerShutterClient[] gh_roll_shutter_list = new GHRollerShutterClient[6];
	private IrrigatorClient[] irrigator_list = new IrrigatorClient[6];
	private LuminosityClient[] luminosity_list = new LuminosityClient[6];
	private SoilMonitorClient[] soil_monitor_list = new SoilMonitorClient[6];
	
	private class RegistrationResource extends CoapResource {
		public RegistrationResource(String name) {
			super(name);
		}
		
		/* The only operation possible is post: a client can only register itself */
		public void handlePOST(CoapExchange exchange) {
			
			/* Read the JSON registration file to read its functionalities */
			String request = exchange.getRequestText();
			try {
				JSONObject obj = new JSONObject(request);
				String client_functionality = obj.getString("Functionality");
				int client_id = obj.getInt("ID");
				String server_addr = obj.getString("IP").toString();
				
				/* Add the corresponding type of client to the clients' list 
				 * client_id % 2 -> indicates which orchid the node belongs to,
				 * considering the specific 2 orchids of the cooja simulation */
				if(client_functionality.equals("air_monitor")) {
					AirMonitorClient client = new AirMonitorClient(server_addr,
							(client_id % 2),gui,client_id);
					air_monitor_list[(client_id % 2)] = client;
					exchange.respond(ResponseCode.CREATED);
					System.out.println("Air monitor with id " + 
							Integer.toString(client_id) + " succesfully registered");
				}
				else if(client_functionality.equals("gh_heating")) {
					GHHeatingClient client = new GHHeatingClient(server_addr,
							(client_id % 2),gui);
					gh_heating_list[(client_id % 2)] =  client;
					exchange.respond(ResponseCode.CREATED);
					System.out.println("GreenHouse Heating with id " + 
							Integer.toString(client_id) + " succesfully registered");
				} 
				else if(client_functionality.equals("gh_r_shutt")) {
					GHRollerShutterClient client = new GHRollerShutterClient(server_addr,
							(client_id % 2),gui);
					gh_roll_shutter_list[(client_id % 2)] = client;
					exchange.respond(ResponseCode.CREATED);
					System.out.println("GreenHouse Roller Shutter with id " + 
							Integer.toString(client_id) + " succesfully registered");
				} 
				else if(client_functionality.equals("irrigator")) {
					IrrigatorClient client = new IrrigatorClient(server_addr,
							(client_id % 2),gui);
					irrigator_list[(client_id % 2)] = client;
					exchange.respond(ResponseCode.CREATED);
					System.out.println("Irrigator with id " + 
							Integer.toString(client_id) + " succesfully registered");
				} 
				else if(client_functionality.equals("luminosity")) {
					LuminosityClient client = new LuminosityClient(server_addr,
							(client_id % 2),gui);
					luminosity_list[(client_id % 2)] = client;
					exchange.respond(ResponseCode.CREATED);
					System.out.println("Luminosity monitor with id " + 
							Integer.toString(client_id) + " succesfully registered");
				} 
				else if(client_functionality.equals("soil_monitor")) {
					SoilMonitorClient client = new SoilMonitorClient(server_addr,
							(client_id % 2),gui);
					soil_monitor_list[(client_id % 2)] = client;
					exchange.respond(ResponseCode.CREATED);
					System.out.println("Soil monitor with id " + 
							Integer.toString(client_id) + " succesfully registered");
				} else {
					exchange.respond(ResponseCode.METHOD_NOT_ALLOWED);
				}
			}
			catch(Exception e) {
				System.err.print(e);
			}
		}
		
	}

	public static void main(String[] args) {
		CloudApplication app = new CloudApplication();
		server.add(app.new RegistrationResource("registration"));
		server.start();
	}

	public void irrigate(int num_orch) {
		if(irrigator_list[num_orch].getStatus().equals("off")) {
			irrigator_list[num_orch].turnON();
			soil_monitor_list[num_orch].humidityIncrease(70);
			
		} else {
			irrigator_list[num_orch].turnOFF();
		}
	}

	public void heating(int num_orch) {
		if(gh_heating_list[num_orch].getStatus().equals("off")) {
			gh_heating_list[num_orch].turnON();
			air_monitor_list[num_orch].temperatureIncrease();
		} else {
			gh_heating_list[num_orch].turnOFF();
		}
	}

	public void shut(int num_orch) {
		if(gh_roll_shutter_list[num_orch].getStatus().equals("closed")) {
			gh_roll_shutter_list[num_orch].open();					//Update actuator
			luminosity_list[num_orch].setShutters(true);			//Less light perceived -> properly simulate it
			luminosity_list[num_orch].increaseLight();
		} else {
			gh_roll_shutter_list[num_orch].close();					//Update actuator
			luminosity_list[num_orch].setShutters(true);			//The full amount of light is perceived
			luminosity_list[num_orch].decreaseLight();
		}
	}

	public void fertilize(int num_orch) {
		soil_monitor_list[num_orch].addSalinity((float) 1);
		float ph = soil_monitor_list[num_orch].getPH();
		if(ph < 6)
			soil_monitor_list[num_orch].addPH(6-ph);
		else
			soil_monitor_list[num_orch].subPH(ph-6);
	}

}