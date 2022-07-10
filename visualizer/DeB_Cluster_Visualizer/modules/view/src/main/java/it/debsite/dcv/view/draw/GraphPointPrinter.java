package it.debsite.dcv.view.draw;

import it.debsite.dcv.view.GraphPoint;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.geom.Ellipse2D;
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
public class GraphPointPrinter {
    
    private static final double RADIUS = 4;
    
    private static final double MARGIN = 4;
    
    private static final double BOX_MARGIN = 4;
    
    private final BufferedImage image;
    
    public GraphPointPrinter(final BufferedImage image) {
        
        this.image = image;
    }
    
    public void draw(final List<? extends GraphPoint> points) {
        
        for (final GraphPoint point : points) {
            this.printPoint(point);
        }
    }
    
    private void printPoint(final GraphPoint point) {
        
        final Graphics2D context = this.image.createGraphics();
        context.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );
        context.setColor(Color.BLUE);
        
        context.fill(new Ellipse2D.Double(point.getX() - GraphPointPrinter.RADIUS,
            point.getY() - GraphPointPrinter.RADIUS,
            GraphPointPrinter.RADIUS * 2,
            GraphPointPrinter.RADIUS * 2
        ));
        final double textX = point.getX() + GraphPointPrinter.RADIUS + GraphPointPrinter.MARGIN;
        final double textY = point.getY();
        final double textWidth = context.getFontMetrics().stringWidth(point.getName());
        final double textHeight = context.getFontMetrics().getHeight();
        
        context.setColor(new Color(0, 0, 0, 128));
        final Rectangle2D.Double box = new Rectangle2D.Double(textX - GraphPointPrinter.BOX_MARGIN,
            textY - textHeight,
            GraphPointPrinter.BOX_MARGIN + textWidth + GraphPointPrinter.BOX_MARGIN,
            GraphPointPrinter.BOX_MARGIN + textHeight + GraphPointPrinter.BOX_MARGIN
        );
        context.fill(box);
        
        context.setColor(Color.BLACK);
        context.drawString(point.getName(), (float) textX, (float) textY);
    }
}
