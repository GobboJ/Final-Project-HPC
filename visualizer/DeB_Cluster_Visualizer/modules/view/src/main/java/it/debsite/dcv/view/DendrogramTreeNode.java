package it.debsite.dcv.view;

import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.ToString;
import org.jetbrains.annotations.Nullable;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-10
 * @since version date
 */
@Getter
@Setter
public class DendrogramTreeNode {
    
    @Nullable
    private DendrogramTreeNode parent;
    
    @Nullable
    private DendrogramTreeNode left;
    
    @Nullable
    private DendrogramTreeNode right;
    
    private double distance;
    
    private String name;
    
    private double x;
    
    private double y;
    
    public DendrogramTreeNode(final String name, final double x, final double y, double distance) {
        
        this.name = name;
        this.x = x;
        this.y = y;
        this.distance = distance;
    }
    
    public boolean isLeaf() {
        
        return (this.left == null) && (this.right == null);
    }
}
