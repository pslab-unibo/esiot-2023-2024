package esiot.smart_car_washing;

public class SmartCarWashingDashboardController  {

	static final String MSG_MAINTENANCE_DONE 	= "ok";
	
	SerialCommChannel channel;
	SmartCarWashingDashboardView view;
	LogView logger;
	
	public SmartCarWashingDashboardController(String port, SmartCarWashingDashboardView view, LogView logger) throws Exception {
		this.view = view;
		this.logger = logger;
		
		channel = new SerialCommChannel(port,115200);		
		new MonitoringAgent(channel,view,logger).start();
			
		/* attesa necessaria per fare in modo che Arduino completi il reboot */
		
		System.out.println("Waiting Arduino for rebooting...");		
		Thread.sleep(4000);
		System.out.println("Ready.");		
	
	}
	
	public void notifyMaintenanceDone() {
		  channel.sendMsg(MSG_MAINTENANCE_DONE);
	}
	
}
