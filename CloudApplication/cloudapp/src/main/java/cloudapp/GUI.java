package cloudapp;

import javax.swing.*;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.LinkedList;
import java.util.List;

class Tab extends JPanel implements ActionListener{

	private static final long serialVersionUID = 1L;
	private CloudApplication app;
	private int num_tab;
	
	String name;
	JPanel text_panel = new JPanel();
	JPanel button_panel = new JPanel();
	JTextField air_temp = new JTextField();
	JTextField air_hum = new JTextField();
	JTextField soil_temp = new JTextField();
	JTextField soil_hum = new JTextField();
	JTextField soil_ph = new JTextField();
	JTextField soil_sal = new JTextField();
	JTextField luminosity = new JTextField();
	
	JLabel name_sens_1 = new JLabel("Air Temperature: ");
	JLabel name_sens_2 = new JLabel("Air Humidity: ");
	JLabel name_sens_3 = new JLabel("Soil Temperature: ");
	JLabel name_sens_4 = new JLabel("Soil Humidity: ");
	JLabel name_sens_5 = new JLabel("Soil pH: ");
	JLabel name_sens_6 = new JLabel("Soil Salinity: ");
	JLabel name_sens_7 = new JLabel("Luminosity: ");

	JButton irrigate = new JButton("Irrigate");
	JButton gh_heating = new JButton("GreenHouse Heating");
	JButton gh_roll_shutters = new JButton("Roller Shutters");
	JButton fertilize = new JButton("Fertilize");

	public Tab(String name, CloudApplication app, int num_tab) {
		super();
		this.app = app;
		this.name = name;
		this.num_tab = num_tab;
		text_panel.setLayout(new GridBagLayout());
		GridBagConstraints c = new GridBagConstraints();
		c.gridx = 0;
		c.gridy = 0;
		c.fill = GridBagConstraints.HORIZONTAL;
		text_panel.add(name_sens_1, c);
		c.gridy = 1;
		text_panel.add(name_sens_2, c);
		c.gridy = 2;
		text_panel.add(name_sens_3, c);
		c.gridy = 3;
		text_panel.add(name_sens_4, c);
		c.gridy = 4;
		text_panel.add(name_sens_5, c);
		c.gridy = 5;
		text_panel.add(name_sens_6, c);
		c.gridy = 6;
		text_panel.add(name_sens_7, c);
		c.gridy = 7;	
		c.gridx = 1;
		c.gridy = 0;
		c.ipady = 20;
		c.ipadx = 200;
		air_temp.setEditable(false);
		text_panel.add(air_temp, c);
		c.gridy = 1;
		air_hum.setEditable(false);
		text_panel.add(air_hum, c);
		c.gridy = 2;
		soil_temp.setEditable(false);
		text_panel.add(soil_temp, c);
		c.gridy = 3;
		soil_hum.setEditable(false);
		text_panel.add(soil_hum, c);
		c.gridy = 4;
		soil_ph.setEditable(false);
		text_panel.add(soil_ph, c);
		c.gridy = 5;
		soil_sal.setEditable(false);
		text_panel.add(soil_sal, c);
		c.gridy = 6;
		luminosity.setEditable(false);
		text_panel.add(luminosity, c);
		
		button_panel.setLayout(new GridLayout(2, 2));
		irrigate.addActionListener(this);
		gh_heating.addActionListener(this);
		gh_roll_shutters.addActionListener(this);
		fertilize.addActionListener(this);
		irrigate.setBackground(Color.WHITE);
		gh_heating.setBackground(Color.RED);
		gh_roll_shutters.setBackground(Color.RED);
		irrigate.setBackground(Color.WHITE);
		button_panel.add(irrigate);
		button_panel.add(gh_heating);
		button_panel.add(gh_roll_shutters);
		button_panel.add(fertilize);
		this.setLayout(new BorderLayout());
		text_panel.setSize(400,400);
		this.add(text_panel, BorderLayout.NORTH);
		this.add(button_panel, BorderLayout.SOUTH);

	}


	public void actionPerformed(ActionEvent event) {
		String action = event.getActionCommand();
        if (action.equals("Irrigate")) {
            app.irrigate(num_tab);
            if(!(irrigate.getBackground().equals(Color.CYAN)))
            	irrigate.setBackground(Color.CYAN);
            else
            	irrigate.setBackground(Color.WHITE);
        }
        else if (action.equals("GreenHouse Heating")) {
            app.heating(num_tab);
            if(!(gh_heating.getBackground().equals(Color.GREEN)))
            	gh_heating.setBackground(Color.GREEN);
            else
            	gh_heating.setBackground(Color.RED);
        }
        else if (action.equals("Roller Shutters")) {
            app.shut(num_tab);
            if(!(gh_roll_shutters.getBackground().equals(Color.GREEN)))
            	gh_roll_shutters.setBackground(Color.GREEN);
            else
            	gh_roll_shutters.setBackground(Color.RED);
        }
        else if (action.equals("Fertilize")) {
            app.fertilize(num_tab);
        }
	}


	public void updateAirTemp(float temperature) {
		air_temp.setText(Float.toString(temperature) + " °C");
	}


	public void updateAirHum(float humidity) {
		air_hum.setText(Float.toString(humidity) + " %");
	}


	public void updateLum(float lum_val) {
		luminosity.setText(Float.toString(lum_val) + " lux");
	}


	public void updateSoilTemp(float temperature) {
		soil_temp.setText(Float.toString(temperature) + " °C");
	}


	public void updateSoilHum(float humidity) {
		soil_hum.setText(Float.toString(humidity) + " %");
	}


	public void updateSoilPH(float pH) {
		soil_ph.setText(Float.toString(pH));
	}


	public void updateSoilSal(float salinity) {
		soil_sal.setText(Float.toString(salinity) + " dS/m");
	}
}


public class GUI {
	JFrame f;
	JTabbedPane tp = new JTabbedPane();
	int current_orchs = -1;
	List<Tab> tabs = new LinkedList<Tab>();
	
	private CloudApplication app;
	
	GUI(CloudApplication app){  
		this.app = app;
	    f = new JFrame();   
	    tp.setBounds(50, 50, 500, 350);  
	    f.add(tp);  
	    f.setSize(600, 450);  
	    f.setLayout(null);  
	    f.setVisible(true);  
		f.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
	}
	
	public void addTab() {
		current_orchs++;
		int num_tab = current_orchs + 1;
		Tab tab = new Tab("Orchid "+Integer.toString(num_tab), app, (num_tab-1));
		tabs.add(tab);
		tp.add("Orchid "+Integer.toString(current_orchs), tab);
	}

	public boolean existsTab(int pos) {
		if(current_orchs < pos)
			return false;
		return true;
	}

	public void updateAirTemp(int pos_tab, float temperature) {
		Tab current_tab = tabs.get(pos_tab);
		current_tab.updateAirTemp(temperature);
	}

	public void updateAirHum(int pos_tab, float humidity) {
		Tab current_tab = tabs.get(pos_tab);
		current_tab.updateAirHum(humidity);
	}

	public void updateLuminosity(int pos_tab, float luminosity) {
		Tab current_tab = tabs.get(pos_tab);
		current_tab.updateLum(luminosity);
	}

	public void updateSoilTemp(int pos_tab, float temperature) {
		Tab current_tab = tabs.get(pos_tab);
		current_tab.updateSoilTemp(temperature);
	}

	public void updateSolHum(int pos_tab, float humidity) {
		Tab current_tab = tabs.get(pos_tab);
		current_tab.updateSoilHum(humidity);
	}

	public void updateSoilPH(int pos_tab, float pH) {
		Tab current_tab = tabs.get(pos_tab);
		current_tab.updateSoilPH(pH);
	}

	public void updateSoilSal(int pos_tab, float salinity) {
		Tab current_tab = tabs.get(pos_tab);
		current_tab.updateSoilSal(salinity);
	}

}