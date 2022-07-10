package it.debsite.dcv.view.draw;

import it.debsite.dcv.view.GraphCluster;

import java.awt.BasicStroke;
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
public class GraphClusterPrinter {
    
    private static final double MARGIN = 2;
    
    private static final float LINE_WIDTH = 3;
    
    private static final double BOX_MARGIN = 2;
    
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
        
        final double textX = (cluster.getMinX() + ((cluster.getMaxX() - cluster.getMinX()) / 2));
        final double textY = cluster.getMinY();
        final double textWidth = context.getFontMetrics().stringWidth(cluster.getName());
        final double textHeight = context.getFontMetrics().getHeight();
        
        context.setColor(new Color(0, 0, 0, 128));
        final Rectangle2D.Double box = new Rectangle2D.Double(textX - BOX_MARGIN,
            textY - textHeight,
            BOX_MARGIN + textWidth + BOX_MARGIN,
            BOX_MARGIN + textHeight + BOX_MARGIN
        );
        context.fill(box);
        
        context.setColor(new Color(255, 0, 0, 64));
        context.setStroke(new BasicStroke(GraphClusterPrinter.LINE_WIDTH));
        context.fill(new Rectangle2D.Double(cluster.getMinX() - GraphClusterPrinter.MARGIN,
            cluster.getMinY() - GraphClusterPrinter.MARGIN,
            (cluster.getMaxX() - cluster.getMinX()) + (GraphClusterPrinter.MARGIN * 2),
            (cluster.getMaxY() - cluster.getMinY()) + (GraphClusterPrinter.MARGIN * 2)
        ));
        context.setColor(Color.BLACK);
        context.drawString(cluster.getName(), (float) textX, (float) textY);
    }
}
