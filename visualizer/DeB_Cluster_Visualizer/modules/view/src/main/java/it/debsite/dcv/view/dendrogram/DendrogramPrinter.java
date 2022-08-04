package it.debsite.dcv.view.dendrogram;

import it.debsite.dcv.view.GraphGridLine;
import it.debsite.dcv.view.dendrogram.printer.DendrogramFramePrinter;
import it.debsite.dcv.view.dendrogram.tree.DendrogramTreeNode;
import it.debsite.dcv.view.utils.FontUtils;
import it.debsite.dcv.view.utils.GridPrinter;
import org.jetbrains.annotations.NotNull;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.Stroke;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.text.DecimalFormat;
import java.util.Objects;

/**
 * Class allowing to print a dendrogram on an image.
 *
 * @author DeB
 * @version 1.1.1 2022-08-04
 * @since 1.0
 */
public class DendrogramPrinter {
    
    /* *********************************************************************************************
     *
     * Frame.
     *
     **********************************************************************************************/
    
    /**
     * Color of the horizontal grid lines.
     */
    @NotNull
    private static final Color GRID_LINES_COLOR = Color.LIGHT_GRAY;
    
    /**
     * Width of the horizontal grid lines.
     */
    private static final float GRID_LINES_WIDTH = 3;
    
    /**
     * Color of the labels associated to the grid lines.
     */
    @NotNull
    private static final Color GRID_LINES_LABEL_COLOR = Color.BLACK;
    
    /**
     * Height of font of the labels associated to the grid lines.
     */
    private static final float GRID_LINES_LABEL_TEXT_HEIGHT = 20;
    
    /**
     * Space between each grid line and the associated label.
     */
    private static final float GRID_LINES_LABEL_MARGIN = 10;
    
    /**
     * Color of the two axes.
     */
    @NotNull
    private static final Color AXES_COLOR = Color.BLACK;
    
    /**
     * Width of the line of the two axes.
     */
    private static final float AXES_LINE_WIDTH = 3;
    
    /**
     * Width of the arrow printed on the top of the vertical axis.
     */
    private static final double ARROW_WIDTH = 20;
    
    /**
     * Height of the arrow printed on the top of the vertical axis.
     */
    private static final double ARROW_HEIGHT = 20;
    
    /**
     * Space between the top of the frame and the top vertex of the arrow printed on the top of the
     * vertical axis.
     */
    private static final double ARROW_MARGIN = 10;
    
    /* *********************************************************************************************
     *
     * Dendrogram bars.
     *
     **********************************************************************************************/
    
    /**
     * Color of the bars of the dendrogram.
     */
    @NotNull
    private static final Color DENDROGRAM_BAR_COLOR = new Color(12, 6, 76);
    
    /**
     * Width of the dendrogram bars.
     */
    private static final float DENDROGRAM_BAR_LINE_WIDTH = 3;
    
    /**
     * Stroke to use when printing the dendrogram bars.
     */
    @NotNull
    private static final Stroke DENDROGRAM_BAR_STROKE =
        new BasicStroke(DendrogramPrinter.DENDROGRAM_BAR_LINE_WIDTH);
    
    /**
     * Space between the dendrogram bars and the associated distance label.
     */
    @NotNull
    private static final Color DISTANCE_LABEL_TEXT_COLOR = DendrogramPrinter.DENDROGRAM_BAR_COLOR;
    
    /**
     * Height of the font of the distance labels.
     */
    private static final float DISTANCE_LABEL_TEXT_HEIGHT = 22;
    
    /**
     * Font to use when printing the distance labels.
     */
    @NotNull
    private static final Font DISTANCE_LABELS_FONT =
        FontUtils.deriveSystemFont(DendrogramPrinter.DISTANCE_LABEL_TEXT_HEIGHT);
    
    /* *********************************************************************************************
     *
     * Dendrogram leaves.
     *
     **********************************************************************************************/
    
    /**
     * Color of the labels describing the leaves of the dendrogram.
     */
    @NotNull
    private static final Color LEAVES_LABEL_TEXT_COLOR = Color.BLACK;
    
    /**
     * Height of the font of the labels describing the leaves of the dendrogram.
     */
    private static final float LEAVES_LABEL_TEXT_HEIGHT = 22;
    
    /**
     * Font to use when printing the labels describing the leaves of the dendrogram.
     */
    @NotNull
    private static final Font LEAVES_LABELS_FONT =
        FontUtils.deriveSystemFont(DendrogramPrinter.LEAVES_LABEL_TEXT_HEIGHT);
    
    /**
     * Space between the labels describing the leaves of the dendrogram and the horizontal axis.
     */
    private static final double LEAF_LABEL_MARGIN = 10;
    
    /* *********************************************************************************************
     *
     * Other constants.
     *
     **********************************************************************************************/
    
    /**
     * Format to use when printing the distances.
     */
    @NotNull
    private static final DecimalFormat DISTANCE_FORMAT;
    
    /*
     * Initializes the format of the distance.
     */
    static {
        // Initialize the format
        DISTANCE_FORMAT = new DecimalFormat();
        DendrogramPrinter.DISTANCE_FORMAT.setMaximumFractionDigits(2);
    }
    
    /**
     * Graphic context where the dendrogram will be printed.
     */
    @NotNull
    private final Graphics2D context;
    
    /**
     * Printer that prints the frame around the dendrogram.
     */
    @NotNull
    private final DendrogramFramePrinter framePrinter;
    
    /**
     * Line object containing the information about one of bars of the dendrogram. It is a field so
     * to avoid creating a new object every time a new bar is drawn.
     */
    @NotNull
    private final Line2D dendrogramBar;
    
    /**
     * Creates a new printer that prints a dendrogram on the specified image.
     *
     * @param image Image where the dendrogram will be printed.
     */
    public DendrogramPrinter(@NotNull final BufferedImage image) {
        
        // Initialize the context of the grid printer
        final Graphics2D gridPrinterContext = image.createGraphics();
        gridPrinterContext.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_ON
        );
        // Create the grid printer
        final GridPrinter gridPrinter = new GridPrinter(gridPrinterContext,
            DendrogramPrinter.GRID_LINES_COLOR,
            new BasicStroke(DendrogramPrinter.GRID_LINES_WIDTH),
            DendrogramPrinter.GRID_LINES_LABEL_COLOR,
            FontUtils.deriveSystemFont(DendrogramPrinter.GRID_LINES_LABEL_TEXT_HEIGHT),
            DendrogramPrinter.GRID_LINES_LABEL_MARGIN
        );
        
        // Create the frame printer
        this.framePrinter = new DendrogramFramePrinter(image.createGraphics(),
            gridPrinter,
            DendrogramPrinter.AXES_COLOR,
            new BasicStroke(DendrogramPrinter.AXES_LINE_WIDTH),
            DendrogramPrinter.ARROW_WIDTH,
            DendrogramPrinter.ARROW_HEIGHT,
            DendrogramPrinter.ARROW_MARGIN
        );
        
        // Initialize the context where the dendrogram will be drawn
        this.context = image.createGraphics();
        this.context.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
            RenderingHints.VALUE_TEXT_ANTIALIAS_ON
        );
        
        this.dendrogramBar = new Line2D.Double();
    }
    
    /**
     * Draws the dendrogram.
     *
     * @param rootNode Root node of the dendrogram tree.
     * @param frame Position and size of the frame that encloses the dendrogram.
     * @param horizontalGridLines Sequence of horizontal grid lines.
     * @param horizontalAxisY {@code y} coordinate of the horizontal axis, relative to the
     *     top-left corner of the image.
     * @param verticalAxisX {@code x} coordinate of the horizontal axis, relative to the
     *     top-left corner of the image.
     */
    public void drawDendrogram(
        @NotNull final DendrogramTreeNode rootNode,
        @NotNull final Rectangle2D frame,
        @NotNull final Iterable<? extends @NotNull GraphGridLine> horizontalGridLines,
        final double horizontalAxisY,
        final double verticalAxisX
    ) {
        
        // Draw the grid lines
        this.framePrinter.drawGridLines(frame, horizontalGridLines, verticalAxisX);
        // Recursively draw the dendrogram
        this.drawInternalNode(rootNode);
        // Draw the frame
        this.framePrinter.drawFrame(frame, horizontalAxisY, verticalAxisX);
    }
    
    /**
     * Draws a tree node.
     *
     * @param node Tree node to draw.
     * @return The coordinates of the printed node.
     */
    @NotNull
    private Point2D drawTreeNode(@NotNull final DendrogramTreeNode node) {
        
        // Draw the node based on its type (leaf or internal)
        if (node.isLeaf()) {
            return this.drawLeaf(node);
        }
        return this.drawInternalNode(node);
    }
    
    /**
     * Draws an internal node.
     *
     * @param internalNode Internal node to draw.
     * @return The coordinates of the middle point of the edge connecting the left subtree and the
     *     right one of the specified node.
     */
    @SuppressWarnings("FeatureEnvy")
    @NotNull
    private Point2D drawInternalNode(@NotNull final DendrogramTreeNode internalNode) {
        
        // Recursively draw the left subtree
        final Point2D leftMiddleNode =
            this.drawTreeNode(Objects.requireNonNull(internalNode.getLeft()));
        // Recursively draw the right subtree
        final Point2D rightMiddleNode =
            this.drawTreeNode(Objects.requireNonNull(internalNode.getRight()));
        
        // Compute the height of the bar starting from the left subtree
        final double leftBarHeight = leftMiddleNode.getY() - internalNode.getNodeY();
        // Compute the height of the bar starting from the left subtree
        final double rightBarHeight = rightMiddleNode.getY() - internalNode.getNodeY();
        
        // Set the aspect of the dendrogram bars
        this.context.setColor(DendrogramPrinter.DENDROGRAM_BAR_COLOR);
        this.context.setStroke(DendrogramPrinter.DENDROGRAM_BAR_STROKE);
        
        // Draw the bar starting from the left subtree
        this.dendrogramBar.setLine(leftMiddleNode.getX(),
            leftMiddleNode.getY(),
            leftMiddleNode.getX(),
            leftMiddleNode.getY() - leftBarHeight
        );
        this.context.draw(this.dendrogramBar);
        
        // Draw the bar starting from the right subtree
        this.dendrogramBar.setLine(rightMiddleNode.getX(),
            rightMiddleNode.getY(),
            rightMiddleNode.getX(),
            rightMiddleNode.getY() - rightBarHeight
        );
        this.context.draw(this.dendrogramBar);
        
        // Draw the line connecting the two bars
        this.dendrogramBar.setLine(leftMiddleNode.getX(),
            leftMiddleNode.getY() - leftBarHeight,
            rightMiddleNode.getX(),
            rightMiddleNode.getY() - rightBarHeight
        );
        this.context.draw(this.dendrogramBar);
        
        // Set the aspect of the distance label
        this.context.setColor(DendrogramPrinter.DISTANCE_LABEL_TEXT_COLOR);
        this.context.setFont(DendrogramPrinter.DISTANCE_LABELS_FONT);
        
        // Round the distance
        final String distanceString =
            DendrogramPrinter.DISTANCE_FORMAT.format(internalNode.getDistance());
        
        // Compute the bounds of the distance label
        final Rectangle2D textBounds = FontUtils.getTextBounds(distanceString,
            DendrogramPrinter.DISTANCE_LABELS_FONT,
            this.context.getFontMetrics()
        );
        
        // Compute the position of the text
        final double textX = rightMiddleNode.getX() - (textBounds.getWidth() / 2);
        final double textY = internalNode.getNodeY() - DendrogramPrinter.LEAF_LABEL_MARGIN;
        
        // Print the distance label
        // noinspection NumericCastThatLosesPrecision
        this.context.drawString(distanceString, (float) textX, (float) textY);
        
        // Return the coordinate of the point in between the two bars
        return new Point2D.Double((leftMiddleNode.getX() + rightMiddleNode.getX()) / 2,
            internalNode.getNodeY()
        );
    }
    
    /**
     * Draws a leaf node.
     *
     * @param leafNode Leaf node to print.
     * @return The coordinates of the printed leaf node, relative to the top-left corner of the
     *     image.
     */
    @SuppressWarnings("FeatureEnvy")
    @NotNull
    private Point2D drawLeaf(@NotNull final DendrogramTreeNode leafNode) {
        
        // Set the aspect of the label
        this.context.setFont(DendrogramPrinter.LEAVES_LABELS_FONT);
        this.context.setColor(DendrogramPrinter.LEAVES_LABEL_TEXT_COLOR);
        
        // Extract the information of the node
        final double nodeX = leafNode.getNodeX();
        final double nodeY = leafNode.getNodeY();
        final String nodeLabelText = leafNode.getLabelText();
        
        // Compute the bounds of the label
        final Rectangle2D textBounds = FontUtils.getTextBounds(nodeLabelText,
            DendrogramPrinter.LEAVES_LABELS_FONT,
            this.context.getFontMetrics()
        );
        // Compute the position of the label
        final double textX = nodeX - (textBounds.getWidth() / 2);
        final double textY = nodeY + DendrogramPrinter.LEAF_LABEL_MARGIN + textBounds.getHeight();
        
        // Draw the label
        // noinspection NumericCastThatLosesPrecision
        this.context.drawString(nodeLabelText, (float) textX, (float) textY);
        
        // Return the coordinates of the printed node
        return new Point2D.Double(nodeX, nodeY);
    }
}
