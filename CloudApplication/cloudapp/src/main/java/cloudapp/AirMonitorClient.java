package cloudapp;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.json.JSONObject;

public class AirMonitorClient{
	
	private CoapClient temp_client;
	//private CoapClient hum_client;
	private CoapObserveRelation temp_relation;
	//private CoapObserveRelation hum_relation;
	
	private String server_address;
	
	private GUI gui;
	private int num_orch;
	
	public boolean active = false;
	
	private float temperature = -1;
	private float humidity = -1;
	
	/* Constructor initializes 2 observing relationships
	 * towards the resources of air humidity and temperature,
	 * updating the private attributes of the current class.
	 * */
	public AirMonitorClient(String address, int pos, final GUI gui, final int node_id) {
		server_address = address;
		this.gui = gui;
		this.num_orch = pos;
		
		if(!this.gui.existsTab(num_orch))
			this.gui.addTab();
		
		temp_client = new CoapClient("[" + address + "]" + "/air");
		
		temp_relation = temp_client.observe(
				new CoapHandler() {
					public void onLoad(CoapResponse response) {
							String content = response.getResponseText();
							
							if(content.length() > 0) {
								String cont_string = new String(content);
								JSONObject obj = new JSONObject(cont_string);
								temperature = obj.getFloat("Air temperature");
								humidity = obj.getFloat("Air humidity");
								gui.updateAirTemp(num_orch,temperature);
								gui.updateAirHum(num_orch,humidity);
							}
					}
					public void onError() {
						System.err.println("-Failed--------");
					}
				}
		);
		
		active = true;
		
		temp_client.shutdown();
	}
	
	public float getTemperature() {
		return temperature;
	}
	
	public float getHumidity() {
		return humidity;
	}
	
	/* For simulation reasons, we can ask to the air temperature 
	 * resource to start increasing by calling a POST on the 
	 * resource, specifying the sign "+" that identifies
	 * the growth direction of the measurements */
	public void temperatureIncrease() {
		temp_client = new CoapClient("[" + server_address + "]" + "/air_temp");
		temp_client.post("sign=+", MediaTypeRegistry.TEXT_PLAIN);
		temp_client.shutdown();
	}
	
	/* For simulation reasons, we can ask to the air temperature 
	 * resource to start decreasing by calling a POST on the 
	 * resource, specifying the sign "-" that identifies
	 * the growth direction of the measurements */
	public void temperatureDecrease() {
		temp_client = new CoapClient("[" + server_address + "]" + "/air_temp");
		temp_client.post("sign=-", MediaTypeRegistry.TEXT_PLAIN);
		temp_client.shutdown();
	}
	
	public void stopObserving() {
		temp_relation.proactiveCancel(); // to cancel observing
		//hum_relation.proactiveCancel();
		
		active = false;
	}
	
}
