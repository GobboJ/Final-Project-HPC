package it.debsite.dcv.presenter.utils;

import it.debsite.dcv.presenter.GraphPresenter;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-10
 * @since version date
 */
public class ScaleComputer {
    
    private static final double[] SCALE = new double[]{1, 2, 4, 5, 10};
    
    public static double getScale(final double xBlock) {
        
        // xBlock = 2.3 --> 4
        // xBlock = 4.5 --> 5
        // xBlock = 7.9 --> 10
        
        double scale = 1;
        int scaleIndex = 0;
        double newScale = ScaleComputer.SCALE[0];
        
        if (xBlock > 1) {
            while (newScale < xBlock) {
                scaleIndex++;
                if (scaleIndex == ScaleComputer.SCALE.length) {
                    scale *= ScaleComputer.SCALE[ScaleComputer.SCALE.length - 1];
                    scaleIndex = 0;
                    newScale = scale * ScaleComputer.SCALE[0];
                } else {
                    newScale = scale * ScaleComputer.SCALE[scaleIndex];
                }
            }
            return newScale;
        }
        
        double prevScale = newScale;
        while (newScale > xBlock) {
            scaleIndex++;
            if (scaleIndex == ScaleComputer.SCALE.length) {
                scale /= ScaleComputer.SCALE[ScaleComputer.SCALE.length - 1];
                scaleIndex = 0;
                newScale = scale / ScaleComputer.SCALE[0];
            } else {
                prevScale = newScale;
                newScale = scale / ScaleComputer.SCALE[scaleIndex];
            }
        }
        
        return prevScale;
    }
}
