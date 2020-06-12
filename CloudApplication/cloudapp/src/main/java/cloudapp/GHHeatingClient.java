package cloudapp;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.json.JSONObject;

public class GHHeatingClient {

	private CoapClient heating_client;
	
	private GUI gui;
	private int num_orch;
	
	public boolean active = false;
	
	private String address;

	public GHHeatingClient(String address, int pos, GUI gui) {
		this.address = address;
		this.gui = gui;
		this.num_orch = pos;
		
		if(!this.gui.existsTab(num_orch))
			this.gui.addTab();
		
		active = true;
	}
	
	public String getStatus() {
		heating_client = new CoapClient("[" + address + "]" + "/gh_heating");
		CoapResponse coap_response = heating_client.get();
		heating_client.shutdown();
		String response = coap_response.getResponseText();
		JSONObject obj = new JSONObject(response);
		String status = obj.getString("Heating Status");
		return status;
	}
	
	public void turnON() {
		heating_client = new CoapClient("[" + address + "]" + "/gh_heating");
		heating_client.post("mode=on", MediaTypeRegistry.TEXT_PLAIN);
		heating_client.shutdown();
	}
	
	public void turnOFF() {
		heating_client = new CoapClient("[" + address + "]" + "/gh_heating");
		heating_client.post("mode=off", MediaTypeRegistry.TEXT_PLAIN);
		heating_client.shutdown();
	}
	
	public void shutDown() {
		active = false;
	}
	
}
