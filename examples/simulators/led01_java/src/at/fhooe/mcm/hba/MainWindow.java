package at.fhooe.mcm.hba;

import java.awt.BorderLayout;
import java.awt.Dimension;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

import at.fhooe.mcm.hba.listeners.ViewChangedListener;

public class MainWindow {

	public static final int WINDOW_HEIGHT = 300;
	public static final int WINDOW_WIDTH = 1000;
	
	private static JFrame mFrame;
	private static ContextManagement mContextManagement;
	
	
	public static void main(String [] args) throws Exception
	{		
		//Schedule a job for the event dispatch thread:
        //creating and showing this application's GUI.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                //Turn off metal's use of bold fonts
            	UIManager.put("swing.boldMetal", Boolean.FALSE);
            	createAndShowGUI();
            }
        });
	}
	
	/**
     * Create the GUI and show it. For thread safety,
     * this method should be invoked from
     * the event dispatch thread.
     */
    private static void createAndShowGUI() {
        //Create and set up the window.
    	mFrame = new JFrame("Home&Building Automation Simulation");
    	mFrame.setPreferredSize(new Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
    	mFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
       
    	mContextManagement = new ContextManagement();
    	mContextManagement.addListener(new ViewChangedListener() {
			
			@Override
			public void viewChanged() {
				mFrame.remove(mContextManagement.getView());
				mFrame.add(mContextManagement.getView(), BorderLayout.CENTER);
				repaintFrame();
			}
		});
        mFrame.add(mContextManagement.getView(), BorderLayout.CENTER);
        
        //Display the window.
        mFrame.pack();
        mFrame.setVisible(true);
    }
    
    private static void repaintFrame() {
        mFrame.invalidate();
        mFrame.validate();
        mFrame.repaint();
        mFrame.pack();
    }
}
