package at.fhooe.mcm.hba;

import at.fhooe.mcm.hba.datastructure.Led;

public class LedFactory {

	private static int mLedCounter = 0;
	
	public static Led createLed() {
		mLedCounter++;
		
		Led led = new Led(mLedCounter);
		led.setEnabled(false);
		
		return led;
	}
	
	/*public static void increaseLedCounter() {
		mLedCounter++;
	}*/
	
	public static void decreaseLedCounter() {
		mLedCounter--;
	}
}
