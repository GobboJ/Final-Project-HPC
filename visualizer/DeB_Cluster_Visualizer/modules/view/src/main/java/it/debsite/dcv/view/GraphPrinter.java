package it.debsite.dcv.view;

import it.debsite.dcv.view.draw.AxesPrinter;
import it.debsite.dcv.view.draw.GraphClusterPrinter;
import it.debsite.dcv.view.draw.GraphPointPrinter;
import javafx.geometry.Insets;
import javafx.scene.canvas.Canvas;

import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class GraphPrinter {
    
    private final Canvas canvas;
    
    private final AxesPrinter axesPrinter;
    
    private final GraphPointPrinter pointPrinter;
    
    private final GraphClusterPrinter clusterPrinter;
    
    public GraphPrinter(final Canvas canvas) {
        
        this.canvas = canvas;
        this.axesPrinter = new AxesPrinter(canvas);
        this.pointPrinter = new GraphPointPrinter(canvas);
        this.clusterPrinter = new GraphClusterPrinter(canvas);
    }
    
    public void draw(
        final double originX,
        final double originY,
        final List<GraphPoint> points,
        final List<GraphCluster> clusters
    ) {
        
        this.axesPrinter.draw(originX, originY);
        this.pointPrinter.draw(points);
        this.clusterPrinter.draw(clusters);
    }
    
}
