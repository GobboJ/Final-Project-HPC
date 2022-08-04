package it.debsite.dcv.view.dendrogram.tree;

import lombok.Getter;
import lombok.Setter;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/**
 * Class describing a node in the dendrogram tree.
 *
 * @author DeB
 * @version 1.2 2022-08-04
 * @since 1.0
 */
@Getter
@Setter
public class DendrogramTreeNode {
    
    /**
     * Parent node of this node. If this node is the root of the tree, then this field is
     * {@code null}.
     */
    @Nullable
    private DendrogramTreeNode parent;
    
    /**
     * Left child node of this node. If this node is a leaf, then this field is {@code null}.
     */
    @Nullable
    private DendrogramTreeNode left;
    
    /**
     * Right child node of this node. If this node is a leaf, then this field is {@code null}.
     */
    @Nullable
    private DendrogramTreeNode right;
    
    /**
     * Distance, in the {@code x1-x2} plane, between the points in the left subtree of the node and
     * the points in right one. If this node is a leaf, then this field is {@code 0}.
     */
    private double distance;
    
    /**
     * Text of the label associated to the node.
     */
    @NotNull
    private String labelText;
    
    /**
     * {@code x} coordinate of the node, relative to the top-left corner of the image.
     */
    private double nodeX;
    
    /**
     * {@code y} coordinate of the node, relative to the top-left corner of the image.
     */
    private double nodeY;
    
    /**
     * Creates a new representation of a node of the dendrogram tree.
     *
     * @param labelText Label associated to the node.
     * @param distance Distance, in the {@code x1-x2} plane, between the points in the left
     *     subtree of the node and the points in right one.
     */
    public DendrogramTreeNode(
        @NotNull final String labelText, final double distance
    ) {
        
        this.labelText = labelText;
        this.distance = distance;
    }
    
    /**
     * Returns whether this node is a leaf or not.
     *
     * @return {@code true} if this node is a leaf, {@code false} otherwise.
     */
    public boolean isLeaf() {
        
        // A leaf node has no left and no right subtree
        return (this.left == null) && (this.right == null);
    }
}
