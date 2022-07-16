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
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class GraphClusterPrinter {
    
    private static final float LINE_WIDTH = 3;
    
    private static final double BOX_MARGIN = 2;
    
    private static final float FONT_HEIGHT = 16;
    
    private static final double TEXT_MARGIN = 10;
    
    private final BufferedImage image;
    
    public GraphClusterPrinter(final BufferedImage image) {
        
        this.image = image;
    }
    
    public void draw(final List<GraphCluster> clusters) {
        
        for (final GraphCluster cluster : clusters) {
            this.drawCluster(cluster);
        }
        for (final GraphCluster cluster : clusters) {
            this.drawLabel(cluster);
        }
    }
    
    private void drawCluster(final GraphCluster cluster) {
        
        final Graphics2D context = this.image.createGraphics();
        context.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );
        
        final Ellipse2D.Double circle =
            new Ellipse2D.Double(cluster.getCenterX() - cluster.getRadiusX(),
                cluster.getCenterY() - cluster.getRadiusY(),
                (cluster.getRadiusX()) * 2,
                (cluster.getRadiusY()) * 2
            );
        
        context.setColor(Color.RED);
        context.setStroke(new BasicStroke(GraphClusterPrinter.LINE_WIDTH));
        context.draw(circle);
        context.setColor(Color.BLACK);
    }
    
    private void drawLabel(final GraphCluster cluster) {
        
        final Graphics2D context = this.image.createGraphics();
        context.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );
        context.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_ON
        );
        context.setFont(SystemFont.deriveSystemFont(GraphClusterPrinter.FONT_HEIGHT, Font.BOLD));
        
        final double textWidth = context.getFontMetrics().stringWidth(cluster.getName());
        final double textX = cluster.getCenterX() - (textWidth / 2);
        final double textY =
            cluster.getCenterY() - cluster.getRadiusY() - GraphClusterPrinter.TEXT_MARGIN;
        
        context.setColor(new Color(0, 0, 0, 128));
        final Rectangle2D.Double box =
            new Rectangle2D.Double(textX - GraphClusterPrinter.BOX_MARGIN,
                textY - GraphClusterPrinter.FONT_HEIGHT,
                GraphClusterPrinter.BOX_MARGIN + textWidth + GraphClusterPrinter.BOX_MARGIN,
                GraphClusterPrinter.BOX_MARGIN + GraphClusterPrinter.FONT_HEIGHT + GraphClusterPrinter.BOX_MARGIN
            );
        context.fill(box);
    
        context.setColor(Color.BLACK);
        context.drawString(cluster.getName(), (float) textX, (float) textY);
    }
}
