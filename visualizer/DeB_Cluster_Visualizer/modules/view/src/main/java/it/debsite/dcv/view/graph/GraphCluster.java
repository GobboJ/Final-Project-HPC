package it.debsite.dcv.view.graph;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.ToString;
import org.jetbrains.annotations.NotNull;

/**
 * Container holding the information about a cluster to be printed on the graph.
 *
 * @author DeB
 * @version 1.2 2022-08-02
 * @since 1.0
 */
@AllArgsConstructor
@Getter
@ToString
public class GraphCluster {
    
    /**
     * Text of the label printed near to the cluster.
     */
    @NotNull
    private String labelText;
    
    /**
     * {@code x} coordinate, relative to the top-left corner of the image, of the center of the
     * circle that encloses all the points belonging to the cluster this object represents.
     */
    private double centerX;
    
    /**
     * {@code y} coordinate, relative to the top-left corner of the image, of the center of the
     * circle that encloses all the points belonging to the cluster this object represents.
     */
    private double centerY;
    
    /**
     * Radius along the {@code x} axis of the circle that encloses all the points belonging to the
     * cluster this object represents.<br> The value of this field is given in pixels.
     */
    private double radiusX;
    
    /**
     * Radius along the {@code y} axis of the circle that encloses all the points belonging to the
     * cluster this object represents.<br> The value of this field is given in pixels.
     */
    private double radiusY;
}
