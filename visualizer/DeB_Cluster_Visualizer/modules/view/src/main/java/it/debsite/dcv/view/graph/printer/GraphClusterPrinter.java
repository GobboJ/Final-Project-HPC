package it.debsite.dcv.view.graph.printer;

import it.debsite.dcv.view.graph.GraphCluster;
import it.debsite.dcv.view.utils.FontUtils;
import org.jetbrains.annotations.NotNull;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Rectangle2D;

/**
 * Description.
 *
 * @author DeB
 * @version 1.1 2022-08-02
 * @since 1.0
 */
public class GraphClusterPrinter {
    
    /**
     * Context where the ellipses enclosing all the points belonging to a cluster, together with
     * their associated labels, will be printed.
     */
    @NotNull
    private final Graphics2D context;
    
    /**
     * Color of the ellipse enclosing all the points belonging to a cluster.
     */
    @NotNull
    private final Color clusterColor;
    
    /**
     * Stroke used when the ellipse representing a cluster is drawn.
     */
    @NotNull
    private final Stroke clusterStroke;
    
    /**
     * Background color of the box surrounding the text label.
     */
    @NotNull
    private final Color labelBoxBackgroundColor;
    
    /**
     * Space between the ellipse enclosing the points of the cluster and the text label.
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
     * Font to use to draw the text label.
     */
    @NotNull
    private final Font labelTextFont;
    
    /**
     * Ellipse object containing the information about one of the cluster circles. It is a field so
     * to avoid creating a new object every time a cluster is drawn.
     */
    @NotNull
    private final Ellipse2D clusterEllipse;
    
    /**
     * Rectangle object containing the information about one of the boxes surrounding the text
     * labels. It is a field so to avoid creating a new object every time a label is drawn.
     */
    @NotNull
    private final Rectangle2D labelBox;
    
    /**
     * Creates a new printer that prints the clusters and the associated labels.
     *
     * @param context Graphic context where the clusters will be drawn.
     * @param clusterColor Color of the ellipse enclosing all the points belonging to a
     *     cluster.
     * @param clusterStroke Stroke to use when drawing the ellipse enclosing all the points
     *     belonging to a cluster,
     * @param labelBoxBackgroundColor Background color of the box surrounding the text label
     *     associated to a cluster.
     * @param labelBoxMargin Space between the ellipse enclosing all the points belonging to the
     *     cluster and the box surrounding the label associated to the cluster.
     * @param labelBoxPadding Space between the box surrounding the label associated to the
     *     cluster and the label itself.
     * @param labelTextColor Color of the text labels associated to the clusters.
     * @param labelTextFont Font to use to draw the text labels associated to the clusters.
     */
    public GraphClusterPrinter(
        @NotNull final Graphics2D context,
        @NotNull final Color clusterColor,
        @NotNull final Stroke clusterStroke,
        @NotNull final Color labelBoxBackgroundColor,
        final double labelBoxMargin,
        final double labelBoxPadding,
        @NotNull final Color labelTextColor,
        @NotNull final Font labelTextFont
    ) {
        
        this.context = context;
        this.clusterColor = clusterColor;
        this.clusterStroke = clusterStroke;
        this.labelBoxBackgroundColor = labelBoxBackgroundColor;
        this.labelTextColor = labelTextColor;
        this.labelBoxMargin = labelBoxMargin;
        this.labelBoxPadding = labelBoxPadding;
        this.labelTextFont = labelTextFont;
        this.clusterEllipse = new Ellipse2D.Double();
        this.labelBox = new Rectangle2D.Double();
    }
    
    /**
     * Draws the specified clusters.
     *
     * @param clusters Clusters to draw.
     */
    public void drawClusters(@NotNull final Iterable<? extends @NotNull GraphCluster> clusters) {
        
        // Draw the ellipses enclosing the points belonging to the clusters
        for (@NotNull final GraphCluster cluster : clusters) {
            this.drawCluster(cluster);
        }
        // Draw all the labels associated to the clusters
        for (@NotNull final GraphCluster cluster : clusters) {
            this.drawLabel(cluster);
        }
    }
    
    /**
     * Draws the ellipse enclosing all the points belonging to the specified cluster.
     *
     * @param cluster Cluster to draw.
     */
    @SuppressWarnings("FeatureEnvy")
    private void drawCluster(@NotNull final GraphCluster cluster) {
        
        // Set the aspect of the cluster ellipse
        this.context.setColor(this.clusterColor);
        this.context.setStroke(this.clusterStroke);
        
        // Draw the ellipse enclosing all the points of the cluster
        this.clusterEllipse.setFrame(cluster.getCenterX() - cluster.getRadiusX(),
            cluster.getCenterY() - cluster.getRadiusY(),
            (cluster.getRadiusX()) * 2,
            (cluster.getRadiusY()) * 2
        );
        this.context.draw(this.clusterEllipse);
    }
    
    /**
     * Draws the text label associated to the specified cluster.
     *
     * @param cluster Cluster the label to draw is associated to.
     */
    @SuppressWarnings("FeatureEnvy")
    private void drawLabel(final GraphCluster cluster) {
        
        // Compute the bounds of the label text
        final Rectangle2D textBounds = FontUtils.getTextBounds(cluster.getLabelText(),
            this.labelTextFont,
            this.context.getFontMetrics()
        );
        // Compute the position of the text label
        final double textX = cluster.getCenterX() - (textBounds.getWidth() / 2);
        final double textY = cluster.getCenterY() - cluster.getRadiusY() - this.labelBoxMargin -
                                 this.labelBoxPadding;
        
        // Set the aspect of the box
        this.context.setColor(this.labelBoxBackgroundColor);
        
        // Draw the box
        this.labelBox.setFrame(textX - this.labelBoxPadding,
            textY - textBounds.getHeight() - this.labelBoxPadding,
            textBounds.getWidth() + (this.labelBoxPadding * 2),
            textBounds.getHeight() + (this.labelBoxPadding * 2)
        );
        this.context.fill(this.labelBox);
        
        // Set the aspect of the label
        this.context.setColor(this.labelTextColor);
        this.context.setFont(this.labelTextFont);
        
        // Draw the label
        // noinspection NumericCastThatLosesPrecision
        this.context.drawString(cluster.getLabelText(), (float) textX, (float) textY);
    }
}
