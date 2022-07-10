package it.debsite.dcv.presenter.utils;

import it.debsite.dcv.model.ClusterPoint;
import javafx.geometry.Insets;
import lombok.Getter;

import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class GraphInformation {
    
    @Getter
    private final double minX1;
    
    @Getter
    private final double maxX1;
    
    @Getter
    private final double minX2;
    
    @Getter
    private final double maxX2;
    
    /**
     * Pixel for 1 x.
     */
    private final double xPixels;
    
    /**
     * Pixels for 1 y.
     */
    private final double yPixels;
    
    private final Insets insets;
    
    public GraphInformation(
        final List<ClusterPoint> points,
        final Insets insets,
        final double canvasWidth,
        final double canvasHeight
    ) {
        
        this.insets = insets;
        if (points.isEmpty()) {
            throw new IllegalArgumentException("Empty cluster");
        }
        final ClusterPoint firstPoint = points.get(0);
        double currentMinX1 = firstPoint.getX1();
        double currentMaxX1 = firstPoint.getX1();
        double currentMinX2 = firstPoint.getX2();
        double currentMaxX2 = firstPoint.getX2();
        
        // Compute the bounding box
        for (final ClusterPoint clusterPoint : points) {
            
            currentMinX1 = StrictMath.min(currentMinX1, clusterPoint.getX1());
            currentMaxX1 = StrictMath.max(currentMaxX1, clusterPoint.getX1());
            currentMinX2 = StrictMath.min(currentMinX2, clusterPoint.getX2());
            currentMaxX2 = StrictMath.max(currentMaxX2, clusterPoint.getX2());
        }
        this.minX1 = currentMinX1;
        this.maxX1 = currentMaxX1;
        this.minX2 = currentMinX2;
        this.maxX2 = currentMaxX2;
        // Compute the steps
        final double availableWidth =
            StrictMath.max(canvasWidth - insets.getLeft() - insets.getRight(), 0);
        final double availableHeight =
            StrictMath.max(canvasHeight - insets.getTop() - insets.getBottom(), 0);
        
        this.xPixels = availableWidth / (this.maxX1 - this.minX1);
        this.yPixels = availableHeight / (this.maxX2 - this.minX2);
    }
    
    public double computeXOf(final double pointX1) {
        
        return this.insets.getLeft() + (pointX1 * this.xPixels);
    }
    
    public double computeYOf(final double pointX2) {
        
        return this.insets.getTop() + ((this.maxX2 - pointX2) * this.yPixels);
    }
}
