package it.debsite.dcv.presenter.utils;

import lombok.AllArgsConstructor;
import lombok.Getter;
import org.jetbrains.annotations.NotNull;

/**
 * Container describing the area, in the {@code x1-x2} plane, that encloses all the points belonging
 * to a cluster.
 *
 * @author DeB
 * @version 1.1 2022-08-03
 * @since 1.0
 */
@AllArgsConstructor
@Getter
public class ClusterArea {
    
    /**
     * Name of the cluster this object represents.
     */
    @NotNull
    private final String name;
    
    /**
     * {@code x1} coordinate of the center of the area that encloses all the points belonging to the
     * cluster this object represents.
     */
    private final double centerX1;
    
    /**
     * {@code x2} coordinate of the center of the area that encloses all the points belonging to the
     * cluster this object represents.
     */
    private final double centerX2;
    
    /**
     * Radius of the area, in the {@code x1-x2} plane, that encloses all the points belonging to the
     * cluster this object represents.
     */
    private final double radius;
    
    /**
     * Depth of the deepest sub-cluster (i.e., the cluster with more sub-clusters) contained in the
     * cluster this object represents.
     */
    private final int subClustersNestingLevel;
}
