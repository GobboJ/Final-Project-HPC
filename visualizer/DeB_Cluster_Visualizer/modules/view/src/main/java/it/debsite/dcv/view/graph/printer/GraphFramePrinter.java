package it.debsite.dcv.view.graph.printer;

import it.debsite.dcv.view.GraphGridLine;
import it.debsite.dcv.view.utils.GridPrinter;
import org.jetbrains.annotations.NotNull;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.awt.geom.Line2D;
import java.awt.geom.Rectangle2D;

/**
 * Class allowing to draw the frame of the graph, together with the grid and the axes.
 *
 * @author DeB
 * @version 1.1 2022-08-02
 * @since 1.0
 */
public class GraphFramePrinter {
    
    /**
     * Context where the frame and the axes will be drawn.
     */
    @NotNull
    private final Graphics2D context;
    
    /**
     * Printer that prints the grid.
     */
    @NotNull
    private final GridPrinter gridPrinter;
    
    /**
     * Color of the frame surrounding the graph.
     */
    @NotNull
    private final Color frameColor;
    
    /**
     * Stroke to use to draw the frame that surrounds the graph.
     */
    @NotNull
    private final Stroke frameStroke;
    
    /**
     * Color of the axes.
     */
    @NotNull
    private final Color axesColor;
    
    /**
     * Stroke to use to draw the axes.
     */
    @NotNull
    private final Stroke axesStroke;
    
    /**
     * Line object containing the information about one of the two axes to draw. It is a field so to
     * avoid creating a new object every time an axis is drawn.
     */
    @NotNull
    private final Line2D axisLine;
    
    /**
     * Creates a new printer that prints the frame of the graph, together with the axes and the
     * grid.
     *
     * @param context Graphic context where the frame will be drawn.
     * @param gridPrinter Printer that prints the grid.
     * @param frameColor Color of the frame surrounding the graph.
     * @param frameStroke Stroke used when drawing the frame that surrounds the graph.
     * @param axesColor Color of the two axes.
     * @param axesStroke Stroke to use when drawing the two axes.
     */
    public GraphFramePrinter(
        @NotNull final Graphics2D context,
        @NotNull final GridPrinter gridPrinter,
        @NotNull final Color frameColor,
        @NotNull final Stroke frameStroke,
        @NotNull final Color axesColor,
        @NotNull final Stroke axesStroke
    ) {
        
        this.context = context;
        this.gridPrinter = gridPrinter;
        this.frameColor = frameColor;
        this.frameStroke = frameStroke;
        this.axesColor = axesColor;
        this.axesStroke = axesStroke;
        this.axisLine = new Line2D.Double();
    }
    
    /**
     * Draws the frame, the axes and the grid of the graph.
     *
     * @param frame Rectangle describing the position and size of the frame.
     * @param verticalGridLines Sequence of vertical grid lines.
     * @param horizontalGridLines Sequence of horizontal grid lines.
     * @param xAxisGridLine Grid line describing the x-axis.
     * @param yAxisGridLine Grid line describing the y-axis.
     */
    public void drawFrame(
        @NotNull final Rectangle2D frame,
        @NotNull final Iterable<? extends @NotNull GraphGridLine> verticalGridLines,
        @NotNull final Iterable<? extends @NotNull GraphGridLine> horizontalGridLines,
        @NotNull final GraphGridLine xAxisGridLine,
        @NotNull final GraphGridLine yAxisGridLine
    ) {
        
        // Draw the vertical grid lines
        for (@NotNull final GraphGridLine verticalGridLine : verticalGridLines) {
            this.gridPrinter.drawVerticalGridLine(verticalGridLine,
                frame.getY(),
                frame.getY() + frame.getHeight()
            );
        }
        // Draw the horizontal grid lines
        for (@NotNull final GraphGridLine horizontalGridLine : horizontalGridLines) {
            this.gridPrinter.drawHorizontalGridLine(horizontalGridLine,
                frame.getX(),
                frame.getX() + frame.getWidth(),
                true
            );
        }
        
        // Set the aspect of the axes
        this.context.setColor(this.axesColor);
        this.context.setStroke(this.axesStroke);
        
        // Draw the x-axis
        final double xAxisYCoordinate = xAxisGridLine.getPixelCoordinate();
        this.axisLine.setLine(frame.getX(),
            xAxisYCoordinate,
            frame.getX() + frame.getWidth(),
            xAxisYCoordinate
        );
        this.context.draw(this.axisLine);
        
        // Draw the y-axis
        final double yAxisXCoordinate = yAxisGridLine.getPixelCoordinate();
        this.axisLine.setLine(yAxisXCoordinate,
            frame.getY(),
            yAxisXCoordinate,
            frame.getY() + frame.getHeight()
        );
        this.context.draw(this.axisLine);
        
        // Draw the frame
        this.context.setColor(this.frameColor);
        this.context.setStroke(this.frameStroke);
        this.context.draw(frame);
    }
}
