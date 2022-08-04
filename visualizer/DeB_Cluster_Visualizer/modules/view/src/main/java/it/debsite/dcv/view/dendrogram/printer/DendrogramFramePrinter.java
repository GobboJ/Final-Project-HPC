package it.debsite.dcv.view.dendrogram.printer;

import it.debsite.dcv.view.GraphGridLine;
import it.debsite.dcv.view.utils.GridPrinter;
import org.jetbrains.annotations.NotNull;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.Stroke;
import java.awt.geom.Line2D;
import java.awt.geom.Path2D;
import java.awt.geom.Rectangle2D;

/**
 * Description.
 *
 * @author DeB
 * @version 1.1 2022-08-04
 * @since 1.0
 */
public class DendrogramFramePrinter {
    
    /**
     * Graphical context where the frame will be drawn.
     */
    @NotNull
    private final Graphics2D context;
    
    /**
     * Printer that prints the horizontal grid lines.
     */
    @NotNull
    private final GridPrinter gridPrinter;
    
    /**
     * Color of the two axes.
     */
    @NotNull
    private final Color axesColor;
    
    /**
     * Stroke to use to draw the two axes.
     */
    @NotNull
    private final Stroke axesStroke;
    
    /**
     * Width of the arrow to print on the top of the vertical axis.
     */
    private final double arrowWidth;
    
    /**
     * Height of the arrow to print on the top of the vertical axis.
     */
    private final double arrowHeight;
    
    /**
     * Space between the top of the frame and the top vertex of the arrow.
     */
    private final double arrowMarginTop;
    
    /**
     * Creates a new printer that prints the frame that encloses the dendrogram. In particular, it
     * prints the horizontal grid lines together with the two axes.
     *
     * @param context Graphical context where the frame will be drawn.
     * @param gridPrinter Printer that prints the horizontal grid lines.
     * @param axesColor Color of the two axes.
     * @param axesStroke Stroke to use to draw the two axes.
     * @param arrowWidth Width, in pixels, of the arrow printed on the top of the vertical
     *     axis.
     * @param arrowHeight Height, in pixels, of the arrow printed on the top of the vertical
     *     axis.
     * @param arrowMarginTop Space, in pixels, between the top of the frame and the top vertex
     *     of the arrow printed on the top of the vertical axis.
     */
    public DendrogramFramePrinter(
        @NotNull final Graphics2D context,
        @NotNull final GridPrinter gridPrinter,
        @NotNull final Color axesColor,
        @NotNull final Stroke axesStroke,
        final double arrowWidth,
        final double arrowHeight,
        final double arrowMarginTop
    ) {
        
        this.context = context;
        this.gridPrinter = gridPrinter;
        this.axesColor = axesColor;
        this.axesStroke = axesStroke;
        this.arrowWidth = arrowWidth;
        this.arrowHeight = arrowHeight;
        this.arrowMarginTop = arrowMarginTop;
    }
    
    /**
     * Draws the horizontal grid lines of the dendrogram.
     *
     * @param frame Rectangle enclosing the dendrogram.
     * @param horizontalGridLines Sequence of horizontal grid lines.
     * @param verticalAxiX {@code x} coordinate of the vertical axis, relative to the top-left
     *     corner of the image.
     */
    public void drawGridLines(
        @NotNull final Rectangle2D frame,
        @NotNull final Iterable<? extends @NotNull GraphGridLine> horizontalGridLines,
        final double verticalAxiX
    ) {
        
        // Draw the horizontal grid lines
        for (@NotNull final GraphGridLine label : horizontalGridLines) {
            this.gridPrinter.drawHorizontalGridLine(label,
                verticalAxiX,
                frame.getX() + frame.getWidth(),
                false
            );
        }
    }
    
    /**
     * Draws the frame of the dendrogram, which is composed of the two axes.
     *
     * @param frame Rectangle enclosing the dendrogram.
     * @param horizontalAxisY {@code y} coordinate of the horizontal axis, relative to the
     *     top-left corner of the image.
     * @param verticalAxisX {@code x} coordinate of the vertical axis, relative to the top-left
     *     corner of the image.
     */
    public void drawFrame(
        @NotNull final Rectangle2D frame, final double horizontalAxisY, final double verticalAxisX
    ) {
        
        // Set the aspect of the axes
        this.context.setColor(this.axesColor);
        this.context.setStroke(this.axesStroke);
        
        // Draw the horizontal axis
        this.context.draw(new Line2D.Double(verticalAxisX,
            horizontalAxisY,
            frame.getX() + frame.getWidth(),
            horizontalAxisY
        ));
        // Draw the vertical axis
        this.context.draw(new Line2D.Double(verticalAxisX,
            (frame.getY() + this.arrowHeight) - this.arrowMarginTop,
            verticalAxisX,
            horizontalAxisY
        ));
        
        // Create the path of the arrow
        final Path2D arrow = new Path2D.Double();
        // Start from the top vertex of the arrow
        arrow.moveTo(verticalAxisX, frame.getY() - this.arrowMarginTop);
        // Move to the bottom-right vertex of the arrow
        arrow.lineTo(verticalAxisX + (this.arrowWidth / 2),
            (frame.getY() + this.arrowHeight) - this.arrowMarginTop
        );
        // Move to the bottom-left vertex of the arrow
        arrow.lineTo(verticalAxisX - (this.arrowWidth / 2),
            (frame.getY() + this.arrowHeight) - this.arrowMarginTop
        );
        // Move again to the top vertex of the arrow
        arrow.closePath();
        
        // Draw the arrow
        this.context.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );
        this.context.fill(arrow);
    }
}
