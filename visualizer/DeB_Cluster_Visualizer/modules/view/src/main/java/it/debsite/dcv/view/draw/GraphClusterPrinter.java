package it.debsite.dcv.view.draw;

import it.debsite.dcv.view.GraphCluster;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;

import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class GraphClusterPrinter {
    
    private static final double MARGIN = 10;
    
    private final Canvas canvas;
    
    public GraphClusterPrinter(final Canvas canvas) {
        
        this.canvas = canvas;
    }
    
    public void draw(final List<GraphCluster> clusters) {
        
        for (GraphCluster cluster : clusters) {
            this.drawCluster(cluster);
        }
    }
    
    private void drawCluster(final GraphCluster cluster) {
        
        final GraphicsContext context = this.canvas.getGraphicsContext2D();
        context.setStroke(Color.RED);
        context.setLineWidth(3);
        context.strokeOval(
            cluster.getMinX() - GraphClusterPrinter.MARGIN,
            cluster.getMinY() - GraphClusterPrinter.MARGIN,
            (cluster.getMaxX() - cluster.getMinX()) + GraphClusterPrinter.MARGIN,
            (cluster.getMaxY() - cluster.getMinY()) + GraphClusterPrinter.MARGIN
        );
        context.setFill(Color.BLACK);
        context.fillText(
            cluster.getName(),
            cluster.getMinX() + ((cluster.getMaxX() - cluster.getMinX()) / 2),
            cluster.getMinY() - GraphClusterPrinter.MARGIN - 16
        );
    }
}
