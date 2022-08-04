package it.debsite.dcv.presenter;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.presenter.utils.ClusterArea;
import it.debsite.dcv.presenter.utils.CoordinatesConverter;
import it.debsite.dcv.presenter.utils.DistanceMatrix;
import it.debsite.dcv.presenter.utils.GridCellLengthComputer;
import it.debsite.dcv.view.GraphGridLine;
import it.debsite.dcv.view.graph.GraphCluster;
import it.debsite.dcv.view.graph.GraphPoint;
import it.debsite.dcv.view.graph.GraphPrinter;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.UnmodifiableView;

import java.awt.geom.Rectangle2D;
import java.awt.image.RenderedImage;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;

/**
 * Class taking care of performing all the required operations to print the graph of points and
 * clusters to an image.
 *
 * @author DeB
 * @version 1.2 2022-08-04
 * @since 1.0
 */
public class GraphPresenter {
    
    /**
     * A utility interface that wraps the call to a method responsible for converting a coordinate
     * in the {@code x1-x2} plane to the {@code x-y} plane.
     *
     * @author DeB
     * @version 1.0 2022-08-03
     * @since 1.0
     */
    @FunctionalInterface
    private static interface PointToPixelConverter {
        
        /**
         * Converts the specified coordinate in the {@code x1-x2} to the corresponding coordinate in
         * the {@code x-y} plane.
         *
         * @param pointValue Value of the coordinate in the {@code x1-x2} to convert,
         * @return The converted coordinate.
         */
        public double convert(double pointValue);
    }
    
    /**
     * Container holding the information of the area of a cluster enclosed within another cluster.
     *
     * @author DeB
     * @version 1.0 2022-08-03
     * @since 1.0
     */
    @NoArgsConstructor
    @Getter
    @Setter
    private static class SubClusterArea {
        
        /**
         * Radius of the area, in the {@code x1-x2} plane, that encloses all the points belonging to
         * the cluster this object represents.
         */
        private double radius;
        
        /**
         * Depth of the deepest sub-cluster (i.e., the cluster with more sub-clusters) contained in
         * the cluster this object represents.
         */
        private int nestingLevel;
    }
    
    /**
     * Container holding the information of the areas of a set of clusters, together with the
     * cluster with the widest area.
     *
     * @author DeB
     * @version 1.0 2022-08-03
     * @since 1.0
     */
    @AllArgsConstructor
    @Getter
    private static class ClustersAreas {
        
        /**
         * List of all the clusters areas.
         */
        @NotNull
        private final List<@NotNull ClusterArea> clusterAreas;
        
        /**
         * Cluster with the widest area.
         */
        @NotNull
        private final ClusterArea widestClusterArea;
        
    }
    
    /**
     * Space between the borders of the image and the frame enclosing the graph.
     */
    private static final double FRAME_MARGIN = 80;
    
    /**
     * Space between the graph and the frame that encloses it.
     */
    private static final double GRAPH_MARGIN = 20;
    
    /**
     * Preferred length, in pixels, of each cell of the grid. This value is just a hint used when
     * computing the real size of the grid cells. In any cases, the size of the graph grid cells
     * cannot be smaller than {@code GRID_CELL_LENGTH} &times; {@code GRID_CELL_LENGTH} and cannot
     * be bigger than {@code GRID_CELL_LENGTH * 2} &times; {@code GRID_CELL_LENGTH * 2}.
     */
    private static final double GRID_CELL_LENGTH = 60;
    
    /**
     * Format used when printing the grid lines.
     */
    private static final DecimalFormat GRID_LINES_VALUES_FORMAT;
    
    /*
     * Initialize the format to use when printing the values on the gird lines.
     */
    static {
        GRID_LINES_VALUES_FORMAT = new DecimalFormat();
        GraphPresenter.GRID_LINES_VALUES_FORMAT.setMaximumFractionDigits(2);
    }
    
    /**
     * Width, in pixels, of the image where the graph will be printed.
     */
    private final double imageWidth;
    
    /**
     * Height, in pixels, of the image where the graph will be printed.
     */
    private final double imageHeight;
    
    /**
     * Printer that prints the graph.
     */
    private final GraphPrinter graphPrinter;
    
    /**
     * Creates anew presenter that takes care of drawing the graph of points and clusters.
     *
     * @param image Image where the graph will be printed.
     * @param graphPrinter Printer used to print the graph on the specified image.
     */
    public GraphPresenter(
        @NotNull final RenderedImage image, final GraphPrinter graphPrinter
    ) {
        
        this.imageWidth = image.getWidth();
        this.imageHeight = image.getHeight();
        this.graphPrinter = graphPrinter;
    }
    
    /**
     * Draws the graph with the specified points and clusters to the image.
     *
     * @param points Points to draw in the graph.
     * @param clusters Clusters to draw in the graph.
     */
    @SuppressWarnings("FeatureEnvy")
    public void drawGraph(
        @NotNull @UnmodifiableView final List<? extends @NotNull ClusterPoint> points,
        @NotNull @UnmodifiableView final List<? extends @NotNull Cluster> clusters
    ) {
        
        // Compute the areas of the clusters
        final ClustersAreas clustersAreas =
            GraphPresenter.computeAllClustersAreas(points, clusters);
        
        // Compute the bounds of the frame enclosing the graph. This adds a bit of margin between
        // the real frame and the graph.
        
        // noinspection ConstantExpression
        final Rectangle2D graphFrame = new Rectangle2D.Double(GraphPresenter.FRAME_MARGIN,
            GraphPresenter.FRAME_MARGIN,
            this.imageWidth - (GraphPresenter.FRAME_MARGIN * 2),
            this.imageHeight - (GraphPresenter.FRAME_MARGIN * 2)
        );
        
        // Create the converter for the coordinates
        final CoordinatesConverter coordinatesConverter =
            new CoordinatesConverter(clustersAreas.getWidestClusterArea(), graphFrame);
        
        // Compute the coordinates on the x-y plane of the cluster points
        final Collection<GraphPoint> graphPoints = new ArrayList<>(points.size());
        // noinspection Convert2streamapi
        for (final ClusterPoint point : points) {
            // Convert the point coordinates
            // noinspection ObjectAllocationInLoop
            final GraphPoint graphPoint = new GraphPoint(point.getName(),
                coordinatesConverter.computeXCoordinateOf(point.getX1Coordinate()),
                coordinatesConverter.computeYCoordinateOf(point.getX2Coordinate())
            );
            graphPoints.add(graphPoint);
        }
        
        // Compute the coordinates on the x-y plane of the clusters
        final Collection<@NotNull GraphCluster> graphClusters = new ArrayList<>(clusters.size());
        for (final ClusterArea clusterArea : clustersAreas.getClusterAreas()) {
            // Nesting level of the cluster, used to compute the margin between enclosed clusters
            final int nestingLevel = clusterArea.getSubClustersNestingLevel();
            
            // Convert the cluster coordinates, taking care of adding an appropriate margin
            // between a cluster and the parent one
            // noinspection ObjectAllocationInLoop
            final GraphCluster graphCluster = new GraphCluster(clusterArea.getName(),
                coordinatesConverter.computeXCoordinateOf(clusterArea.getCenterX1()),
                coordinatesConverter.computeYCoordinateOf(clusterArea.getCenterX2()),
                coordinatesConverter.computeXLengthFromX1Length(clusterArea.getRadius()) +
                    (nestingLevel * GraphPrinter.CLUSTERS_MARGIN),
                coordinatesConverter.computeYLengthFromX2Length(clusterArea.getRadius()) +
                    (nestingLevel * GraphPrinter.CLUSTERS_MARGIN)
            
            );
            graphClusters.add(graphCluster);
        }
        
        // Compute the bounds of the frame that contains the entire graph together with all the
        // grid lines
        // noinspection ConstantExpression
        final Rectangle2D.Double frame =
            new Rectangle2D.Double(graphFrame.getX() - GraphPresenter.GRAPH_MARGIN,
                graphFrame.getY() - GraphPresenter.GRAPH_MARGIN,
                graphFrame.getWidth() + (GraphPresenter.GRAPH_MARGIN * 2),
                graphFrame.getHeight() + (GraphPresenter.GRAPH_MARGIN * 2)
            );
        
        // Compute all the grid lines
        final List<@NotNull GraphGridLine> verticalGridLines =
            GraphPresenter.computeVerticalGridLines(coordinatesConverter, frame);
        final List<@NotNull GraphGridLine> horizontalGridLines =
            GraphPresenter.computeHorizontalGridLines(coordinatesConverter, frame);
        final GraphGridLine xAxisGridLine =
            new GraphGridLine("x", coordinatesConverter.computeYCoordinateOf(0));
        final GraphGridLine yAxisGridLine =
            new GraphGridLine("y", coordinatesConverter.computeXCoordinateOf(0));
        
        // Draw the graph
        this.graphPrinter.drawGraph(frame,
            verticalGridLines,
            horizontalGridLines,
            xAxisGridLine,
            yAxisGridLine,
            graphPoints,
            graphClusters
        );
    }
    
    /**
     * Computes all the areas that enclose the points belonging to the clusters.
     *
     * @param points Points to draw in the graph.
     * @param clusters Clusters to draw in the graph.
     * @return The computed areas, together with the widest one.
     */
    @NotNull
    private static ClustersAreas computeAllClustersAreas(
        @NotNull @UnmodifiableView final List<? extends @NotNull ClusterPoint> points,
        @NotNull @UnmodifiableView final List<? extends @NotNull Cluster> clusters
    ) {
        
        // Compute the distance matrix
        final DistanceMatrix matrix = new DistanceMatrix(points);
        
        // Order the clusters by the number of points that belongs to it
        final List<@NotNull Cluster> orderedClusters = new ArrayList<>(clusters);
        orderedClusters.sort(Comparator.comparingInt(cluster -> cluster.getClusterPoints().size()));
        
        // Map that maps the identifiers of the clusters with their corresponding cluster area
        final Map<String, ClusterArea> clusterToAreas = new HashMap<>();
        
        // Cluster with the widest area
        ClusterArea widestClusterArea = new ClusterArea("", 0, 0, 0, 0);
        double widestClusterAreaRadius = 0;
        
        // Object used as containers and instantiated here to be reused
        final SubClusterArea leftSubClusterArea = new SubClusterArea();
        final SubClusterArea rightSubClusterArea = new SubClusterArea();
        
        // Compute all the cluster areas
        final List<@NotNull ClusterArea> clusterAreas = new ArrayList<>(clusters.size());
        for (final Cluster cluster : orderedClusters) {
            // Compute the area of the cluster
            // noinspection ObjectAllocationInLoop
            final ClusterArea clusterArea = GraphPresenter.computeClusterArea(cluster,
                matrix,
                clusterToAreas,
                leftSubClusterArea,
                rightSubClusterArea
            );
            clusterAreas.add(clusterArea);
            clusterToAreas.put(cluster.getIdentifier(), clusterArea);
            // Update the widest cluster area
            if (widestClusterAreaRadius <= clusterArea.getRadius()) {
                widestClusterArea = clusterArea;
                widestClusterAreaRadius = clusterArea.getRadius();
            }
        }
        
        return new ClustersAreas(clusterAreas, widestClusterArea);
    }
    
    /**
     * Computes the area that completely encloses the specified cluster and all the points that
     * belongs to it.
     *
     * @param cluster Cluster to compute the area of.
     * @param distanceMatrix Matrix that holds all the distances between all the pair of
     *     points.
     * @param clusterToAreas Map that maps the
     * @param leftSubClusterArea Object used in the computation of the area of the left
     *     sub-cluster. This object can be reused so to avoid the creation of a new object every
     *     time this method is called.
     * @param rightSubClusterArea Object used in the computation of the area of the right
     *     sub-cluster. This object can be reused so to avoid the creation of a new object every
     *     time this method is called.
     * @return The computed area.
     */
    @SuppressWarnings("FeatureEnvy")
    @NotNull
    private static ClusterArea computeClusterArea(
        @NotNull final Cluster cluster,
        @NotNull final DistanceMatrix distanceMatrix,
        @NotNull @UnmodifiableView
        final Map<@NotNull String, ? extends @NotNull ClusterArea> clusterToAreas,
        @NotNull final SubClusterArea leftSubClusterArea,
        @NotNull final SubClusterArea rightSubClusterArea
    ) {
        
        // Extract the points in the cluster
        final List<@NotNull ClusterPoint> clusterPoints = cluster.getClusterPoints();
        // Initialize the search
        ClusterPoint firstPoint = clusterPoints.get(0);
        ClusterPoint secondPoint = clusterPoints.get(1);
        double maxDistance = distanceMatrix.getDistance(firstPoint, secondPoint);
        
        int clusterPointIndex = 0;
        // Compute the maximum distance between the points in the cluster
        for (final ClusterPoint clusterPoint : clusterPoints) {
            // Iterator over the following points
            final ListIterator<@NotNull ClusterPoint> otherPointsIterator =
                clusterPoints.listIterator(clusterPointIndex + 1);
            
            // Iterate over the following points
            while (otherPointsIterator.hasNext()) {
                // Compute the distance between the points
                final ClusterPoint otherPoint = otherPointsIterator.next();
                final double distance = distanceMatrix.getDistance(clusterPoint, otherPoint);
                
                // Update the maximum distance and the farthest points, if necessary
                if (distance > maxDistance) {
                    maxDistance = distance;
                    firstPoint = clusterPoint;
                    secondPoint = otherPoint;
                }
            }
            clusterPointIndex++;
        }
        
        // Compute the center of the cluster
        final double clusterCenterX1 =
            (firstPoint.getX1Coordinate() + secondPoint.getX1Coordinate()) / 2;
        final double clusterCenterX2 =
            (firstPoint.getX2Coordinate() + secondPoint.getX2Coordinate()) / 2;
        
        // Compute the area of the left sub-cluster
        GraphPresenter.computeSubClusterArea(cluster.getFirstSubClusterIdentifier(),
            clusterCenterX1,
            clusterCenterX2,
            maxDistance,
            clusterToAreas,
            leftSubClusterArea
        );
        // Compute the area of the right sub-cluster
        GraphPresenter.computeSubClusterArea(cluster.getSecondSubClusterIdentifier(),
            clusterCenterX1,
            clusterCenterX2,
            maxDistance,
            clusterToAreas,
            rightSubClusterArea
        );
        
        // Create the area that encloses all the points in the cluster
        return new ClusterArea(cluster.getName(),
            clusterCenterX1,
            clusterCenterX2,
            StrictMath.max(leftSubClusterArea.getRadius(), rightSubClusterArea.getRadius()),
            StrictMath.max(leftSubClusterArea.getNestingLevel(),
                rightSubClusterArea.getNestingLevel()
            ) + 1
        );
    }
    
    /**
     * Computes the area that completely encloses the cluster centered in
     * {@code (centerX1, centerX2)} and the specified sub-cluster.
     *
     * @param subClusterIdentifier Identifier of the sub-cluster to enclose in the cluster to
     *     compute the area of.
     * @param clusterCenterX1 {@code x1} coordinate of the center of the cluster to compute the
     *     area of.
     * @param clusterCenterX2 {@code x2} coordinate of the center of the cluster to compute the
     *     area of.
     * @param maxDistance Maximum distance between the points in the cluster to compute the area
     *     of.
     * @param clusterToAreas Map that maps the identifier of the clusters to their areas.
     * @param result Object where the computed area will be placed. This is object is used so to
     *     avoid creating a new object every time this method is invoked.
     */
    private static void computeSubClusterArea(
        @NotNull final String subClusterIdentifier,
        final double clusterCenterX1,
        final double clusterCenterX2,
        final double maxDistance,
        @NotNull @UnmodifiableView
        final Map<@NotNull String, ? extends @NotNull ClusterArea> clusterToAreas,
        @NotNull final SubClusterArea result
    ) {
        
        // Check the type of the sub-cluster
        if (subClusterIdentifier.startsWith("C")) {
            // Retrieve the area of the sub-cluster
            final ClusterArea clusterArea = clusterToAreas.get(subClusterIdentifier);
            
            // The sub-cluster is a formed by at least two points, hence we need to compute the
            // radius of the area centered in (centerX1, centerX2) that completely encloses the
            // area of the sub-cluster
            result.setRadius(GraphPresenter.computeClusterRadius(clusterCenterX1,
                clusterCenterX2,
                clusterArea
            ));
            result.setNestingLevel(clusterArea.getSubClustersNestingLevel());
        } else {
            // The sub-cluster is formed only by one point, hence to enclose the point it is
            // enough a radius equal to half of the maximum distance between the points in the
            // cluster
            result.setRadius(maxDistance / 2);
            result.setNestingLevel(0);
        }
    }
    
    /**
     * Computes the radius of the cluster centered in the point {@code (centerX1, centerX2)} so that
     * it completely encloses the specified cluster area.
     *
     * @param centerX1 {@code x1} coordinate of the center of the cluster the radius compute
     *     of.
     * @param centerX2 {@code x2} coordinate of the center of the cluster the radius compute
     *     of.
     * @param clusterArea Area to completely enclose in the cluster the radius compute of.
     * @return The computed radius.
     */
    @SuppressWarnings("FeatureEnvy")
    private static double computeClusterRadius(
        final double centerX1, final double centerX2, @NotNull final ClusterArea clusterArea
    ) {
        
        // The radius of the area is just the distance between the center of the area and the
        // center of the cluster, plus the radius of the cluster
        final double x1Difference = centerX1 - clusterArea.getCenterX1();
        final double x2Difference = centerX2 - clusterArea.getCenterX2();
        
        return StrictMath.sqrt((x1Difference * x1Difference) + (x2Difference * x2Difference)) +
                   clusterArea.getRadius();
    }
    
    /**
     * Computes the vertical grid lines of the graph.
     *
     * @param coordinatesConverter Converter of coordinates from the {@code x1-x2} plane to the
     *     {@code x-y} plane.
     * @param frame Frame enclosing the graph.
     * @return The computed grid lines.
     */
    @SuppressWarnings("FeatureEnvy")
    @NotNull
    private static List<@NotNull GraphGridLine> computeVerticalGridLines(
        @NotNull final CoordinatesConverter coordinatesConverter, @NotNull final Rectangle2D frame
    ) {
        
        // Compute the exact length of a grid cell
        final double cellX1Length =
            coordinatesConverter.computeX1LengthFromXLength(GraphPresenter.GRID_CELL_LENGTH);
        
        // Compute the closest predefined length for the grid cells
        final double gridCellX1Length = GridCellLengthComputer.computeCellLength(cellX1Length);
        final double gridCellXLength =
            coordinatesConverter.computeXLengthFromX1Length(gridCellX1Length);
        
        // Compute the grid lines
        return GraphPresenter.computeGridLines(gridCellX1Length,
            coordinatesConverter.getMinX1(),
            coordinatesConverter.getMaxX1(),
            frame.getX(),
            frame.getX() + frame.getWidth() + 1,
            gridCellX1Length,
            gridCellXLength,
            coordinatesConverter::computeXCoordinateOf
        );
    }
    
    /**
     * Computes the horizontal grid lines of the graph.
     *
     * @param coordinatesConverter Converter of coordinates from the {@code x1-x2} plane to the
     *     {@code x-y} plane.
     * @param frame Frame enclosing the graph.
     * @return The computed grid lines.
     */
    @SuppressWarnings("FeatureEnvy")
    @NotNull
    private static List<@NotNull GraphGridLine> computeHorizontalGridLines(
        @NotNull final CoordinatesConverter coordinatesConverter, @NotNull final Rectangle2D frame
    ) {
        
        // Compute the exact length of a grid cell
        final double cellX2Length =
            coordinatesConverter.computeX2LengthFromYLength(GraphPresenter.GRID_CELL_LENGTH);
        
        // Compute the closest predefined length for the grid cells
        final double gridCellX2Length = GridCellLengthComputer.computeCellLength(cellX2Length);
        final double gridCellYLength =
            coordinatesConverter.computeYLengthFromX2Length(gridCellX2Length);
        
        // Compute the grid lines
        return GraphPresenter.computeGridLines(gridCellX2Length,
            coordinatesConverter.getMaxX2(),
            coordinatesConverter.getMinX2(),
            frame.getY(),
            frame.getY() + frame.getHeight() + 1,
            -gridCellX2Length,
            gridCellYLength,
            coordinatesConverter::computeYCoordinateOf
        );
    }
    
    /**
     * Computes the grid lines.
     *
     * @param gridCellLength Length, in the {@code x1-x2} plane, of one grid cell.
     * @param minValue If the grid lines are vertical, the {@code x1} value corresponding to the
     *     left-most grid line. If the grid lines are horizontal, instead, the {@code x2} coordinate
     *     of the top-most grid line.
     * @param maxValue If the grid lines are vertical, the {@code x1} value corresponding to the
     *     right-most grid line. If the grid lines are horizontal, instead, the {@code x2}
     *     coordinate of the bottom-most grid line.
     * @param minFramePixel If the grid lines are vertical, the {@code x} coordinate of the left
     *     edge of the frame. If the grid lines are horizontal, instead, the {@code y} coordinate of
     *     the top edge of the frame.
     * @param maxFramePixel If the grid lines are vertical, the {@code x} coordinate of the
     *     right edge of the frame. If the grid lines are horizontal, instead, the {@code y}
     *     coordinate of the bottom edge of the frame.
     * @param valueStep If the grid lines are vertical, the {@code x1} difference between one
     *     grid line and the one immediately on its right. If the grid lines are horizontal,
     *     instead, the {@code x2} difference between one grid line and the one immediately down
     *     it.
     * @param pixelStep If the grid lines are vertical, the {@code x} difference between one
     *     grid line and the one immediately on its right. If the grid lines are horizontal,
     *     instead, the {@code y} difference between one grid line and the one immediately down it.
     * @param pointToPixelConverter If the grid lines are vertical, the converter that converts
     *     {@code x1} coordinates into {@code x} coordinates. If the grid lines are horizontal,
     *     instead, the converter that converts {@code x2} coordinates into {@code y} coordinates.
     * @return the computed grid lines.
     */
    @SuppressWarnings("MethodWithTooManyParameters")
    @NotNull
    private static List<@NotNull GraphGridLine> computeGridLines(
        final double gridCellLength,
        final double minValue,
        final double maxValue,
        final double minFramePixel,
        final double maxFramePixel,
        final double valueStep,
        final double pixelStep,
        @NotNull final PointToPixelConverter pointToPixelConverter
    ) {
        
        // Compute the start and end coordinates on the x1-x2 plane approximating them to the
        // closest value snapped to the grid
        double firstGridLineValue = StrictMath.floor(minValue / gridCellLength) * gridCellLength;
        double lastGridLineValue = StrictMath.ceil(maxValue / gridCellLength) * gridCellLength;
        
        // Compute the start and end coordinates on the x-y plane
        double firstGridLinePixel = pointToPixelConverter.convert(firstGridLineValue);
        double lastGridLinePixel = pointToPixelConverter.convert(lastGridLineValue);
        
        // Add possible new grid lines before the first one
        double newPixelValue;
        do {
            newPixelValue = firstGridLinePixel - pixelStep;
            if (newPixelValue > minFramePixel) {
                firstGridLineValue -= valueStep;
                firstGridLinePixel -= pixelStep;
            }
        } while (newPixelValue > minFramePixel);
        
        // Add possible new grid lines after the last one
        do {
            newPixelValue = lastGridLinePixel + pixelStep;
            if (newPixelValue < maxFramePixel) {
                lastGridLineValue += valueStep;
                lastGridLinePixel += pixelStep;
            }
        } while (newPixelValue < maxFramePixel);
        
        // Compute the grid lines
        final List<@NotNull GraphGridLine> gridLines = new ArrayList<>();
        double gridLineValue = firstGridLineValue;
        double gridLinePixel = firstGridLinePixel;
        
        while (gridLinePixel <= (lastGridLinePixel + 1)) {
            // Exclude grid lines outside the frame
            if ((gridLinePixel > minFramePixel) && (gridLinePixel < maxFramePixel)) {
                // Create the label
                // noinspection ObjectAllocationInLoop
                gridLines.add(new GraphGridLine(GraphPresenter.GRID_LINES_VALUES_FORMAT.format(
                    gridLineValue), gridLinePixel));
            }
            // Move to the next grid line
            gridLineValue += valueStep;
            gridLinePixel += pixelStep;
        }
        
        return gridLines;
    }
}
