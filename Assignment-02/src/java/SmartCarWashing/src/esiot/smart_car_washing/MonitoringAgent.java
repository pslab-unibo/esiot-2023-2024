package esiot.smart_car_washing;

import java.util.LinkedList;

public class MonitoringAgent extends Thread {

	SerialCommChannel channel;
	SmartCarWashingDashboardView view;
	LogView logger;
	
	static final String PLANT_PREFIX 	=  "cw:";
	static final String LOG_PREFIX 	=  "lo:";
	static final String MSG_STATE 		= "st:";
	
	static final String[] stateNames = {"Idle", "Checking-In", "Checking-Out","Ready for Washing", "Washing", "In maintenance" }; 
	
	public MonitoringAgent(SerialCommChannel channel, SmartCarWashingDashboardView view, LogView log) throws Exception {
		this.view = view;
		this.logger = log;
		this.channel = channel;
	}
	
	public void run(){
		boolean inMaintenance = true;
		while (true){
			try {
				String msg = channel.receiveMsg();
				if (msg.startsWith(PLANT_PREFIX)){
					String cmd = msg.substring(PLANT_PREFIX.length()); 
					// logger.log("new command: "+cmd);				
					
					if (cmd.startsWith(MSG_STATE)){
						try {
							String args = cmd.substring(MSG_STATE.length()); 
							
							String[] elems = args.split(":");
							if (elems.length >= 3) {
								int stateCode = Integer.parseInt(elems[0]);
								int numWashes = Integer.parseInt(elems[1]);
								double temp = Double.parseDouble(elems[2]);
		
								view.setNumWashes(numWashes);
								view.setCurrentTemperature(temp);
								view.setPlantState(stateNames[stateCode]);
								
								if (stateCode == 5 && !inMaintenance) { // maintenance
									inMaintenance = true;
									view.enableMaintenance();
								} else if (stateCode != 5 && inMaintenance) {
									inMaintenance = false;
									
								}
							}
						} catch (Exception ex) {
							ex.printStackTrace();
							System.err.println("Error in msg: " + cmd);
						}
					}
				} else if (msg.startsWith(LOG_PREFIX)){
					this.logger.log(msg.substring(LOG_PREFIX.length()));
				}
			} catch (Exception ex){
				ex.printStackTrace();
			}
		}
	}

}
