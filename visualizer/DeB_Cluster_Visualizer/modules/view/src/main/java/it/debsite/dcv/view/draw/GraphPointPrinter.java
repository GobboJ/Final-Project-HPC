package it.debsite.dcv.view.draw;

import it.debsite.dcv.view.GraphPoint;
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
public class GraphPointPrinter {
    
    private static final double RADIUS = 1.5;
    
    private final Canvas canvas;
    
    public GraphPointPrinter(final Canvas canvas) {
        
        this.canvas = canvas;
    }
    
    public void draw(final List<? extends GraphPoint> points) {
        
        for (final GraphPoint point : points) {
            this.printPoint(point);
        }
    }
    
    private void printPoint(final GraphPoint point) {
        
        final GraphicsContext context = this.canvas.getGraphicsContext2D();
        context.setFill(Color.BLUE);
        context.fillOval(point.getX() - GraphPointPrinter.RADIUS,
            point.getY() - GraphPointPrinter.RADIUS, 3.0,
            3.0
        );
        context.setFill(Color.BLACK);
        context.fillText(
            point.getName(),
            point.getX() + GraphPointPrinter.RADIUS + 4,
            point.getY()
        );
    }
}
