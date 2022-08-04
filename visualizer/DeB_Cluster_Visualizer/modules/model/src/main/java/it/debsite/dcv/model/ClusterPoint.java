package it.debsite.dcv.model;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.ToString;
import org.jetbrains.annotations.NotNull;

/**
 * Class representing a point that belongs to a cluster.
 *
 * @author DeB
 * @version 1.1 2022-08-04
 * @since 1.0
 */
@AllArgsConstructor
@Getter
@ToString
public class ClusterPoint {
    
    /**
     * Identifier uniquely identifying the point.
     */
    @NotNull
    private String identifier;
    
    /**
     * Name of the point.
     */
    @NotNull
    private String name;
    
    /**
     * {@code x1} coordinate of the point.
     */
    private double x1Coordinate;
    
    /**
     * {@code x2} coordinate of the point.
     */
    private double x2Coordinate;
}
