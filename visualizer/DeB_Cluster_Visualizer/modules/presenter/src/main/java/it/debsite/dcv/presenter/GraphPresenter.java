package it.debsite.dcv.presenter;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.presenter.utils.GraphInformation;
import it.debsite.dcv.view.GraphCluster;
import it.debsite.dcv.view.GraphPoint;
import it.debsite.dcv.view.GraphPrinter;
import javafx.geometry.Insets;
import javafx.scene.canvas.Canvas;

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
    
    private final Canvas canvas;
    
    private final GraphPrinter graphPrinter;
    
    public GraphPresenter(
        final List<ClusterPoint> points,
        final List<Cluster> clusters,
        final Canvas canvas,
        final GraphPrinter graphPrinter
    ) {
        
        this.canvas = canvas;
        this.graphPrinter = graphPrinter;
        
        final GraphInformation graphInformation =
            new GraphInformation(points, new Insets(10), canvas.getWidth(), canvas.getHeight());
        
        final List<GraphPoint> graphPoints = new ArrayList<>(points.size());
        for (final ClusterPoint point : points) {
            
            final GraphPoint graphPoint = new GraphPoint(
                point.getName(),
                graphInformation.computeXOf(point.getX1()),
                graphInformation.computeYOf(point.getX2())
            );
            graphPoints.add(graphPoint);
        }
        
        final List<GraphCluster> graphClusters = new ArrayList<>(clusters.size());
        for (final Cluster cluster : clusters) {
            graphClusters.add(GraphPresenter.createGraphCluster(cluster, graphInformation));
        }
        
        this.graphPrinter.draw(
            graphInformation.computeXOf(0),
            graphInformation.computeYOf(0),
            graphPoints,
            graphClusters
        );
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
        
        return new GraphCluster(
            cluster.getName(),
            graphInformation.computeXOf(minX1),
            graphInformation.computeYOf(maxX2),
            graphInformation.computeXOf(maxX1),
            graphInformation.computeYOf(minX2)
            
        );
    }
}
