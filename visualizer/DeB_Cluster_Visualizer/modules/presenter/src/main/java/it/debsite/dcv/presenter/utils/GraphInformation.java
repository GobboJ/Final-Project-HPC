package it.debsite.dcv.presenter.utils;

import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.view.GraphPrinter;
import lombok.Getter;

import java.awt.geom.Rectangle2D;
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
    
    private final Rectangle2D.Double area;
    
    public GraphInformation(
        final List<ClusterCircle> clusterCircleList,
        final Rectangle2D.Double area,
        final double imageWidth,
        final double imageHeight
    ) {
        
        if (clusterCircleList.isEmpty()) {
            throw new IllegalArgumentException("Empty cluster");
        }
        ClusterCircle widestCluster = clusterCircleList.get(0);
        
        // Compute the bounding box
        for (final ClusterCircle clusterPoint : clusterCircleList) {
            if (widestCluster.getRadius() < clusterPoint.getRadius()) {
                widestCluster = clusterPoint;
            }
        }
        this.minX1 = widestCluster.getCenterX1() - widestCluster.getRadius();
        this.maxX1 = widestCluster.getCenterX1() + widestCluster.getRadius();
        this.minX2 = widestCluster.getCenterX2() - widestCluster.getRadius();
        this.maxX2 = widestCluster.getCenterX2() + widestCluster.getRadius();
        
        final int maxNestingLevel = widestCluster.getNestingLevel();
        this.area = new Rectangle2D.Double(
            area.getX() + (maxNestingLevel * GraphPrinter.CLUSTER_MARGIN),
            area.getY() + (maxNestingLevel * GraphPrinter.CLUSTER_MARGIN),
            area.getWidth() - (maxNestingLevel * (GraphPrinter.CLUSTER_MARGIN * 2)),
            area.getHeight() - (maxNestingLevel * (GraphPrinter.CLUSTER_MARGIN * 2))
        );
        
        // Compute the steps
        this.xPixels = this.area.width / (this.maxX1 - this.minX1);
        this.yPixels = this.area.height / (this.maxX2 - this.minX2);
    }
    
    public double computeXOf(final double pointX1) {
        
        return this.area.x + ((pointX1 - this.minX1) * this.xPixels);
    }
    
    public double computeYOf(final double pointX2) {
        
        return this.area.y + ((this.maxX2 - pointX2) * this.yPixels);
    }
    
    public double computeX1From(final double x) {
        
        // (this.maxX1 - this.minX1) : width = ??? : x
        // 4 : 500 : ?? : 62.5
        
        return this.minX1 + (((x - this.area.x) * (this.maxX1 - this.minX1)) / this.area.width);
    }
    
    public double computeX2From(final double y) {
        
        // (this.maxX2 - this.minX2) : height = ??? : y
        
        return this.maxX2 - (((y - this.area.y) * (this.maxX2 - this.minX2)) / this.area.height);
    }
    
    public double computeX1FromLength(final double x) {
        
        // 1 : this.xPixels = ? : x
        
        return x / this.xPixels;
    }
    
    public double computeX2FromLength(final double y) {
        
        // 1 : this.yPixels = ? : y
        
        return y / this.yPixels;
    }
    
    public double computeLengthX(final double length) {
        
        // 1 : this.yPixels = ? : y
        
        return length * this.xPixels;
    }
    
    public double computeLengthY(final double length) {
        
        // 1 : this.yPixels = ? : y
        
        return length * this.yPixels;
    }
}
