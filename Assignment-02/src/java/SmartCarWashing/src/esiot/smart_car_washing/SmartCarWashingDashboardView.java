package esiot.smart_car_washing;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.LayoutManager;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.LinkedList;

import javax.swing.*;

class SmartCarWashingDashboardView extends JFrame implements ActionListener  {

	private JButton maintenanceDone;
	private JTextField numWashes;
	private JTextField currentTemperature;
	
	private JTextField plantState;
	private SmartCarWashingDashboardController controller;	
	
	public SmartCarWashingDashboardView(){
		super(".:: Smart Car Washing ::.");
		setSize(600,150);

		JPanel mainPanel = new JPanel();
		mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
		mainPanel.add(Box.createRigidArea(new Dimension(0,20)));

		JPanel infoLine = new JPanel();
		infoLine.setLayout(new BoxLayout(infoLine, BoxLayout.LINE_AXIS));
		plantState = new JTextField("--");
		plantState.setEditable(false);
		plantState.setPreferredSize(new Dimension(200,15));
		infoLine.add(new JLabel("Plant State:"));
		infoLine.add(plantState);
		numWashes = new JTextField("--");
		numWashes.setEditable(false);
		numWashes.setPreferredSize(new Dimension(100,15));
		infoLine.add(new JLabel("Num washes:"));
		infoLine.add(numWashes);
		currentTemperature = new JTextField("--");
		currentTemperature.setEditable(false);
		currentTemperature.setPreferredSize(new Dimension(200,15));
		infoLine.add(new JLabel("Current Temperature:"));
		infoLine.add(currentTemperature);
		
		mainPanel.add(infoLine);
		mainPanel.add(Box.createRigidArea(new Dimension(0,20)));
		mainPanel.setPreferredSize(new Dimension(200,20));

		JPanel buttonPanel = new JPanel();
		maintenanceDone = new JButton("Maintenance Done");
		maintenanceDone.setEnabled(false);
		maintenanceDone.addActionListener(this);
		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.X_AXIS));	    
		buttonPanel.add(maintenanceDone);
		
		mainPanel.add(buttonPanel);
		mainPanel.add(Box.createRigidArea(new Dimension(0,20)));
		setContentPane(mainPanel);	
		
		addWindowListener(new WindowAdapter(){
			public void windowClosing(WindowEvent ev){
				System.exit(-1);
			}
		});
	}

	public void registerController(SmartCarWashingDashboardController contr){
		this.controller = contr;
	}

	public void setPlantState(String msg){
		SwingUtilities.invokeLater(() -> {
			plantState.setText(msg); 
		});
	}

	public void setNumWashes(int nWashes){
		SwingUtilities.invokeLater(() -> {
			numWashes.setText("" + nWashes);
		});
	}

	public void setCurrentTemperature(double temp){
		SwingUtilities.invokeLater(() -> {
			currentTemperature.setText("" + temp);
		});
	}
	
	public void enableMaintenance() {
		SwingUtilities.invokeLater(() -> {
			maintenanceDone.setEnabled(true);
		});
	}

	/*
	public void updateSamples(Sample[] samples) {
		try {
			SwingUtilities.invokeAndWait(() -> {
				graphicPanel.updateSamples(samples);
			});
		} catch (Exception ex) {}
	}

	public void startMaintenanceForRefilling() {
		SwingUtilities.invokeLater(() -> {
			refill.setEnabled(true);
		});
	}

	public void startMaintenanceForRecovering() {
		SwingUtilities.invokeLater(() -> {
			recover.setEnabled(true);
		});
	}
	*/
	
	public void actionPerformed(ActionEvent ev){
		  try {
			  if (ev.getSource() == maintenanceDone){
				  controller.notifyMaintenanceDone();
				  maintenanceDone.setEnabled(false);
			  } 
		  } catch (Exception ex){
			  ex.printStackTrace();

		  }
	}
}
