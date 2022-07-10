package it.debsite.dcv.view;

import it.debsite.dcv.view.draw.HorizontalAxisPrinter;

import javax.swing.JLabel;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.util.List;
import java.util.Objects;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-10
 * @since version date
 */
public class DendrogramPrinter {
    
    private static final double TEXT_MARGIN = 10;
    
    private static final float LINE_WIDTH = 3;
    
    private static final float FONT_HEIGHT = 22;
    
    private final BufferedImage image;
    
    public DendrogramPrinter(final BufferedImage image) {
        
        this.image = image;
    }
    
    public void draw(
        final DendrogramTreeNode root,
        final Rectangle2D.Double area,
        final double horizontalAxisY,
        final double verticalAxisX,
        final List<GraphAxisLabel> yAxisLabels
    ) {
        
        this.drawAxes(area, horizontalAxisY, verticalAxisX, yAxisLabels);
        this.drawCluster(root);
    }
    
    private void drawAxes(
        final Rectangle2D.Double area,
        final double horizontalAxesY,
        final double verticalAxesX,
        final List<GraphAxisLabel> yAxisLabels
    ) {
        
        final Graphics2D context = this.image.createGraphics();
        context.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_ON
        );
        
        context.setColor(Color.BLACK);
        context.setStroke(new BasicStroke(DendrogramPrinter.LINE_WIDTH));
        
        final JLabel jLabel = new JLabel();
        final String fontFamily = jLabel.getFont().getFamily();
        context.setFont(new Font(
            fontFamily,
            Font.PLAIN,
            10
        ).deriveFont(DendrogramPrinter.FONT_HEIGHT));
        
        // Other horizontal
        for (final GraphAxisLabel label : yAxisLabels) {
            HorizontalAxisPrinter.drawHorizontalLine(context,
                verticalAxesX,
                area.getX() + area.getWidth(),
                label.getPixelCoordinate(),
                label.getName(),
                DendrogramPrinter.LINE_WIDTH,
                DendrogramPrinter.FONT_HEIGHT,
                DendrogramPrinter.TEXT_MARGIN,
                false
            );
        }
        
        // Horizontal
        context.draw(new Line2D.Double(verticalAxesX,
            horizontalAxesY,
            area.getX() + area.getWidth(),
            horizontalAxesY
        ));
        // Vertical
        context.draw(new Line2D.Double(verticalAxesX, area.getY(), verticalAxesX, horizontalAxesY));
    }
    
    private Point2D.Double drawTreeNode(final DendrogramTreeNode node) {
        
        if (node.isLeaf()) {
            return this.drawPoint(node);
        }
        return this.drawCluster(node);
    }
    
    private Point2D.Double drawCluster(final DendrogramTreeNode clusterNode) {
        
        final Graphics2D context = this.image.createGraphics();
        context.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_ON
        );
        
        final Point2D.Double leftMiddleNode =
            this.drawTreeNode(Objects.requireNonNull(clusterNode.getLeft()));
        final Point2D.Double rightMiddleNode =
            this.drawTreeNode(Objects.requireNonNull(clusterNode.getRight()));
        
        final double leftBarHeight = leftMiddleNode.y - clusterNode.getY();
        final double rightBarHeight = rightMiddleNode.y - clusterNode.getY();
        
        context.setColor(new Color(12, 6, 76));
        context.setStroke(new BasicStroke(DendrogramPrinter.LINE_WIDTH));
        context.draw(new Line2D.Double(leftMiddleNode.x,
            leftMiddleNode.y,
            leftMiddleNode.x,
            leftMiddleNode.y - leftBarHeight
        ));
        context.draw(new Line2D.Double(rightMiddleNode.x,
            rightMiddleNode.y,
            rightMiddleNode.x,
            rightMiddleNode.y - rightBarHeight
        ));
        
        context.draw(new Line2D.Double(leftMiddleNode.x,
            leftMiddleNode.y - leftBarHeight,
            rightMiddleNode.x,
            rightMiddleNode.y - rightBarHeight
        ));
        
        final JLabel label = new JLabel();
        final String fontFamily = label.getFont().getFamily();
        context.setFont(new Font(
            fontFamily,
            Font.PLAIN,
            10
        ).deriveFont(DendrogramPrinter.FONT_HEIGHT));
        final double textWidth = context.getFontMetrics().stringWidth(clusterNode.getName());
        
        final double textX = rightMiddleNode.x - (textWidth / 2);
        final double textY = clusterNode.getY() - DendrogramPrinter.TEXT_MARGIN;
        
        context.drawString("%.2f".formatted(clusterNode.getDistance()),
            (float) textX,
            (float) textY
        );
        
        return new Point2D.Double((leftMiddleNode.x + rightMiddleNode.x) / 2, clusterNode.getY());
    }
    
    private Point2D.Double drawPoint(final DendrogramTreeNode pointNode) {
        
        final Graphics2D context = this.image.createGraphics();
        context.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_ON
        );
        
        final double textWidth = context.getFontMetrics().stringWidth(pointNode.getName());
        final double textHeight = context.getFontMetrics().getHeight();
        
        final double textX = pointNode.getX() - (textWidth / 2);
        final double textY = pointNode.getY() + DendrogramPrinter.TEXT_MARGIN + textHeight;
        
        final JLabel label = new JLabel();
        final String fontFamily = label.getFont().getFamily();
        context.setFont(new Font(
            fontFamily,
            Font.PLAIN,
            10
        ).deriveFont(DendrogramPrinter.FONT_HEIGHT));
        context.setColor(Color.BLACK);
        context.drawString(pointNode.getName(), (float) textX, (float) textY);
        
        return new Point2D.Double(pointNode.getX(), pointNode.getY());
    }
}
