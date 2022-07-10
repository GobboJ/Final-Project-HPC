package it.debsite.dcv.view.draw;

import it.debsite.dcv.view.GraphPoint;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import lombok.AllArgsConstructor;
import lombok.Getter;

import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class AxesPrinter {
    
    private static final double LINE_WIDTH = 3;
    
    private final Canvas canvas;
    
    public AxesPrinter(final Canvas canvas) {
        
        this.canvas = canvas;
    }
    
    public void draw(
        final double originX, final double originY
    ) {
        
        final GraphicsContext context = this.canvas.getGraphicsContext2D();
        context.setStroke(Color.BLACK);
        context.setLineWidth(AxesPrinter.LINE_WIDTH);
        // Draw the x-axis
        context.strokeLine(0, originY, this.canvas.getWidth(), originY);
        // Draw the y-axis
        context.strokeLine(originX, 0, originX, this.canvas.getHeight());
    }
}
