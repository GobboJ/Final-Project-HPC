package it.debsite.dcv.presenter;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.presenter.utils.GraphInformation;
import it.debsite.dcv.view.GraphAxisLabel;
import it.debsite.dcv.view.GraphCluster;
import it.debsite.dcv.view.GraphPoint;
import it.debsite.dcv.view.GraphPrinter;

import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class GraphPresenter {
    
    private static final int MARGIN = 60;
    
    private static final int BOX_MARGIN = 10;
    
    private static final int BLOCK_SIZE = 60;
    
    private static final double[] SCALE = new double[]{1, 2, 4, 5, 10};
    
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
        
        final Rectangle2D.Double area = new Rectangle2D.Double(GraphPresenter.MARGIN,
            GraphPresenter.MARGIN,
            imageWidth - (GraphPresenter.MARGIN * 2),
            imageHeight - (GraphPresenter.MARGIN * 2)
        );
        
        final GraphInformation graphInformation =
            new GraphInformation(points, area, imageWidth, imageHeight);
        
        final List<GraphPoint> graphPoints = new ArrayList<>(points.size());
        for (final ClusterPoint point : points) {
            
            final GraphPoint graphPoint = new GraphPoint(point.getName(),
                graphInformation.computeXOf(point.getX1()),
                graphInformation.computeYOf(point.getX2())
            );
            graphPoints.add(graphPoint);
        }
        
        final List<GraphCluster> graphClusters = new ArrayList<>(clusters.size());
        for (final Cluster cluster : clusters) {
            graphClusters.add(GraphPresenter.createGraphCluster(cluster, graphInformation));
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
        this.graphPrinter.draw(area, boxArea, xLabels, yLabels, graphPoints, graphClusters);
    }
    
    private static GraphCluster createGraphCluster(
        final Cluster cluster, final GraphInformation graphInformation
    ) {
        
        final List<ClusterPoint> clusterPoints = cluster.getClusterPoints();
        if (clusterPoints.isEmpty()) {
            throw new IllegalArgumentException("Empty cluster");
        }
        final ClusterPoint firstPoint = clusterPoints.get(0);
        double minX1 = firstPoint.getX1();
        double maxX1 = firstPoint.getX1();
        double minX2 = firstPoint.getX2();
        double maxX2 = firstPoint.getX2();
        
        for (final ClusterPoint clusterPoint : clusterPoints) {
            minX1 = StrictMath.min(minX1, clusterPoint.getX1());
            maxX1 = StrictMath.max(maxX1, clusterPoint.getX1());
            minX2 = StrictMath.min(minX2, clusterPoint.getX2());
            maxX2 = StrictMath.max(maxX2, clusterPoint.getX2());
        }
        
        return new GraphCluster(cluster.getName(),
            graphInformation.computeXOf(minX1),
            graphInformation.computeYOf(maxX2),
            graphInformation.computeXOf(maxX1),
            graphInformation.computeYOf(minX2)
        );
    }
    
    private static List<GraphAxisLabel> computeXLabels(
        final GraphInformation graphInformation, final Rectangle2D.Double area
    ) {
        
        // Compute x1 and x2 correspondent to BLOCK_SIZE pixels
        final double x1Block = graphInformation.computeX1FromLength(GraphPresenter.BLOCK_SIZE);
        // Round to scale
        final double x1Scale = GraphPresenter.getScale(x1Block);
        final double xScale = graphInformation.computeLengthX(x1Scale);
        
        // Compute the start and end points
        final double x1Start = StrictMath.floor(graphInformation.getMinX1() / x1Scale) * x1Scale;
        final double x1End = StrictMath.ceil(graphInformation.getMaxX1() / x1Scale) * x1Scale;
        
        // Compute the x labels
        final double startX = graphInformation.computeXOf(x1Start);
        final double endX = graphInformation.computeXOf(x1End);
        
        final List<GraphAxisLabel> xLabels = new ArrayList<>();
        double x = startX;
        double x1 = x1Start;
        while (x < (endX + 1)) {
            if ((x > area.x) && (x < (area.x + area.width + 1))) {
                xLabels.add(new GraphAxisLabel("%.2f".formatted(x1), x));
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
        final double x2Scale = GraphPresenter.getScale(x2Block);
        final double yScale = graphInformation.computeLengthX(x2Scale);
        
        // Compute the start and end points
        final double x2Start = StrictMath.floor(graphInformation.getMinX2() / x2Scale) * x2Scale;
        final double x2End = StrictMath.ceil(graphInformation.getMaxX2() / x2Scale) * x2Scale;
        
        // Compute the x labels
        final double startY = graphInformation.computeYOf(x2End);
        final double endY = graphInformation.computeYOf(x2Start);
        
        final List<GraphAxisLabel> yLabels = new ArrayList<>();
        double y = startY;
        double x2 = x2End;
        
        while (y < (endY + 1)) {
            if ((y > area.y) && (y < (area.y + area.height + 1))) {
                yLabels.add(new GraphAxisLabel("%.2f".formatted(x2),
                    y
                ));
            }
            y += yScale;
            x2 -= x2Scale;
        }
        
        return yLabels;
    }
    
    private static double getScale(final double xBlock) {
        
        // xBlock = 2.3 --> 4
        // xBlock = 4.5 --> 5
        // xBlock = 7.9 --> 10
        
        double scale = 1;
        int scaleIndex = 0;
        double newScale = GraphPresenter.SCALE[0];
        double prevScale = newScale;
        
        if (xBlock > 1) {
            while (newScale < xBlock) {
                scaleIndex++;
                if (scaleIndex == GraphPresenter.SCALE.length) {
                    scale *= GraphPresenter.SCALE[GraphPresenter.SCALE.length - 1];
                    scaleIndex = 0;
                    newScale = GraphPresenter.SCALE[0];
                } else {
                    prevScale = newScale;
                    newScale = scale * GraphPresenter.SCALE[scaleIndex];
                }
            }
            
        } else {
            while (newScale > xBlock) {
                scaleIndex++;
                if (scaleIndex == GraphPresenter.SCALE.length) {
                    scale /= GraphPresenter.SCALE[GraphPresenter.SCALE.length - 1];
                    scaleIndex = 0;
                    newScale = GraphPresenter.SCALE[0];
                } else {
                    prevScale = newScale;
                    newScale = scale / GraphPresenter.SCALE[scaleIndex];
                }
            }
        }
        
        return prevScale;
    }
}
