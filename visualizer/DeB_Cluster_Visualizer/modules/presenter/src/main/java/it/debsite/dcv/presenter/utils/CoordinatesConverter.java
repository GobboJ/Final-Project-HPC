package it.debsite.dcv.presenter.utils;

import it.debsite.dcv.view.graph.GraphPrinter;
import lombok.Getter;
import org.jetbrains.annotations.NotNull;

import java.awt.geom.Rectangle2D;

/**
 * Utility class allowing to convert coordinates and length from the {@code x1-x2} plane into the
 * {@code x-y} plane of the image, and vice-versa.
 *
 * @author DeB
 * @version 1.1 2022-08-03
 * @since 1.0
 */
public class CoordinatesConverter {
    
    /**
     * Minimum value of {@code x1} represented in the image.
     */
    @Getter
    private final double minX1;
    
    /**
     * Maximum value of {@code x1} represented in the image.
     */
    @Getter
    private final double maxX1;
    
    /**
     * Minimum value of {@code x2} represented in the image.
     */
    @Getter
    private final double minX2;
    
    /**
     * Maximum value of {@code x2} represented in the image.
     */
    @Getter
    private final double maxX2;
    
    /**
     * Number of pixels that correspond to 1 unit of {@code x1}.
     */
    private final double x1Pixels;
    
    /**
     * Number of pixels that correspond to 1 unit of {@code x2}.
     */
    private final double x2Pixels;
    
    /**
     * Area of the frame that encloses the graph to print.
     */
    private final Rectangle2D graphPointsFrame;
    
    /**
     * Creates a new converter that converts coordinates and length in the {@code x1-x2} plane into
     * the corresponding coordinates or length in the {@code x-y} plane of the image.
     *
     * @param widestClusterArea The area of the widest cluster.
     * @param frame Area of the frame in the image where the graph will be printed.
     */
    @SuppressWarnings("FeatureEnvy")
    public CoordinatesConverter(
        @NotNull final ClusterArea widestClusterArea, @NotNull final Rectangle2D frame
    ) {
        // Extract the radius of the widest cluster
        final double widestClusterAreaRadius = widestClusterArea.getRadius();
        
        // Compute the box in the x1-x2 plane enclosing the widest cluster
        this.minX1 = widestClusterArea.getCenterX1() - widestClusterAreaRadius;
        this.maxX1 = widestClusterArea.getCenterX1() + widestClusterAreaRadius;
        this.minX2 = widestClusterArea.getCenterX2() - widestClusterAreaRadius;
        this.maxX2 = widestClusterArea.getCenterX2() + widestClusterAreaRadius;
        
        // Compute the maximum nesting level of clusters. This is used to avoid cluster ellipses
        // overlap in the graph, as well as avoids ellipses overflow the image frame.
        final int maxNestingLevel = widestClusterArea.getSubClustersNestingLevel();
        
        // Compute the frame that encloses only the points of the graph
        // noinspection ConstantExpression
        this.graphPointsFrame = new Rectangle2D.Double(
            frame.getX() + (maxNestingLevel * GraphPrinter.CLUSTERS_MARGIN),
            frame.getY() + (maxNestingLevel * GraphPrinter.CLUSTERS_MARGIN),
            frame.getWidth() - (maxNestingLevel * (GraphPrinter.CLUSTERS_MARGIN * 2)),
            frame.getHeight() - (maxNestingLevel * (GraphPrinter.CLUSTERS_MARGIN * 2))
        );
        
        // Compute units length
        this.x1Pixels = this.graphPointsFrame.getWidth() / (this.maxX1 - this.minX1);
        this.x2Pixels = this.graphPointsFrame.getHeight() / (this.maxX2 - this.minX2);
    }
    
    /**
     * Computes the {@code x} coordinate on the image corresponding to the specified {@code x1}
     * coordinate.
     *
     * @param x1Coordinate {@code x1} coordinate to convert to the corresponding {@code x}
     *     coordinate in the image.
     * @return The converted coordinate.
     */
    public double computeXCoordinateOf(final double x1Coordinate) {
        
        return this.graphPointsFrame.getX() + ((x1Coordinate - this.minX1) * this.x1Pixels);
    }
    
    /**
     * Computes the {@code y} coordinate on the image corresponding to the specified {@code x2}
     * coordinate.
     *
     * @param x2Coordinate {@code x2} coordinate to convert to the corresponding {@code y}
     *     coordinate in the image.
     * @return The converted coordinate.
     */
    public double computeYCoordinateOf(final double x2Coordinate) {
        
        return this.graphPointsFrame.getY() + ((this.maxX2 - x2Coordinate) * this.x2Pixels);
    }
    
    /**
     * Computes the length along the {@code x1} direction corresponding to the specified length in
     * pixels along the {@code x} direction.
     *
     * @param xLength Length in pixels along the {@code x} direction to convert into the
     *     {@code x1} equivalent.
     * @return The converted length.
     */
    public double computeX1LengthFromXLength(final double xLength) {
        
        // 1 : this.x1Pixels = ???? : xLength
        return xLength / this.x1Pixels;
    }
    
    /**
     * Computes the length along the {@code x2} direction corresponding to the specified length in
     * pixels along the {@code y} direction.
     *
     * @param yLength Length in pixels along the {@code y} direction to convert into the
     *     {@code x2} equivalent.
     * @return The converted length.
     */
    public double computeX2LengthFromYLength(final double yLength) {
        
        // 1 : this.x2Pixels = ???? : yLength
        return yLength / this.x2Pixels;
    }
    
    /**
     * Computes the number of pixels in the {@code y} direction corresponding to the specified
     * length along the {@code x1} direction.
     *
     * @param x1Length Length along the {@code x1} direction to convert into pixels.
     * @return The converted length.
     */
    public double computeXLengthFromX1Length(final double x1Length) {
        
        // 1 : this.x1Pixels = x1Length : ????
        return x1Length * this.x1Pixels;
    }
    
    /**
     * Computes the number of pixels in the {@code y} direction corresponding to the specified
     * length along the {@code x2} direction.
     *
     * @param x2Length Length along the {@code x2} direction to convert into pixels.
     * @return The converted length.
     */
    public double computeYLengthFromX2Length(final double x2Length) {
        
        // 1 : this.x2Pixels = x2Length : ????
        return x2Length * this.x2Pixels;
    }
}
