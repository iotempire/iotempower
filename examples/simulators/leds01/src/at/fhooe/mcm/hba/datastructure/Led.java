package at.fhooe.mcm.hba.datastructure;

import javax.swing.JCheckBox;

public class Led extends JCheckBox {

	private static final long serialVersionUID = 1L;
	
	private static final String LED_TOPIC = "simulation/led%s/set";
	
	private int mNr;
	
	public Led(int nr) {
		this.mNr = nr;
		
		this.setText("Led" + mNr);
	}

	public int getNr() {
		return mNr;
	}
	
	public String getTopic() {
		return String.format(LED_TOPIC, mNr);
	}
}
