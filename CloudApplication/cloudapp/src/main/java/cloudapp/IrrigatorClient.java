package cloudapp;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.json.JSONObject;

public class IrrigatorClient {

	private CoapClient irrigator_client;
	
	private GUI gui;
	private int num_orch;
	private String address;
	
	public boolean active = false;

	public IrrigatorClient(String address, int pos, GUI gui) {
		this.gui = gui;
		this.num_orch = pos;
		this.address = address;
		
		if(!this.gui.existsTab(num_orch))
			this.gui.addTab();
		
		active = true;
	}
	
	public String getStatus() {
		irrigator_client = new CoapClient("[" + address + "]" + "/irrigator");
		CoapResponse coap_response = irrigator_client.get();
		String response = coap_response.getResponseText();
		irrigator_client.shutdown();
		JSONObject obj = new JSONObject(response);
		String status = obj.getString("Irrigator_Status");
		return status;
	}
	
	public void turnON() {
		irrigator_client = new CoapClient("[" + address + "]" + "/irrigator");
		irrigator_client.post("mode=on", MediaTypeRegistry.TEXT_PLAIN);
		irrigator_client.shutdown();
	}
	
	public void turnOFF() {
		irrigator_client = new CoapClient("[" + address + "]" + "/irrigator");
		irrigator_client.post("mode=off", MediaTypeRegistry.TEXT_PLAIN);
		irrigator_client.shutdown();
	}
	
	public void shutDown() {
		active = false;
	}
}
