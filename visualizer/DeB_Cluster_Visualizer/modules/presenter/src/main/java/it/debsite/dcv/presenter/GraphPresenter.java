package it.debsite.dcv.presenter;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.presenter.utils.ClusterCircle;
import it.debsite.dcv.presenter.utils.DistanceMatrix;
import it.debsite.dcv.presenter.utils.GraphInformation;
import it.debsite.dcv.presenter.utils.ScaleComputer;
import it.debsite.dcv.view.GraphAxisLabel;
import it.debsite.dcv.view.GraphCluster;
import it.debsite.dcv.view.GraphPoint;
import it.debsite.dcv.view.GraphPrinter;

import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class GraphPresenter {
    
    private static final int MARGIN = 60;
    
    private static final int BOX_MARGIN = 20;
    
    private static final int BLOCK_SIZE = 60;
    
    private static final DecimalFormat DECIMAL_FORMAT;
    
    static {
        DECIMAL_FORMAT = new DecimalFormat();
        GraphPresenter.DECIMAL_FORMAT.setMaximumFractionDigits(2);
    }
    
    private final BufferedImage image;
    
    private final GraphPrinter graphPrinter;
    
    public GraphPresenter(
        final List<ClusterPoint> points,
        final List<Cluster> clusters,
        final BufferedImage image,
        final GraphPrinter graphPrinter
    ) {
        
        this.image = image;
        this.graphPrinter = graphPrinter;
        
        final double imageWidth = image.getWidth();
        final double imageHeight = image.getHeight();
        
        // Compute the distance matrix
        final DistanceMatrix matrix = new DistanceMatrix(points);
        
        // Order the clusters by the number of points within it
        final List<Cluster> orderedClusters = new ArrayList<>(clusters);
        orderedClusters.sort(Comparator.comparingInt(cluster -> cluster.getClusterPoints().size()));
        final Map<String, ClusterCircle> addedClusterCircles = new HashMap<>();
        
        // Compute the bounding box
        final List<ClusterCircle> clusterCircleList = new ArrayList<>(clusters.size());
        for (final Cluster cluster : orderedClusters) {
            // Compute the maximum distance between the points in the cluster
            final List<ClusterPoint> clusterPoints = cluster.getClusterPoints();
            ClusterPoint firstPoint = clusterPoints.get(0);
            ClusterPoint secondPoint = clusterPoints.get(1);
            double maxDistance = matrix.getDistance(firstPoint, secondPoint);
            
            for (int i = 0; i < clusterPoints.size(); i++) {
                for (int j = i + 1; j < clusterPoints.size(); j++) {
                    final double distance =
                        matrix.getDistance(clusterPoints.get(i), clusterPoints.get(j));
                    if (distance > maxDistance) {
                        maxDistance = distance;
                        firstPoint = clusterPoints.get(i);
                        secondPoint = clusterPoints.get(j);
                    }
                }
            }
            
            final double clusterCenterX1 = (firstPoint.getX1() + secondPoint.getX1()) / 2;
            final double clusterCenterX2 = (firstPoint.getX2() + secondPoint.getX2()) / 2;
            
            final double leftRadius;
            final int leftNestingLevel;
            final List<ClusterCircle> internalClusterCircles = new ArrayList<>(2);
            
            if (cluster.getLeftIdentifier().startsWith("C")) {
                final ClusterCircle leftClusterCircle =
                    addedClusterCircles.get(cluster.getLeftIdentifier());
                
                leftRadius = GraphPresenter.computeRadius(clusterCenterX1,
                    clusterCenterX2,
                    leftClusterCircle
                );
                leftNestingLevel = leftClusterCircle.getNestingLevel();
                internalClusterCircles.add(leftClusterCircle);
            } else {
                leftRadius = maxDistance / 2;
                leftNestingLevel = 0;
            }
            
            final double rightRadius;
            final int rightNestingLevel;
            
            if (cluster.getRightIdentifier().startsWith("C")) {
                final ClusterCircle rightClusterCircle =
                    addedClusterCircles.get(cluster.getRightIdentifier());
                rightRadius = GraphPresenter.computeRadius(clusterCenterX1,
                    clusterCenterX2,
                    rightClusterCircle
                );
                rightNestingLevel = rightClusterCircle.getNestingLevel();
                internalClusterCircles.add(rightClusterCircle);
            } else {
                rightRadius = maxDistance / 2;
                rightNestingLevel = 0;
            }
            
            // Create the circle that wraps all the points in the cluster
            final ClusterCircle clusterCircle = new ClusterCircle(cluster.getName(),
                clusterCenterX1,
                clusterCenterX2,
                StrictMath.max(leftRadius, rightRadius),
                StrictMath.max(leftNestingLevel, rightNestingLevel) + 1,
                internalClusterCircles
            );
            clusterCircleList.add(clusterCircle);
            addedClusterCircles.put(cluster.getIdentifier(), clusterCircle);
        }
        
        final Rectangle2D.Double area = new Rectangle2D.Double(GraphPresenter.MARGIN,
            GraphPresenter.MARGIN,
            imageWidth - (GraphPresenter.MARGIN * 2),
            imageHeight - (GraphPresenter.MARGIN * 2)
        );
        
        final GraphInformation graphInformation =
            new GraphInformation(clusterCircleList, area, imageWidth, imageHeight);
        
        final List<GraphPoint> graphPoints = new ArrayList<>(points.size());
        for (
            
            final ClusterPoint point : points) {
            
            final GraphPoint graphPoint = new GraphPoint(point.getName(),
                graphInformation.computeXOf(point.getX1()),
                graphInformation.computeYOf(point.getX2())
            );
            graphPoints.add(graphPoint);
        }
        
        final List<GraphCluster> graphClusters = new ArrayList<>(clusters.size());
        for (final ClusterCircle clusterCircle : clusterCircleList) {
            final int nestingLevel = clusterCircle.getNestingLevel();
            
            final GraphCluster graphCluster = new GraphCluster(clusterCircle.getName(),
                graphInformation.computeXOf(clusterCircle.getCenterX1()),
                graphInformation.computeYOf(clusterCircle.getCenterX2()),
                graphInformation.computeLengthX(clusterCircle.getRadius()) +
                    (nestingLevel * GraphPrinter.CLUSTER_MARGIN),
                graphInformation.computeLengthY(clusterCircle.getRadius()) +
                    (nestingLevel * GraphPrinter.CLUSTER_MARGIN)
            
            );
            graphClusters.add(graphCluster);
        }
        
        final Rectangle2D.Double boxArea =
            new Rectangle2D.Double(area.x - GraphPresenter.BOX_MARGIN,
                area.y - GraphPresenter.BOX_MARGIN,
                area.width + (GraphPresenter.BOX_MARGIN * 2),
                area.height + (GraphPresenter.BOX_MARGIN * 2)
            );
        
        final List<GraphAxisLabel> xLabels =
            GraphPresenter.computeXLabels(graphInformation, boxArea);
        
        final List<GraphAxisLabel> yLabels =
            GraphPresenter.computeYLabels(graphInformation, boxArea);
        
        final GraphAxisLabel xAxis = new GraphAxisLabel("x", graphInformation.computeYOf(0));
        
        final GraphAxisLabel yAxis = new GraphAxisLabel("y", graphInformation.computeXOf(0));
        
        this.graphPrinter.draw(area,
            boxArea,
            xLabels,
            yLabels,
            xAxis,
            yAxis,
            graphPoints,
            graphClusters
        );
    }
    
    private static List<GraphAxisLabel> computeXLabels(
        final GraphInformation graphInformation, final Rectangle2D.Double area
    ) {
        
        // Compute x1 and x2 correspondent to BLOCK_SIZE pixels
        final double x1Block = graphInformation.computeX1FromLength(GraphPresenter.BLOCK_SIZE);
        // Round to scale
        final double x1Scale = ScaleComputer.getScale(x1Block);
        final double xScale = graphInformation.computeLengthX(x1Scale);
        
        // Compute the start and end points
        final double x1Start = StrictMath.floor(graphInformation.getMinX1() / x1Scale) * x1Scale;
        final double x1End = StrictMath.ceil(graphInformation.getMaxX1() / x1Scale) * x1Scale;
        
        // Compute the x labels
        final double startX = graphInformation.computeXOf(x1Start);
        double endX = graphInformation.computeXOf(x1End);
        while ((endX + xScale) <= (area.getX() + area.getWidth())) {
            endX += xScale;
        }
        
        final List<GraphAxisLabel> xLabels = new ArrayList<>();
        double x = startX;
        double x1 = x1Start;
        while ((x - xScale) > area.getX()) {
            x -= xScale;
            x1 -= x1Scale;
        }
        
        while (x <= (endX + 1)) {
            if ((x > area.x) && (x < (area.x + area.width + 1))) {
                xLabels.add(new GraphAxisLabel(GraphPresenter.DECIMAL_FORMAT.format(x1), x));
            }
            x += xScale;
            x1 += x1Scale;
        }
        
        return xLabels;
    }
    
    private static List<GraphAxisLabel> computeYLabels(
        final GraphInformation graphInformation, final Rectangle2D.Double area
    ) {
        
        // Compute x1 and x2 correspondent to BLOCK_SIZE pixels
        final double x2Block = graphInformation.computeX2FromLength(GraphPresenter.BLOCK_SIZE);
        
        // Round to scale
        final double x2Scale = ScaleComputer.getScale(x2Block);
        final double yScale = graphInformation.computeLengthX(x2Scale);
        
        // Compute the start and end points
        final double x2Start = StrictMath.floor(graphInformation.getMinX2() / x2Scale) * x2Scale;
        final double x2End = StrictMath.ceil(graphInformation.getMaxX2() / x2Scale) * x2Scale;
        
        // Compute the x labels
        final double startY = graphInformation.computeYOf(x2End);
        double endY = graphInformation.computeYOf(x2Start);
        while ((endY + yScale) <= (area.getY() + area.getHeight())) {
            endY += yScale;
        }
        
        final List<GraphAxisLabel> yLabels = new ArrayList<>();
        double y = startY;
        double x2 = x2End;
        while ((y - yScale) > area.getY()) {
            y -= yScale;
            x2 += x2Scale;
        }
        while (y < (endY + 1)) {
            if ((y > area.y) && (y < (area.y + area.height + 1))) {
                yLabels.add(new GraphAxisLabel(GraphPresenter.DECIMAL_FORMAT.format(x2), y));
            }
            y += yScale;
            x2 -= x2Scale;
        }
        
        return yLabels;
    }
    
    private static double computeRadius(
        final double x1, final double x2, final ClusterCircle clusterCircle
    ) {
        
        final double x1Difference = x1 - clusterCircle.getCenterX1();
        final double x2Difference = x2 - clusterCircle.getCenterX2();
        
        return StrictMath.sqrt((x1Difference * x1Difference) + (x2Difference * x2Difference)) +
                   clusterCircle.getRadius();
    }
}
