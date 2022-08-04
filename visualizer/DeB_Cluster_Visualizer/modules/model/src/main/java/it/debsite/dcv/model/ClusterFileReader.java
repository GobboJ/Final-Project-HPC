package it.debsite.dcv.model;

import it.debsite.dcv.model.parser.RawDataFileParser;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Unmodifiable;

import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Reader that reads the content of a file containing information about the points and the
 * clusters.
 *
 * @author DeB
 * @version 1.2 2022-08-04
 * @since 1.0
 */
public final class ClusterFileReader {
    
    /**
     * The constructor is private to avoid creating instances of this class, which offers only
     * static methods.
     */
    private ClusterFileReader() {
    
    }
    
    /**
     * Reads the data contained in the specified file, parsing it so to extract the information
     * about the points and the clusters.
     *
     * @param filePath Path of the file to read and parse.
     * @return The parsed points and clusters.
     * @throws MalformedFileException If the specified file does not follow the correct format.
     * @throws IOException If some I/O errors occur while reading the file.
     */
    @NotNull
    public static ParseResult parseDataFromFile(@NotNull final Path filePath)
        throws MalformedFileException, IOException {
        
        // Map that maps the identifiers of the points into their complete information
        final Map<@NotNull String, @NotNull ClusterPoint> identifiersToPointsMap = new HashMap<>();
        // Map that maps the identifiers of the clusters into their complete information
        final Map<@NotNull String, @NotNull Cluster> identifiersToClustersMap = new HashMap<>();
        
        // Read and parse the file
        final boolean biDimensional = RawDataFileParser.parseDataFromFile(filePath,
            identifiersToPointsMap,
            identifiersToClustersMap
        );
        
        // Loop over the clusters
        final Map<@NotNull String, @NotNull Boolean> analyzedClusters = new HashMap<>();
        for (final Cluster cluster : identifiersToClustersMap.values()) {
            ClusterFileReader.computeClusterPoints(cluster.getIdentifier(),
                identifiersToPointsMap,
                identifiersToClustersMap,
                analyzedClusters
            );
        }
        
        // Add the clusters
        return new ParseResult(biDimensional,
            new ArrayList<>(identifiersToPointsMap.values()),
            new ArrayList<>(identifiersToClustersMap.values())
        );
    }
    
    /**
     * Computes and sets the points belonging to the specified cluster.<br> In particular, if the
     * cluster with the specified identifier has already been analyzed, then the cluster is left
     * untouched, and the points belonging to it are returned.<br> If, instead, the cluster with the
     * specified identifier has not been analyzed, then this method recursively computes the points
     * belonging to the cluster, updates the {@link Cluster} object associated to the specified
     * identifier, and returns the computed points.
     *
     * @param clusterIdentifier Identifier of the cluster to compute the points belonging to.
     * @param identifiersToPointsMap Map that maps the identifiers of the points to the
     *     {@link ClusterPoint} objects containing their complete information.
     * @param identifiersToClustersMap Map that maps the identifiers of the clusters to the
     *     {@link Cluster} objects containing their complete information.
     * @param analyzedClusters Map that contains the identifiers of the clusters already been
     *     analyzed, or the ones being analyzed. In particular, if the cluster has been analyzed,
     *     then {@code true} is associated to the identifier, otherwise {@code false} is
     *     associated.
     * @return The points belonging to the cluster with the specified identifier.
     * @throws MalformedFileException If the cluster with the specified identifier does not
     *     exist, or if it contains unknown points or clusters.
     */
    @SuppressWarnings("FeatureEnvy")
    private static @Unmodifiable List<@NotNull ClusterPoint> computeClusterPoints(
        @NotNull final String clusterIdentifier,
        @Unmodifiable @NotNull
        final Map<@NotNull String, @NotNull ClusterPoint> identifiersToPointsMap,
        @Unmodifiable @NotNull
        final Map<@NotNull String, @NotNull Cluster> identifiersToClustersMap,
        @NotNull final Map<@NotNull String, @NotNull Boolean> analyzedClusters
    ) throws MalformedFileException {
        
        // Clear the result
        final List<@NotNull ClusterPoint> clusterPoints = new ArrayList<>();
        
        // Check if the cluster is a point
        if (clusterIdentifier.startsWith("P")) {
            // Extract the point
            final ClusterPoint point = identifiersToPointsMap.get(clusterIdentifier);
            // Check if it is valid
            if (point == null) {
                final String formatMessage = "Unknown point '%s'";
                throw new MalformedFileException(formatMessage.formatted(clusterIdentifier));
            }
            // This cluster contains only one point
            clusterPoints.add(point);
        } else {
            // Extract the cluster
            final Cluster cluster = identifiersToClustersMap.get(clusterIdentifier);
            // Check if it is valid
            if (cluster == null) {
                final String formatMessage = "Unknown cluster '%s'";
                throw new MalformedFileException(formatMessage.formatted(clusterIdentifier));
            }
            // Check if it has been already analyzed
            final Boolean alreadyAnalyzedCluster = analyzedClusters.get(clusterIdentifier);
            if (alreadyAnalyzedCluster == null) {
                // Analyze the cluster
                analyzedClusters.put(clusterIdentifier, false);
                // Compute the points in the first sub-cluster
                @Unmodifiable
                final List<@NotNull ClusterPoint> firstSubClusterPoints =
                    ClusterFileReader.computeClusterPoints(cluster.getFirstSubClusterIdentifier(),
                        identifiersToPointsMap,
                        identifiersToClustersMap,
                        analyzedClusters
                    );
                // Compute the points in the second sub-cluster
                @Unmodifiable
                final List<@NotNull ClusterPoint> secondSubClusterPoints =
                    ClusterFileReader.computeClusterPoints(cluster.getSecondSubClusterIdentifier(),
                        identifiersToPointsMap,
                        identifiersToClustersMap,
                        analyzedClusters
                    );
                
                // Compute the final result
                clusterPoints.addAll(firstSubClusterPoints);
                clusterPoints.addAll(secondSubClusterPoints);
                
                // The cluster has been analyzed
                cluster.setClusterPoints(clusterPoints);
                analyzedClusters.put(clusterIdentifier, true);
            } else if (!alreadyAnalyzedCluster) {
                // Circular dependency
                final String formatMessage = "Circular dependency found while parsing cluster '%s'";
                throw new MalformedFileException(formatMessage.formatted(clusterIdentifier));
            } else {
                // Return the points in the cluster
                return cluster.getClusterPoints();
            }
            
        }
        
        return clusterPoints;
    }
}
