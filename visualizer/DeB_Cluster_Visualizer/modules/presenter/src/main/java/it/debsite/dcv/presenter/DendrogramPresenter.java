package it.debsite.dcv.presenter;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.presenter.utils.GridCellLengthComputer;
import it.debsite.dcv.view.GraphGridLine;
import it.debsite.dcv.view.dendrogram.DendrogramPrinter;
import it.debsite.dcv.view.dendrogram.tree.DendrogramTreeNode;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.jetbrains.annotations.UnmodifiableView;

import java.awt.Insets;
import java.awt.geom.Rectangle2D;
import java.awt.image.RenderedImage;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * Presenter that performs all the required operations needed to print a dendrogram onto an image.
 *
 * @author DeB
 * @version 1.1 2022-08-04
 * @since 1.0
 */
public class DendrogramPresenter {
    
    /**
     * Space between the edges of the image and the axes of the dendrogram.
     */
    @NotNull
    private static final Insets FRAME_MARGIN = new Insets(20, 40, 40, 20);
    
    /**
     * Space between the frame enclosing the dendrogram and the dendrogram itself.
     */
    private static final double DENDROGRAM_MARGIN = 20;
    
    /**
     * Preferred distance, in pixels, between one grid line and the following one. This value is
     * just a hint used when computing the real distance between consecutive grid lines. In any
     * cases, the real distance cannot be smaller than {@code GRID_LINES_DISTANCE} and cannot be
     * bigger than {@code GRID_LINES_DISTANCE * 2}.
     */
    private static final double GRID_LINES_DISTANCE = 60;
    
    /**
     * Format to use to properly format the values to print in the label associated to each grid
     * line.
     */
    @NotNull
    private static final DecimalFormat GRID_LINES_LABELS_VALUE_FORMAT;
    
    /*
     * Initialize the format.
     */
    static {
        GRID_LINES_LABELS_VALUE_FORMAT = new DecimalFormat();
        DendrogramPresenter.GRID_LINES_LABELS_VALUE_FORMAT.setMaximumFractionDigits(2);
    }
    
    /**
     * Width, in pixels, of the image where the dendrogram will be printed.
     */
    private final double imageWidth;
    
    /**
     * Height, in pixels, of the image where the dendrogram will be printed.
     */
    private final double imageHeight;
    
    /**
     * Printer that prints the dendrogram onto the image.
     */
    @NotNull
    private final DendrogramPrinter printer;
    
    /**
     * Creates the presenter that prints a dendrogram on the specified image.
     *
     * @param image Image where the dendrogram will be printed,
     * @param printer Printer that prints the dendrogram onto the image.
     */
    public DendrogramPresenter(
        @NotNull final RenderedImage image, @NotNull final DendrogramPrinter printer
    
    ) {
        
        this.imageWidth = image.getWidth();
        this.imageHeight = image.getHeight();
        this.printer = printer;
    }
    
    /**
     * Prints onto the image the dendrogram obtained by the specified clusters and points.
     *
     * @param points Points to represent on the dendrogram.
     * @param clusters Clusters to represent on the dendrogram.
     * @throws IllegalArgumentException If the clusters are not representable using a dendrogram
     *     tree, for instance, because some clusters are isolated, or because the identifier of a
     *     sub-cluster does not refer to an existing cluster.
     */
    public void printDendrogram(
        @NotNull @UnmodifiableView final Collection<? extends @NotNull ClusterPoint> points,
        @NotNull final Iterable<? extends @NotNull Cluster> clusters
    ) {
        
        // Compute the dendrogram tree
        final DendrogramTreeNode rootNode =
            DendrogramPresenter.buildDendrogramTree(points, clusters);
        
        // Compute the position and size of the frame enclosing the dendrogram and the axes
        final Rectangle2D frame = new Rectangle2D.Double(DendrogramPresenter.FRAME_MARGIN.left,
            DendrogramPresenter.FRAME_MARGIN.top,
            this.imageWidth -
                (DendrogramPresenter.FRAME_MARGIN.left + DendrogramPresenter.FRAME_MARGIN.right),
            this.imageHeight -
                (DendrogramPresenter.FRAME_MARGIN.top + DendrogramPresenter.FRAME_MARGIN.bottom)
        );
        
        // Compute the bounds of the frame enclosing just the dendrogram. This allows to add some
        // space between the dendrogram and the axes.
        
        // noinspection ConstantExpression
        final Rectangle2D dendrogramFrame =
            new Rectangle2D.Double(frame.getX() + DendrogramPresenter.DENDROGRAM_MARGIN,
                frame.getY() + DendrogramPresenter.DENDROGRAM_MARGIN,
                frame.getWidth() - (DendrogramPresenter.DENDROGRAM_MARGIN * 2),
                frame.getHeight() - (DendrogramPresenter.DENDROGRAM_MARGIN)
            );
        
        final double consecutiveLeavesMarginX = dendrogramFrame.getWidth() / (points.size() - 1);
        
        // Recursively set the coordinates of all the nodes in the dendrogram
        DendrogramPresenter.setNodeCoordinates(rootNode,
            dendrogramFrame.getX(),
            consecutiveLeavesMarginX,
            dendrogramFrame.getY(),
            dendrogramFrame.getHeight(),
            rootNode.getDistance()
        );
        
        // Compute the position of the axes
        final double horizontalAxisY = dendrogramFrame.getY() + dendrogramFrame.getHeight();
        final double verticalAxisX = frame.getX();
        // Compute the horizontal grid lines
        final List<@NotNull GraphGridLine> horizontalGridLines =
            DendrogramPresenter.computeHorizontalGridLines(rootNode.getDistance(),
                dendrogramFrame.getY(),
                dendrogramFrame.getHeight(),
                horizontalAxisY
            );
        
        // Draw the dendrogram
        this.printer.drawDendrogram(rootNode,
            frame,
            horizontalGridLines,
            horizontalAxisY,
            verticalAxisX
        );
    }
    
    /**
     * Builds the dendrogram tree representing the specified clusters and points.
     *
     * @param points Points to represent in the dendrogram tree.
     * @param clusters Clusters to represent in the dendrogram tree.
     * @return The root node of the dendrogram tree.
     * @throws IllegalArgumentException If the clusters are not representable using a dendrogram
     *     tree.
     */
    @SuppressWarnings("FeatureEnvy")
    @NotNull
    private static DendrogramTreeNode buildDendrogramTree(
        @NotNull final Iterable<? extends @NotNull ClusterPoint> points,
        @NotNull final Iterable<? extends @NotNull Cluster> clusters
    ) {
        
        // Create an unlinked node for every point
        final Map<@NotNull String, @NotNull DendrogramTreeNode> pointIdentifiersToNodes =
            new HashMap<>();
        for (final ClusterPoint point : points) {
            // noinspection ObjectAllocationInLoop
            final DendrogramTreeNode pointNode = new DendrogramTreeNode(point.getName(), 0);
            pointIdentifiersToNodes.put(point.getIdentifier(), pointNode);
        }
        
        // Create an unlinked node for every cluster
        // noinspection LocalVariableNamingConvention
        final Map<@NotNull String, @NotNull DendrogramTreeNode> clusterIdentifiersToNodes =
            new HashMap<>();
        for (final Cluster cluster : clusters) {
            // noinspection ObjectAllocationInLoop
            final DendrogramTreeNode clusterNode =
                new DendrogramTreeNode(cluster.getName(), cluster.getDistance());
            clusterIdentifiersToNodes.put(cluster.getIdentifier(), clusterNode);
        }
        
        // Link the nodes
        DendrogramPresenter.linkDendrogramTreeNodes(clusters,
            pointIdentifiersToNodes,
            clusterIdentifiersToNodes
        );
        
        // Find root, which is the only node with parent equal to null
        @Nullable DendrogramTreeNode rootNode = null;
        for (final DendrogramTreeNode treeNode : clusterIdentifiersToNodes.values()) {
            // Check if the parent of the node is null
            if (treeNode.getParent() == null) {
                // Check if another root node has already been found
                if (rootNode == null) {
                    rootNode = treeNode;
                } else {
                    final String messageFormat = "Found an isolated cluster '%s'";
                    final String message = messageFormat.formatted(rootNode.getLabelText());
                    throw new IllegalArgumentException(message);
                }
            }
        }
        
        // Require the dendrogram tree to have a root
        if (rootNode == null) {
            final String message = "Unable to find the cluster enclosing all the others";
            throw new IllegalArgumentException(message);
        }
        
        return rootNode;
    }
    
    /**
     * Links the nodes of the dendrogram tree.
     *
     * @param clusters Clusters represented in the dendrogram tree.
     * @param pointIdentifiersToNodes Map that maps the identifiers of the points into the nodes
     *     representing them.
     * @param clusterIdentifiersToNodes Map that maps the identifiers of the clusters into the
     *     nodes representing them.
     * @throws IllegalArgumentException If the clusters are not representable using a dendrogram
     *     tree.
     */
    @SuppressWarnings("FeatureEnvy")
    private static void linkDendrogramTreeNodes(
        @NotNull final Iterable<? extends @NotNull Cluster> clusters,
        @UnmodifiableView @NotNull
        final Map<@NotNull String, @NotNull DendrogramTreeNode> pointIdentifiersToNodes,
        @UnmodifiableView @NotNull
        final Map<@NotNull String, @NotNull DendrogramTreeNode> clusterIdentifiersToNodes
    ) {
        // Link the nodes
        for (final Cluster cluster : clusters) {
            // Extract the node corresponding to the cluster
            final DendrogramTreeNode clusterNode =
                clusterIdentifiersToNodes.get(cluster.getIdentifier());
            // Check if it has been found
            if (clusterNode == null) {
                final String messageFormat = "Unable to find the cluster with identifier '%s'";
                throw new IllegalArgumentException(messageFormat.formatted(cluster.getName()));
            }
            
            // Compute the map containing the node corresponding to the left child, based on its
            // type
            @UnmodifiableView
            final Map<@NotNull String, @NotNull DendrogramTreeNode> leftChildNodeMap =
                (cluster.getFirstSubClusterIdentifier().startsWith("P")
                     ? pointIdentifiersToNodes
                     : clusterIdentifiersToNodes);
            // Extract the left child node
            final DendrogramTreeNode leftChildNode =
                leftChildNodeMap.get(cluster.getFirstSubClusterIdentifier());
            
            // Compute the map containing the node corresponding to the right child, based on its
            // type
            @UnmodifiableView
            final Map<@NotNull String, @NotNull DendrogramTreeNode> rightChildNodeMap =
                (cluster.getSecondSubClusterIdentifier().startsWith("P")
                     ? pointIdentifiersToNodes
                     : clusterIdentifiersToNodes);
            // Extract the right child node
            final DendrogramTreeNode rightChildNode =
                rightChildNodeMap.get(cluster.getSecondSubClusterIdentifier());
            
            // The clusters must have both a left and a right child
            if ((leftChildNode == null) || (rightChildNode == null)) {
                final String messageFormat = "The cluster with identifier '%s' is malformed";
                throw new IllegalArgumentException(messageFormat.formatted(cluster.getName()));
            }
            
            // Link the nodes
            clusterNode.setLeft(leftChildNode);
            clusterNode.setRight(rightChildNode);
            leftChildNode.setParent(clusterNode);
            rightChildNode.setParent(clusterNode);
        }
    }
    
    /**
     * Sets the coordinates on the {@code x-y} plane of the specified node. In particular:
     * <ul>
     *     <li>If the specified node is a leaf, then this method sets its {@code x} and {@code y}
     *     coordinates;</li>
     *     <li>If the specified node is an internal node, then this method sets only its {@code y
     *     } coordinate, since the {@code x} one is computed when the dendrogram is printed.</li>
     * </ul>
     *
     * @param treeNode Node to set the coordinates of.
     * @param nextLeafX {@code x} coordinate of the next leaf to place on the horizontal axis.
     * @param consecutiveLeavesMarginX Space between one lead and the following one on the
     *     horizontal axis.
     * @param frameY {@code y} coordinate of the frame enclosing the dendrogram.
     * @param frameHeight Height of the frame enclosing the dendrogram.
     * @param rootNodeDistance Distance between the left and right subtree of the root node of
     *     the dendrogram.
     * @return The {@code x} coordinate of the next leaf to place on the horizontal axis.
     */
    @SuppressWarnings("FeatureEnvy")
    private static double setNodeCoordinates(
        @NotNull final DendrogramTreeNode treeNode,
        final double nextLeafX,
        final double consecutiveLeavesMarginX,
        final double frameY,
        final double frameHeight,
        final double rootNodeDistance
    ) {
        
        // Check the type of the node
        if (treeNode.isLeaf()) {
            // Put the leaf node next to the previous one
            treeNode.setNodeX(nextLeafX);
            // Put the leaf node on the horizontal axis
            treeNode.setNodeY(frameY + frameHeight);
            
            return nextLeafX + consecutiveLeavesMarginX;
        }
        // Compute the height, in pixels, of the node
        // frameHeight : rootNodeDistance = ???? : treeNode.getDistance()
        final double nodeHeight = (treeNode.getDistance() * frameHeight) / rootNodeDistance;
        // Set the y coordinate of the node. Note that the x coordinate is not used because
        // computed when the dendrogram is actually printed to the image.
        treeNode.setNodeY(frameY + (frameHeight - nodeHeight));
        
        // Set the position of the nodes in the left subtree
        final double nextLeafXPosition =
            DendrogramPresenter.setNodeCoordinates(Objects.requireNonNull(treeNode.getLeft()),
                nextLeafX,
                consecutiveLeavesMarginX,
                frameY,
                frameHeight,
                rootNodeDistance
            );
        // Set the position of the nodes in the right subtree
        // noinspection TailRecursion
        return DendrogramPresenter.setNodeCoordinates(Objects.requireNonNull(treeNode.getRight()),
            nextLeafXPosition,
            consecutiveLeavesMarginX,
            frameY,
            frameHeight,
            rootNodeDistance
        );
    }
    
    /**
     * Computes the horizontal grid lines.
     *
     * @param rootNodeDistance Distance between the left and right subtrees of the dendrogram
     *     root node.
     * @param frameY {@code y} coordinate of the frame that encloses the dendrogram.
     * @param frameHeight Height, in pixels, of the frame that encloses the dendrogram.
     * @param horizontalAxisY {@code y} coordinate of the horizontal axis.
     * @return The computed grid lines.
     */
    @NotNull
    private static List<@NotNull GraphGridLine> computeHorizontalGridLines(
        final double rootNodeDistance,
        final double frameY,
        final double frameHeight,
        final double horizontalAxisY
    ) {
        
        // Compute the distance between one grid line and the following one
        // frameHeight : rootNodeDistance = DendrogramPresenter.BLOCK_SIZE : ????
        final double gridCellDistance =
            (rootNodeDistance * DendrogramPresenter.GRID_LINES_DISTANCE) / frameHeight;
        // Compute the closest predefined distance between the grid lines
        final double gridLineDistance = GridCellLengthComputer.computeCellLength(gridCellDistance);
        // Compute the distance, in pixels, between one grid line and the following one
        // frameHeight : rootNodeDistance = ???? : gridLineDistance
        final double gridCellYDistance = (gridLineDistance * frameHeight) / rootNodeDistance;
        
        // Compute the grid lines
        final List<@NotNull GraphGridLine> horizontalGridLines = new ArrayList<>();
        
        double gridLineValue = gridLineDistance;
        double gridLineY = gridCellYDistance;
        
        while ((horizontalAxisY - gridLineY) >= frameY) {
            // Create the label
            // noinspection ObjectAllocationInLoop
            final GraphGridLine label = new GraphGridLine(
                DendrogramPresenter.GRID_LINES_LABELS_VALUE_FORMAT.format(gridLineValue),
                horizontalAxisY - gridLineY
            );
            horizontalGridLines.add(label);
            // Move to the next label
            gridLineValue += gridLineDistance;
            gridLineY += gridCellYDistance;
        }
        
        return horizontalGridLines;
    }
}
