package at.fhooe.mcm.hba.datastructure;

public class Temperature implements IElement {
	
	public static final String TMP_TOPIC = "temperature/sensor/garage/set";
	public static final String TMP_RISE = "rising";
	public static final String TMP_FALL = "falling";
	public static final String TMP_CONST = "const";
	private int mTemp;

	public Temperature(int mTemp) {
		super();
		this.mTemp = mTemp;
	}

	@Override
	public String toString() {
		//return "Temperature:\n\n" + mTemp + " temp";
		return String.valueOf(mTemp);
	}	
}