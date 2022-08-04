package it.debsite.dcv.view.graph.printer;

import it.debsite.dcv.view.graph.GraphPoint;
import it.debsite.dcv.view.utils.FontUtils;
import org.jetbrains.annotations.NotNull;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Rectangle2D;

/**
 * Class responsible for printing the points on the graph.
 *
 * @author DeB
 * @version 1.1 2022-08-02
 * @since 1.0
 */
public class GraphPointPrinter {
    
    /**
     * Context where the points and the associated labels are printed.
     */
    @NotNull
    private final Graphics2D context;
    
    /**
     * Color of the printed points.
     */
    @NotNull
    private final Color pointColor;
    
    /**
     * Radius of the circles representing a point.
     */
    private final double pointRadius;
    
    /**
     * Background olor of the box surrounding the text label.
     */
    @NotNull
    private final Color labelBoxColor;
    
    /**
     * Space between the point circle and the box surrounding the text label.
     */
    private final double labelBoxMargin;
    
    /**
     * Space between the box surrounding the text label and the text label itself.
     */
    private final double labelBoxPadding;
    
    /**
     * Color of the text label.
     */
    @NotNull
    private final Color labelTextColor;
    
    /**
     * Font used for the text label.
     */
    @NotNull
    private final Font labelTextFont;
    
    /**
     * Ellipse object containing the information about one of the point circles. It is a field so to
     * avoid creating a new object every time a point is drawn.
     */
    @NotNull
    private final Ellipse2D pointCircle;
    
    /**
     * Rectangle object containing the information about one of the boxes surrounding the text
     * labels. It is a field so to avoid creating a new object every time a label is drawn.
     */
    @NotNull
    private final Rectangle2D labelBox;
    
    /**
     * Creates a new printer that prints the points and the associated labels to the graph.
     *
     * @param context Graphic context where the points and the labels will be drawn.
     * @param pointColor Color of the circles representing the points.
     * @param pointRadius Radius of the circles representing the points,
     * @param labelBoxBackgroundColor Background color of the box surrounding the label text of
     *     the point.
     * @param labelBoxMargin Space between the circle representing a point and the box
     *     surrounding the label text of the point.
     * @param labelBoxPadding Space between the box surrounding the label text and the label
     *     text itself.
     * @param labelTextColor Color of the text labels.
     * @param labelTextFont Font to use to draw the text labels.
     */
    public GraphPointPrinter(
        @NotNull final Graphics2D context,
        @NotNull final Color pointColor,
        final double pointRadius,
        @NotNull final Color labelBoxBackgroundColor,
        final double labelBoxMargin,
        final double labelBoxPadding,
        @NotNull final Color labelTextColor,
        @NotNull final Font labelTextFont
    ) {
        
        this.context = context;
        this.pointColor = pointColor;
        this.pointRadius = pointRadius;
        this.labelBoxColor = labelBoxBackgroundColor;
        this.labelBoxMargin = labelBoxMargin;
        this.labelBoxPadding = labelBoxPadding;
        this.labelTextColor = labelTextColor;
        this.labelTextFont = labelTextFont;
        this.pointCircle = new Ellipse2D.Double();
        this.labelBox = new Rectangle2D.Double();
    }
    
    /**
     * Draws the specified points to the graph.
     *
     * @param points Points to draw.
     */
    public void drawPoints(@NotNull final Iterable<? extends @NotNull GraphPoint> points) {
        
        // Draw each of the points
        for (@NotNull final GraphPoint point : points) {
            this.printPoint(point);
        }
    }
    
    /**
     * Draws a point on the graph.
     *
     * @param point The point to draw.
     */
    @SuppressWarnings({"NumericCastThatLosesPrecision", "FeatureEnvy"})
    private void printPoint(@NotNull final GraphPoint point) {
        
        // Extract the information of the point
        final double pointCenterX = point.getCenterX();
        final double pointCenterY = point.getCenterY();
        final String pointLabelText = point.getLabelText();
        
        // Set the aspect of the point
        this.context.setColor(this.pointColor);
        
        // Draw the point
        this.pointCircle.setFrame(pointCenterX - this.pointRadius,
            pointCenterY - this.pointRadius,
            this.pointRadius * 2,
            this.pointRadius * 2
        );
        this.context.fill(this.pointCircle);
        
        // Compute the bounds of the text
        this.context.setFont(this.labelTextFont);
        final Rectangle2D textBounds = FontUtils.getTextBounds(pointLabelText,
            this.labelTextFont,
            this.context.getFontMetrics()
        );
        
        // Compute the position of the box
        final double boxX = pointCenterX + this.pointRadius + this.labelBoxMargin;
        final double boxY = pointCenterY - (textBounds.getHeight() / 2) - this.labelBoxPadding;
        // Compute the position of the text
        final double textX = boxX + this.labelBoxPadding;
        final double textY = pointCenterY + (textBounds.getHeight() / 2);
        
        // Draw the box
        this.context.setColor(this.labelBoxColor);
        this.labelBox.setFrame(boxX,
            boxY,
            textBounds.getWidth() + (this.labelBoxPadding * 2),
            textBounds.getHeight() + (this.labelBoxPadding * 2)
        
        );
        this.context.fill(this.labelBox);
        
        // Draw the label
        this.context.setColor(this.labelTextColor);
        this.context.drawString(pointLabelText, (float) textX, (float) textY);
    }
}
