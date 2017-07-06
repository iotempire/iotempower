package at.fhooe.mcm.hba;

import java.awt.BorderLayout;
import java.awt.Dimension;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

public class MainWindow {

	public static final int WINDOW_HEIGHT = 700;
	public static final int WINDOW_WIDTH = 1200;
	
	
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
        JFrame frame = new JFrame("Home&Building Automation Simulation");
        frame.setPreferredSize(new Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
         
        //Add content to the window.
        //frame.add(getTabbedPanePanel(), BorderLayout.CENTER);
       
        ContextManagement cm = new ContextManagement();
        frame.add(cm.getView(), BorderLayout.CENTER);
        
        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }
}
