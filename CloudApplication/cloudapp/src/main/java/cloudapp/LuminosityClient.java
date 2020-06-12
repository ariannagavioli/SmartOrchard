package cloudapp;

import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.json.JSONObject;

public class LuminosityClient {
	private CoapClient lum_client;
	private CoapObserveRelation lum_relation;
	
	private String server_address;
	
	private boolean shutters_closed = true;
	
	public boolean active = false;
	
	private GUI gui;
	private int num_orch;
	
	private float luminosity = -1;

	/* Constructor initializes a observing relationship
	 * towards the luminosity resource, updating the private
	 * attribute of the current class.
	 * */
	public LuminosityClient(String address, int pos, final GUI gui) {
		server_address = address;
		this.gui = gui;
		this.num_orch = pos;
		
		if(!this.gui.existsTab(num_orch))
			this.gui.addTab();
		
		lum_client = new CoapClient("[" + address + "]" + "/luminosity");
		
		lum_relation = lum_client.observe(
				new CoapHandler() {
					public void onLoad(CoapResponse response) {
							String content = response.getResponseText();
							
							if(content.length() > 0) {
								String cont_string = new String(content);
								JSONObject obj = new JSONObject(cont_string);
								luminosity = obj.getFloat("Luminosity");
								if(luminosity >= 106000) 		//Midday -> in the afternoon light decreases
									luminosityDecrease();
								else if(luminosity <= 0.5)		//Midnight -> in the morning light increases
									luminosityIncrease();
								if(shutters_closed && luminosity > 2000)		//Roller shutters are down -> less light perceived
									luminosity = luminosity - 2000;
								gui.updateLuminosity(num_orch, luminosity);
							}
					}
					public void onError() {
						System.err.println("-Failed--------");
					}
				}
		);
		active = true;
		lum_client.shutdown();
	}
	
	public float getLuminosity() {
		return luminosity;
	}
	
	/* For simulation reasons, we can ask to the luminosity
	 * resource to start increasing by calling a POST on the 
	 * resource, specifying the sign "+" that identifies
	 * the growth direction of the measurements */
	public void luminosityIncrease() {
		lum_client = new CoapClient("[" + server_address + "]" + "/luminosity");
		lum_client.post("sign=+", MediaTypeRegistry.TEXT_PLAIN);
		lum_client.shutdown();
	}
	
	/* For simulation reasons, we can ask to the luminosity 
	 * resource to start decreasing by calling a POST on the 
	 * resource, specifying the sign "-" that identifies
	 * the growth direction of the measurements */
	public void luminosityDecrease() {
		lum_client = new CoapClient("[" + server_address + "]" + "/luminosity");
		lum_client.post("sign=-", MediaTypeRegistry.TEXT_PLAIN);
		lum_client.shutdown();
	}
	
	public void stopObserving() {
		lum_relation.proactiveCancel(); // to cancel observing
		
		active = false;
	}
	
	public void setShutters(boolean b) {
		shutters_closed = b;
	}

	public void decreaseLight() {
		if(luminosity > 2000) {
			luminosity -= 2000;
			gui.updateLuminosity(num_orch, luminosity);
		}
		else  {
			luminosity = 0;
			gui.updateLuminosity(num_orch, luminosity);
		}
	}

	public void increaseLight() {
		if(luminosity <105000 ) {
			luminosity += 2000;
			gui.updateLuminosity(num_orch, luminosity);
		}
		else {
			luminosity = 107000;
			gui.updateLuminosity(num_orch, luminosity);
		}
	}
}
