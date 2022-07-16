package it.debsite.dcv.view;

import it.debsite.dcv.view.draw.AxesPrinter;
import it.debsite.dcv.view.draw.GraphClusterPrinter;
import it.debsite.dcv.view.draw.GraphPointPrinter;

import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class GraphPrinter {
    
    private final AxesPrinter axesPrinter;
    
    private final GraphPointPrinter pointPrinter;
    
    private final GraphClusterPrinter clusterPrinter;
    
    public GraphPrinter(final BufferedImage image) {
        
        this.axesPrinter = new AxesPrinter(image);
        this.pointPrinter = new GraphPointPrinter(image);
        this.clusterPrinter = new GraphClusterPrinter(image);
    }
    
    public void draw(
        final Rectangle2D.Double area,
        final Rectangle2D.Double boxArea,
        final List<GraphAxisLabel> xLabels,
        final List<GraphAxisLabel> yLabels,
        final GraphAxisLabel xAxis,
        final GraphAxisLabel yAxis,
        final List<GraphPoint> points,
        final List<GraphCluster> clusters
    ) {
        
        this.axesPrinter.draw(boxArea, xLabels, yLabels, xAxis, yAxis);
        this.clusterPrinter.draw(clusters);
        this.pointPrinter.draw(points);
    }
    
}
