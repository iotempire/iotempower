package at.fhooe.mcm.hba.datastructure;

public class Brightness implements IElement {
	
	private int mLux;

	public Brightness(int lux) {
		super();
		this.mLux = lux;
	}

	@Override
	public String toString() {
		return "Brightness:\n\n" + mLux + " lux";
	}	
}
