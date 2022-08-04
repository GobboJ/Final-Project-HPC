package it.debsite.dcv.model;

import lombok.AccessLevel;
import lombok.Getter;
import lombok.Setter;
import lombok.ToString;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Unmodifiable;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Class representing a cluster of points. Any cluster of points is formed by:
 * <ul>
 *     <li>Two sub-clusters;</li>
 *     <li>One sub-cluster and a point;</li>
 *     <li>Two points.</li>
 * </ul>
 *
 * @author DeB
 * @version 1.1 2022-08-04
 * @since 1.0
 */
@Getter
@ToString
public class Cluster {
    
    /**
     * Identifier uniquely identifying the cluster.
     */
    @NotNull
    private final String identifier;
    
    /**
     * Name of the cluster.
     */
    @NotNull
    private final String name;
    
    /**
     * Identifier of first cluster or point enclosed in this cluster.
     */
    @NotNull
    private final String firstSubClusterIdentifier;
    
    /**
     * Identifier of second cluster or point enclosed in this cluster.
     */
    @SuppressWarnings("FieldNamingConvention")
    @NotNull
    private final String secondSubClusterIdentifier;
    
    /**
     * Distance between the first and the second sub-clusters of this cluster.
     */
    private final double distance;
    
    /**
     * List containing all the points belonging to this cluster.
     */
    @NotNull
    @Getter(AccessLevel.NONE)
    @Setter(AccessLevel.PACKAGE)
    private List<@NotNull ClusterPoint> clusterPoints;
    
    /**
     * Creates a new representation of a cluster of points.
     *
     * @param identifier Identifier that uniquely identifies this cluster of points.
     * @param name Name of the cluster.
     * @param firstSubClusterIdentifier Identifier of the first cluster/point contained into
     *     this cluster.
     * @param secondSubClusterIdentifier Identifier of the first cluster/point contained into
     *     this cluster.
     * @param distance Distance between the two clusters/points contained into this cluster.
     */
    public Cluster(
        @NotNull final String identifier,
        @NotNull final String name,
        @NotNull final String firstSubClusterIdentifier,
        @SuppressWarnings("MethodParameterNamingConvention") @NotNull
        final String secondSubClusterIdentifier,
        final double distance
    ) {
        
        this.identifier = identifier;
        this.name = name;
        this.firstSubClusterIdentifier = firstSubClusterIdentifier;
        this.secondSubClusterIdentifier = secondSubClusterIdentifier;
        this.distance = distance;
        this.clusterPoints = new ArrayList<>();
    }
    
    /**
     * Returns an unmodifiable view of the list containing all the points belonging to this cluster.
     * This includes the points directly enclosed in this cluster (if any) and all the points
     * enclosed in all the sub-clusters.
     *
     * @return An unmodifiable view of all the points belonging to this cluster.
     */
    @Unmodifiable
    public @NotNull List<@NotNull ClusterPoint> getClusterPoints() {
        
        return Collections.unmodifiableList(this.clusterPoints);
    }
}
