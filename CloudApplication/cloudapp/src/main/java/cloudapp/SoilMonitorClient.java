package cloudapp;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.json.JSONObject;

public class SoilMonitorClient {
	
	private CoapClient soil_client;
	private CoapObserveRelation soil_relation;
	
	public boolean active = false;
	
	private String address;
	
	private float temperature = -1;
	private float humidity = -1;
	private float salinity = -1;
	private float pH = -1;
	
	private GUI gui;
	private int num_orch;
	
	/* Constructor initializes 2 observing relationships
	 * towards the resources of air humidity and temperature,
	 * updating the private attributes of the current class.
	 * */
	public SoilMonitorClient(String address, int pos, final GUI gui) {
		this.address = address;
		this.gui = gui;
		this.num_orch = pos;
		
		if(!this.gui.existsTab(num_orch))
			this.gui.addTab();
		
		soil_client = new CoapClient("[" + address + "]" + "/soil");

		soil_relation = soil_client.observe(
				new CoapHandler() {
					public void onLoad(CoapResponse response) {
							String content = response.getResponseText();
							
							if(content.length() > 0) {
								String cont_string = new String(content);
								JSONObject obj = new JSONObject(cont_string);
								humidity = obj.getFloat("Hum");
								gui.updateSolHum(num_orch,humidity);
								pH = obj.getFloat("pH");
								gui.updateSoilPH(num_orch,pH);
								salinity = obj.getFloat("Sal");
								gui.updateSoilSal(num_orch,salinity);
								temperature = obj.getFloat("Tmp");
								gui.updateSoilTemp(num_orch,temperature);
							}
					}
					public void onError() {
						System.err.println("-Failed--------");
					}
				}
		);
		
		soil_client.shutdown();
		
		active = true;
	}
	
	public float getTemperature() {
		return temperature;
	}
	
	public float getHumidity() {
		return humidity;
	}
	
	public float getPH() {
		return pH;
	}
	
	public float getSalinity() {
		return salinity;
	}
	
	/* For simulation reasons, we can ask to the soil temperature 
	 * resource to start increasing by calling a POST on the 
	 * resource, specifying the sign "+" that identifies
	 * the growth direction of the measurements */
	public void temperatureIncrease() {
		soil_client = new CoapClient("[" + address + "]" + "/soil");
		soil_client.post("temp_sign=+", MediaTypeRegistry.TEXT_PLAIN);
		soil_client.shutdown();
	}
	
	/* For simulation reasons, we can ask to the soil temperature 
	 * resource to start decreasing by calling a POST on the 
	 * resource, specifying the sign "-" that identifies
	 * the growth direction of the measurements */
	public void temperatureDecrease() {
		soil_client = new CoapClient("[" + address + "]" + "/soil");
		soil_client.post("temp_sign=-", MediaTypeRegistry.TEXT_PLAIN);
		soil_client.shutdown();
	}
	
	/* For simulation reasons, we can directly increase soil humidity
	 * of a given value (for when irrigating) */
	public void humidityIncrease(int inc_percentage) {
		soil_client = new CoapClient("[" + address + "]" + "/soil");
		soil_client.post("humidity_increase=" + 
				Integer.toString(inc_percentage), MediaTypeRegistry.TEXT_PLAIN);
		soil_client.shutdown();
	}
	
	/* For simulation reasons, we can directly increase soil pH levels
	 * of a given value (for when irrigating) */
	public void addPH(float inc_percentage) {
		soil_client = new CoapClient("[" + address + "]" + "/soil");
		soil_client.post("ph_increase=" + 
				Float.toString(inc_percentage), MediaTypeRegistry.TEXT_PLAIN);
		soil_client.shutdown();
	}
	
	/* For simulation reasons, we can directly increase soil pH levels
	 * of a given value (for when irrigating) */
	public void subPH(float dec_percentage) {
		soil_client = new CoapClient("[" + address + "]" + "/soil");
		soil_client.post("ph_decrease=" + 
				Float.toString(dec_percentage), MediaTypeRegistry.TEXT_PLAIN);
		soil_client.shutdown();
	}
	
	/* For simulation reasons, we can directly increase soil salinity
	 * of a given value (for when irrigating) */
	public void addSalinity(float amount) {
		soil_client = new CoapClient("[" + address + "]" + "/soil");
		soil_client.post("salinity_increase=" + 
				Float.toString(amount), MediaTypeRegistry.TEXT_PLAIN);
		soil_client.shutdown();
	}
	
	public void stopObserving() {
		soil_relation.proactiveCancel(); // to cancel observing
		active = false;
	}
}
