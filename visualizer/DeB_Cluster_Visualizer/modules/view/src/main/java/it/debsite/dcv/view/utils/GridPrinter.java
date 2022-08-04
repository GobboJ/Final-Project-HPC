package it.debsite.dcv.view.utils;

import it.debsite.dcv.view.GraphGridLine;
import org.jetbrains.annotations.NotNull;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.awt.geom.Line2D;
import java.awt.geom.Rectangle2D;

/**
 * Utility class allowing to print a grid together with the labels describing each line of the
 * grid.
 *
 * @author DeB
 * @version 1.2 2022-08-02
 * @since 1.0
 */
@SuppressWarnings("NumericCastThatLosesPrecision")
public final class GridPrinter {
    
    /**
     * Context where the grid will be drawn.
     */
    @NotNull
    private final Graphics2D context;
    
    /**
     * Color of the grid lines.
     */
    @NotNull
    private final Color gridLineColor;
    
    /**
     * Stroke of the grid lines.
     */
    @NotNull
    private final Stroke gridLineStroke;
    
    /**
     * Color of the text labels.
     */
    @NotNull
    private final Color labelTextColor;
    
    /**
     * Font of the text labels.
     */
    @NotNull
    private final Font labelTextFont;
    
    /**
     * Margin between each grid line and the associated label.
     */
    private final double labelTextMargin;
    
    /**
     * Line object containing the information about one grid line to draw. It is a field so to avoid
     * creating a new object for every grid line.
     */
    @NotNull
    private final Line2D line;
    
    /**
     * Creates a new printer that prints the grid on a graph.
     *
     * @param context Graphic context where the grid will be drawn.
     * @param gridLineColor Color of each line of the grid.
     * @param gridLineStroke Stroke to use when drawing the grid lines.
     * @param labelTextColor Color of the text labels printed next to each grid line.
     * @param labelTextFont Font of the text labels printed next to each grid line.
     * @param labelTextMargin Margin, in pixels, between each grid line and the associated
     *     label.
     */
    public GridPrinter(
        @NotNull final Graphics2D context,
        @NotNull final Color gridLineColor,
        @NotNull final Stroke gridLineStroke,
        @NotNull final Color labelTextColor,
        @NotNull final Font labelTextFont,
        final double labelTextMargin
    ) {
        
        this.context = context;
        this.gridLineColor = gridLineColor;
        this.gridLineStroke = gridLineStroke;
        this.labelTextColor = labelTextColor;
        this.labelTextFont = labelTextFont;
        this.labelTextMargin = labelTextMargin;
        this.line = new Line2D.Double();
    }
    
    /**
     * Draws a horizontal grid line together with a textual label on one or both sides of the line.
     *
     * @param gridLine Grid line to print. The {@code y} coordinate of the grid line together
     *     with the text label are specified in this object.
     * @param gridLineStartX {@code x} coordinate, relative to the top-left corner of the image,
     *     of the starting point of the horizontal grid line to draw.
     * @param gridLineEndX {@code x} coordinate, relative to the top-left corner of the image,
     *     of the ending point of the horizontal grid line to draw.
     * @param labelOnBothSides {@code true} if you want the labels to be printed on both sides
     *     of the grid line, {@code false} otherwise.
     */
    @SuppressWarnings("BooleanParameter")
    public void drawHorizontalGridLine(
        @NotNull final GraphGridLine gridLine,
        final double gridLineStartX,
        final double gridLineEndX,
        final boolean labelOnBothSides
    ) {
        
        // Extract the y coordinate of the grid line
        final double gridLineY = gridLine.getPixelCoordinate();
        
        // Set the grid line aspect
        this.context.setColor(this.gridLineColor);
        this.context.setStroke(this.gridLineStroke);
        
        // Draw the grid line
        this.line.setLine(gridLineStartX, gridLineY, gridLineEndX, gridLineY);
        this.context.draw(this.line);
        
        // Set the label aspect
        this.context.setColor(this.labelTextColor);
        this.context.setFont(this.labelTextFont);
        
        // Extract the text of the label
        final String labelText = gridLine.getLabelText();
        
        // Compute the bounds of the label so to center it
        final Rectangle2D textBounds =
            FontUtils.getTextBounds(labelText, this.labelTextFont, this.context.getFontMetrics());
        
        // Draw the label
        this.context.drawString(
            labelText,
            (float) (gridLineStartX - this.labelTextMargin - textBounds.getWidth()),
            (float) (this.line.getY1() + (textBounds.getHeight() / 2))
        );
        // Draw the label also on the right side of the line, if requested
        if (labelOnBothSides) {
            this.context.drawString(
                labelText,
                (float) (gridLineEndX + this.labelTextMargin),
                (float) (this.line.getY1() + (textBounds.getHeight() / 2))
            );
        }
    }
    
    /**
     * Draws a vertical grid line together with a textual label on both sides of the line.
     *
     * @param gridLine Grid line to print. The {@code x} coordinate of the grid line together
     *     with the text label are specified in this object.
     * @param gridLineStartY {@code y} coordinate, relative to the top-left corner of the image,
     *     of the starting point of the vertical grid line to draw.
     * @param gridLineEndY {@code y} coordinate, relative to the top-left corner of the image,
     *     of the ending point of the vertical grid line to draw.
     */
    public void drawVerticalGridLine(
        @NotNull final GraphGridLine gridLine,
        final double gridLineStartY,
        final double gridLineEndY
    ) {
        
        // Extract the x coordinate of the grid line
        final double gridLineX = gridLine.getPixelCoordinate();
        
        // Set the grid line aspect
        this.context.setColor(this.gridLineColor);
        this.context.setStroke(this.gridLineStroke);
        
        // Draw the grid line
        this.line.setLine(gridLineX, gridLineStartY, gridLineX, gridLineEndY);
        this.context.draw(this.line);
        
        // Set the label aspect
        this.context.setColor(Color.BLACK);
        this.context.setFont(this.labelTextFont);
        
        // Extra the text of the label
        final String labelText = gridLine.getLabelText();
        
        // Compute the bounds of the label so to center it
        final Rectangle2D textBounds =
            FontUtils.getTextBounds(labelText, this.labelTextFont, this.context.getFontMetrics());
        
        // Draw the labels
        // noinspection MagicNumber
        this.context.drawString(
            labelText,
            (float) (this.line.getX1() - (textBounds.getWidth() / 2.0)),
            (float) (gridLineStartY - this.labelTextMargin)
        );
        // noinspection MagicNumber
        this.context.drawString(
            labelText,
            (float) (this.line.getX1() - (textBounds.getWidth() / 2.0)),
            (float) (gridLineEndY + this.labelTextMargin + textBounds.getHeight())
        );
    }
}
