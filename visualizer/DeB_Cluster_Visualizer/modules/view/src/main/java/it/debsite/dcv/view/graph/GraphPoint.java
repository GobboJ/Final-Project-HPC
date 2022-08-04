package it.debsite.dcv.view.graph;

import lombok.AllArgsConstructor;
import lombok.Getter;
import org.jetbrains.annotations.NotNull;

/**
 * Container holding the information about a point of the graph.
 *
 * @author DeB
 * @version 1.2 2022-08-02
 * @since 1.0
 */
@AllArgsConstructor
@Getter
public class GraphPoint {
    
    /**
     * Text of the label printed near to the point.
     */
    @NotNull
    private String labelText;
    
    /**
     * {@code x} coordinate of the center of the point, relative to the top-left corner of the
     * image.
     */
    private double centerX;
    
    /**
     * {@code y} coordinate of the center of the point, relative to the top-left corner of the
     * image.
     */
    private double centerY;
}
