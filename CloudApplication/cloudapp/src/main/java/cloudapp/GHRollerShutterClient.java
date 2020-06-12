package cloudapp;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.json.JSONObject;

public class GHRollerShutterClient {
	

	private CoapClient roller_shutter_client;
	
	private GUI gui;
	private int num_orch;
	
	private String address;
	
	public boolean active = false;

	public GHRollerShutterClient(String address, int pos, GUI gui) {
		this.address = address;
		this.gui = gui;
		this.num_orch = pos;
		
		if(!this.gui.existsTab(num_orch))
			this.gui.addTab();

		active = true;
	}
	
	public String getStatus() {
		roller_shutter_client = new CoapClient("[" + address + "]" + "/gh_roller_shutter");
		CoapResponse coap_response = roller_shutter_client.get();
		roller_shutter_client.shutdown();
		String response = coap_response.getResponseText();
		JSONObject obj = new JSONObject(response);
		String status = obj.getString("Roller_Shutter_Status");
		return status;
	}
	
	public void open() {
		roller_shutter_client = new CoapClient("[" + address + "]" + "/gh_roller_shutter");
		roller_shutter_client.post("action=open", MediaTypeRegistry.TEXT_PLAIN);
		roller_shutter_client.shutdown();
	}
	
	public void close() {
		roller_shutter_client = new CoapClient("[" + address + "]" + "/gh_roller_shutter");
		roller_shutter_client.post("action=closed", MediaTypeRegistry.TEXT_PLAIN);
		roller_shutter_client.shutdown();
	}
	
	public void shutDown() {
		active = false;
	}
	
}
