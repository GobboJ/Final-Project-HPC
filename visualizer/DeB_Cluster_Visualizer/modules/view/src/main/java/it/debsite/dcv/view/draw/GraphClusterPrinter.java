package it.debsite.dcv.view.draw;

import it.debsite.dcv.view.GraphCluster;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
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
public class GraphClusterPrinter {
    
    private static final double CIRCLE_MARGIN = 10;
    
    private static final float LINE_WIDTH = 3;
    
    private static final double BOX_MARGIN = 2;
    
    private static final float FONT_HEIGHT = 16;
    
    private final BufferedImage image;
    
    public GraphClusterPrinter(final BufferedImage image) {
        
        this.image = image;
    }
    
    public void draw(final List<GraphCluster> clusters) {
        
        for (final GraphCluster cluster : clusters) {
            this.drawCluster(cluster);
        }
    }
    
    private void drawCluster(final GraphCluster cluster) {
        
        final Graphics2D context = this.image.createGraphics();
        context.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );
        context.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_ON
        );
        context.setFont(SystemFont.deriveSystemFont(GraphClusterPrinter.FONT_HEIGHT, Font.BOLD));
        
        final Ellipse2D.Double circle = new Ellipse2D.Double(
            cluster.getCenterX() - cluster.getRadiusX() - GraphClusterPrinter.CIRCLE_MARGIN,
            cluster.getCenterY() - cluster.getRadiusY() - GraphClusterPrinter.CIRCLE_MARGIN,
            (cluster.getRadiusX() + GraphClusterPrinter.CIRCLE_MARGIN) * 2,
            (cluster.getRadiusY() + GraphClusterPrinter.CIRCLE_MARGIN) * 2
        );
        
        final double textWidth = context.getFontMetrics().stringWidth(cluster.getName());
        
        final double textX = (circle.getX() + (circle.getWidth() / 2)) - (textWidth / 2);
        final double textY = circle.getY() - GraphClusterPrinter.CIRCLE_MARGIN;
        
        context.setColor(new Color(0, 0, 0, 128));
        final Rectangle2D.Double box =
            new Rectangle2D.Double(textX - GraphClusterPrinter.BOX_MARGIN,
                textY - FONT_HEIGHT,
                GraphClusterPrinter.BOX_MARGIN + textWidth + GraphClusterPrinter.BOX_MARGIN,
                GraphClusterPrinter.BOX_MARGIN + FONT_HEIGHT + GraphClusterPrinter.BOX_MARGIN
            );
        context.fill(box);
        
        context.setColor(Color.RED);
        context.setStroke(new BasicStroke(GraphClusterPrinter.LINE_WIDTH));
        context.draw(circle);
        context.setColor(Color.BLACK);
        
        context.drawString(cluster.getName(), (float) textX, (float) textY);
    }
}
