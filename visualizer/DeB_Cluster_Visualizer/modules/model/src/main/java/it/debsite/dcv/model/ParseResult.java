package it.debsite.dcv.model;

import lombok.AccessLevel;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.Setter;
import org.jetbrains.annotations.NotNull;

import java.util.List;

/**
 * Container containing the data parsed from the content a file containing the information about the
 * clusters and the points.
 *
 * @author DeB
 * @version 1.2 2022-08-04
 * @since 1.0
 */
@AllArgsConstructor(access = AccessLevel.PACKAGE)
@Getter
public class ParseResult {
    
    /**
     * Flag indicating whether the all points are bi-dimensional (i.e., with dimension 2) or not.
     */
    @Setter(AccessLevel.PACKAGE)
    private boolean biDimensional;
    
    /**
     * List of points.
     */
    @NotNull
    private final List<@NotNull ClusterPoint> points;
    
    /**
     * List of clusters.
     */
    @NotNull
    private final List<@NotNull Cluster> clusters;
}
