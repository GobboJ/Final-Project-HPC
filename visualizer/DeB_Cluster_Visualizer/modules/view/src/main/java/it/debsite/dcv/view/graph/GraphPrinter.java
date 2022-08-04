package it.debsite.dcv.view.graph;

import it.debsite.dcv.view.GraphGridLine;
import it.debsite.dcv.view.graph.printer.GraphClusterPrinter;
import it.debsite.dcv.view.graph.printer.GraphFramePrinter;
import it.debsite.dcv.view.graph.printer.GraphPointPrinter;
import it.debsite.dcv.view.utils.FontUtils;
import it.debsite.dcv.view.utils.FontUtils.FontStyle;
import it.debsite.dcv.view.utils.GridPrinter;
import org.jetbrains.annotations.NotNull;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;

/**
 * Class allowing to draw the graph containing points and clusters.
 *
 * @author DeB
 * @version 1.1 2022-08-02
 * @since 1.0
 */
public class GraphPrinter {
    
    /* *********************************************************************************************
     *
     * Grid.
     *
     **********************************************************************************************/
    
    /**
     * Color of each grid line that is not an axis.
     */
    @NotNull
    private static final Color GRID_LINE_COLOR = Color.LIGHT_GRAY;
    
    /**
     * Width of each of the grid lines.
     */
    private static final float GRID_LINE_WIDTH = 3.0F;
    
    /**
     * Color of the text label printed near each grid line.
     */
    @NotNull
    private static final Color GRID_LABEL_TEXT_COLOR = Color.BLACK;
    
    /**
     * Height of the text label printed near each grid line.
     */
    private static final float GRID_LABEL_TEXT_HEIGHT = 20.0F;
    
    /**
     * Style of the text label printed near each grid line.
     */
    @NotNull
    private static final FontStyle GRID_LABEL_TEXT_STYLE = FontStyle.PLAIN;
    
    /**
     * Space between each grid line and the corresponding text label.
     */
    private static final double GRID_LABEL_TEXT_MARGIN = 10;
    
    /**
     * Color of the grid lines that correspond to one of the axes.
     */
    @NotNull
    private static final Color AXES_COLOR = Color.GREEN;
    
    /**
     * Width of the grid lines that correspond to one of the axes.
     */
    private static final float AXES_LINE_WIDTH = GraphPrinter.GRID_LINE_WIDTH;
    
    /* *********************************************************************************************
     *
     * Frame.
     *
     **********************************************************************************************/
    
    /**
     * Color of the frame enclosing all the graph.
     */
    @NotNull
    private static final Color FRAME_COLOR = Color.BLACK;
    
    /**
     * Width of the frame line.
     */
    private static final float FRAME_LINE_WIDTH = 3.0F;
    
    /* *********************************************************************************************
     *
     * Points.
     *
     **********************************************************************************************/
    
    /**
     * Color of the circle representing a point.
     */
    @NotNull
    
    private static final Color POINT_COLOR = Color.BLUE;
    
    /**
     * Radius of the circle representing a point.
     */
    private static final double POINT_RADIUS = 4;
    
    /**
     * Background color of the box surrounding the text label associated to a point.
     */
    @NotNull
    private static final Color POINT_BOX_BACKGROUND_COLOR = new Color(0, 0, 0, 128);
    
    /**
     * Space between the point circle and the box surrounding the text label associated to the
     * point.
     */
    private static final double POINT_BOX_LABEL_MARGIN = 4;
    
    /**
     * Space between the box surrounding the text label associated to a point and the text label
     * itself.
     */
    private static final double POINT_BOX_LABEL_PADDING = 4;
    
    /**
     * Color of the text labels associated to the points.
     */
    @NotNull
    private static final Color POINT_LABEL_TEXT_COLOR = Color.BLACK;
    
    /**
     * Height of the text of the label associated to a point.
     */
    private static final float POINT_LABEL_FONT_HEIGHT = 14;
    
    /**
     * Style of the text of the label associated to a point.
     */
    @NotNull
    private static final FontStyle POINT_LABEL_FONT_STYLE = FontStyle.BOLD;
    
    /* *********************************************************************************************
     *
     * Clusters.
     *
     **********************************************************************************************/
    
    /**
     * Color of the ellipse enclosing all the points belonging to a cluster.
     */
    @NotNull
    private static final Color CLUSTER_LINE_COLOR = Color.RED;
    
    /**
     * Width of the line of the ellipse enclosing all the points belonging to a cluster.
     */
    private static final float CLUSTER_LINE_WIDTH = 3;
    
    /**
     * Background color of the box surrounding the label associated to a cluster.
     */
    @NotNull
    private static final Color CLUSTER_LABEL_BACKGROUND_COLOR = new Color(0, 0, 0, 128);
    
    /**
     * Space between the ellipse enclosing all the points belonging to a cluster and the box
     * surrounding the label associated to the cluster.
     */
    private static final double CLUSTER_BOX_LABEL_MARGIN = 10;
    
    /**
     * Space between the box surrounding the text label associated to a cluster and the text label
     * itself.
     */
    private static final double CLUSTER_BOX_LABEL_PADDING = 5;
    
    /**
     * Color of the text label associated to a cluster.
     */
    @NotNull
    private static final Color CLUSTER_LABEL_TEXT_COLOR = Color.BLACK;
    
    /**
     * Height of the font of the label associated to a cluster.
     */
    private static final float CLUSTER_LABEL_FONT_HEIGHT = 14;
    
    /**
     * Style of the font of the label associated to a cluster.
     */
    private static final FontStyle CLUSTER_LABEL_FONT_STYLE = FontStyle.BOLD;
    
    /**
     * Space between the ellipse representing a cluster and the ellipses representing the enclosed
     * clusters. This space allows two ellipses not to overlap.
     */
    public static final double CLUSTERS_MARGIN = 10;
    
    /* *********************************************************************************************
     *
     * Fields.
     *
     **********************************************************************************************/
    
    /**
     * Printer that prints the frame around the graph, together with the grid and the axes.
     */
    @NotNull
    private final GraphFramePrinter graphFramePrinter;
    
    /**
     * Printer that prints the points, together with the associated labels.
     */
    @NotNull
    private final GraphPointPrinter pointPrinter;
    
    /**
     * Printer that prints the clusters, together with the associated labels.
     */
    @NotNull
    private final GraphClusterPrinter clusterPrinter;
    
    /**
     * Create a new printer that prints the graph, with points and clusters, on the specified
     * image.
     *
     * @param image Image where the graph will be drawn.
     */
    @SuppressWarnings("FeatureEnvy")
    public GraphPrinter(@NotNull final BufferedImage image) {
        
        // Initialize the graphic context of the grid printer
        final Graphics2D gridPrinterContext = image.createGraphics();
        gridPrinterContext.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_ON
        );
        // Create the grid printer
        final GridPrinter gridPrinter = new GridPrinter(gridPrinterContext,
            GraphPrinter.GRID_LINE_COLOR,
            new BasicStroke(GraphPrinter.GRID_LINE_WIDTH),
            GraphPrinter.GRID_LABEL_TEXT_COLOR,
            FontUtils.deriveSystemFont(GraphPrinter.GRID_LABEL_TEXT_HEIGHT,
                GraphPrinter.GRID_LABEL_TEXT_STYLE
            ),
            GraphPrinter.GRID_LABEL_TEXT_MARGIN
        );
        
        // Initialize the graphic context of the frame printer
        final Graphics2D graphFramePrinterContext = image.createGraphics();
        graphFramePrinterContext.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );
        graphFramePrinterContext.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_DEFAULT
        );
        // Create the frame printer
        this.graphFramePrinter = new GraphFramePrinter(graphFramePrinterContext,
            gridPrinter,
            GraphPrinter.FRAME_COLOR,
            new BasicStroke(GraphPrinter.FRAME_LINE_WIDTH),
            GraphPrinter.AXES_COLOR,
            new BasicStroke(GraphPrinter.AXES_LINE_WIDTH)
        );
        
        // Initialize the graphic context of the points printer
        final Graphics2D graphPointPrinterContext = image.createGraphics();
        graphPointPrinterContext.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );
        graphPointPrinterContext.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_ON
        );
        // Create the points printer
        this.pointPrinter = new GraphPointPrinter(graphFramePrinterContext,
            GraphPrinter.POINT_COLOR,
            GraphPrinter.POINT_RADIUS,
            GraphPrinter.POINT_BOX_BACKGROUND_COLOR,
            GraphPrinter.POINT_BOX_LABEL_MARGIN,
            GraphPrinter.POINT_BOX_LABEL_PADDING,
            GraphPrinter.POINT_LABEL_TEXT_COLOR,
            FontUtils.deriveSystemFont(GraphPrinter.POINT_LABEL_FONT_HEIGHT,
                GraphPrinter.POINT_LABEL_FONT_STYLE
            )
        );
        
        // Initialize the graphic context of the cluster printer
        final Graphics2D clusterPrinterContext = image.createGraphics();
        clusterPrinterContext.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );
        clusterPrinterContext.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_ON
        );
        // Create the clusters printer
        this.clusterPrinter = new GraphClusterPrinter(clusterPrinterContext,
            GraphPrinter.CLUSTER_LINE_COLOR,
            new BasicStroke(GraphPrinter.CLUSTER_LINE_WIDTH),
            GraphPrinter.CLUSTER_LABEL_BACKGROUND_COLOR,
            GraphPrinter.CLUSTER_BOX_LABEL_MARGIN,
            GraphPrinter.CLUSTER_BOX_LABEL_PADDING,
            GraphPrinter.CLUSTER_LABEL_TEXT_COLOR,
            FontUtils.deriveSystemFont(GraphPrinter.CLUSTER_LABEL_FONT_HEIGHT,
                GraphPrinter.CLUSTER_LABEL_FONT_STYLE
            )
        );
    }
    
    /**
     * Draws the graph with the specified characteristics.
     *
     * @param frame Position and location of the frame than encloses the graph to print.
     * @param verticalGridLines Sequence of the vertical grid lines.
     * @param horizontalGridLines Sequence of the horizontal grid lines.
     * @param xAxisGridLine Grid line corresponding to the x-axis.
     * @param yAxisGridLine Grid line corresponding to the y-axis.
     * @param points Points to print in the graph.
     * @param clusters Clusters to print in the graph.
     */
    @SuppressWarnings("MethodWithTooManyParameters")
    public void drawGraph(
        @NotNull final Rectangle2D frame,
        @NotNull final Iterable<? extends @NotNull GraphGridLine> verticalGridLines,
        @NotNull final Iterable<? extends @NotNull GraphGridLine> horizontalGridLines,
        @NotNull final GraphGridLine xAxisGridLine,
        @NotNull final GraphGridLine yAxisGridLine,
        @NotNull final Iterable<? extends @NotNull GraphPoint> points,
        @NotNull final Iterable<? extends @NotNull GraphCluster> clusters
    ) {
        
        // Draw the grid, and the frame
        this.graphFramePrinter.drawFrame(frame,
            verticalGridLines,
            horizontalGridLines,
            xAxisGridLine,
            yAxisGridLine
        );
        // Draw the clusters
        this.clusterPrinter.drawClusters(clusters);
        // Draw the points
        this.pointPrinter.drawPoints(points);
    }
    
}
