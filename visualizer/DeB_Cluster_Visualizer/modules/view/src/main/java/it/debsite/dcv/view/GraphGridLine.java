package it.debsite.dcv.view;

import lombok.AllArgsConstructor;
import lombok.Getter;
import org.jetbrains.annotations.NotNull;

/**
 * Container holding the information about a grid line, either horizontal or vertical, of the
 * graph.
 *
 * @author DeB
 * @version 1.2.1 2022-08-02
 * @since 1.0
 */
@AllArgsConstructor
@Getter
public class GraphGridLine {
    
    /**
     * Text of the label printed next to the grid line, e.g., the {@code x1} coordinate of all the
     * points lying on this grid line.
     */
    @NotNull
    private String labelText;
    
    /**
     * Coordinate of this grid line:
     * <ul>
     *     <li>If this container holds the information of a vertical grid line, then this field
     *     contains the {@code x} coordinate of the grid line, relative to the top-left corner of
     *     the image;</li>
     *     <li>If this container holds the information of a horizontal grid line, then this field
     *     contains the {@code y} coordinate of the grid line, relative to the top-left corner of
     *     the image.</li>
     * </ul>
     */
    private double pixelCoordinate;
}
