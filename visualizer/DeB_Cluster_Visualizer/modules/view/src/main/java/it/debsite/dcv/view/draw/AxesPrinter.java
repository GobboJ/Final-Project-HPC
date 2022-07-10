package it.debsite.dcv.view.draw;

import it.debsite.dcv.view.GraphAxisLabel;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.geom.Line2D;
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
public class AxesPrinter {
    
    private static final float LINE_WIDTH = 3.0F;
    
    private static final float FONT_HEIGHT = 12.0F;
    
    private static final double TEXT_MARGIN = 5.0F;
    
    private final BufferedImage image;
    
    public AxesPrinter(final BufferedImage image) {
        
        this.image = image;
    }
    
    public void draw(
        final Rectangle2D.Double rectangle,
        final List<GraphAxisLabel> xLabels,
        final List<GraphAxisLabel> yLabels
    ) {
        
        final Graphics2D context = this.image.createGraphics();
        context.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );
        context.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_DEFAULT
        );
        
        // Draw the grid
        for (final GraphAxisLabel label : xLabels) {
            this.drawVerticalLine(context,
                label.getPixelCoordinate(),
                rectangle.y,
                rectangle.y + rectangle.height,
                label.getName()
            );
        }
        for (final GraphAxisLabel label : yLabels) {
            HorizontalAxisPrinter.drawHorizontalLine(context,
                rectangle.x,
                rectangle.x + rectangle.width,
                label.getPixelCoordinate(),
                label.getName(),
                AxesPrinter.LINE_WIDTH,
                AxesPrinter.FONT_HEIGHT,
                AxesPrinter.TEXT_MARGIN,
                true
            );
        }
        
        // Draw the frame
        context.setColor(Color.BLACK);
        context.setStroke(new BasicStroke(AxesPrinter.LINE_WIDTH));
        context.draw(rectangle);
    }
    
    private void drawVerticalLine(
        final Graphics2D context,
        final double x,
        final double startY,
        final double endY,
        final String text
    ) {
        
        context.setColor(Color.LIGHT_GRAY);
        context.setStroke(new BasicStroke(AxesPrinter.LINE_WIDTH));
        context.draw(new Line2D.Double(x, startY, x, endY));
        context.setColor(Color.BLACK);
        final Font font = new Font("Arial", Font.PLAIN, 10).deriveFont(AxesPrinter.FONT_HEIGHT);
        context.setFont(font);
        final int textWidth = context.getFontMetrics().stringWidth(text);
        final int textHeight = context.getFontMetrics().getHeight();
        context.drawString(text,
            (float) (x - (textWidth / 2.0F)),
            (float) (startY - AxesPrinter.TEXT_MARGIN)
        );
        context.drawString(text,
            (float) (x - (textWidth / 2.0F)),
            (float) (endY + AxesPrinter.TEXT_MARGIN + textHeight)
        );
    }
}
