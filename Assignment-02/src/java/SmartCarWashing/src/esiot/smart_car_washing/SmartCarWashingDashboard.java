package esiot.smart_car_washing;

import javax.swing.SwingUtilities;

class SmartCarWashingDashboard   {

	static SmartCarWashingDashboardView view = null;
	static LogView log = null;

	public static void main(String[] args) throws Exception {	
		/*
		if (args.length != 1){
			System.err.println("Args: <serial port>");
			System.exit(1);
		}*/
		SwingUtilities.invokeAndWait(() -> {
			view = new SmartCarWashingDashboardView();
			log = new LogView();
		});
		String portName = "/dev/cu.usbmodem1442401";
		SmartCarWashingDashboardController contr = new SmartCarWashingDashboardController(portName,view,log);
		view.registerController(contr);
		SwingUtilities.invokeLater(() -> {
			view.setVisible(true);
			log.setVisible(true);
		});
	}
}