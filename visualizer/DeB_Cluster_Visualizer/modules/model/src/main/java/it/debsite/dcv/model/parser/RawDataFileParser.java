package it.debsite.dcv.model.parser;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.model.MalformedFileException;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.io.BufferedReader;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Parser that parses the file containing the information about the points and the clusters, and
 * gives back a raw representation of them.
 *
 * @author DeB
 * @version 1.0 2022-08-04
 * @since 1.0
 */
public final class RawDataFileParser {
    
    /**
     * Pattern that allows to extract the information related to a point.
     */
    @SuppressWarnings("ConstantExpression")
    private static final Pattern POINT_PATTERN = Pattern.compile(
        "(?<id>P\\d+): \"(?<name>[^\"]*)\"(?<coords> (?<x1>[-]?\\d+[.]?\\d*) " +
            "(?<x2>[-]?\\d+[.]?\\d*)(?<othCoords>[\\s\\d.]*))?");
    
    /**
     * Pattern that allows to extract the information related to a cluster.
     */
    @SuppressWarnings("ConstantExpression")
    private static final Pattern CLUSTER_PATTERN = Pattern.compile(
        "(?<id>C\\d+): \"(?<name>[^\"]*)\" (?<first>[CP]\\d+) " +
            "(?<second>[CP]\\d+) (?<distance>[-]?\\d+[.]?\\d*)");
    
    /**
     * The constructor is private so to avoid creating object of this class, which offers only
     * static methods.
     */
    private RawDataFileParser() {
    
    }
    
    /**
     * Parse the data contained in the specified file.
     *
     * @param filePath File to parse.
     * @param identifiersToPointsMap Map that maps the identifiers of the points into their
     *     complete information.
     * @param identifiersToClustersMap Map that maps the identifiers of the clusters into their
     *     complete information.
     * @return {@code true} if exactly two coordinates are provided for all the points,
     *     {@code false} if, instead, no coordinates or more than 2 coordinates are provided for at
     *     least one point.
     * @throws MalformedFileException If the file to parse does snot follow the correct format.
     * @throws IOException If an I/O error occurs while reading the file.
     */
    public static boolean parseDataFromFile(
        @NotNull final Path filePath,
        @NotNull
        final Map<? super @NotNull String, ? super @NotNull ClusterPoint> identifiersToPointsMap,
        @NotNull
        final Map<? super @NotNull String, ? super @NotNull Cluster> identifiersToClustersMap
    ) throws MalformedFileException, IOException {
        
        // Create the two matcher used to parse the data
        final Matcher pointMatcher = RawDataFileParser.POINT_PATTERN.matcher("");
        final Matcher clusterMatcher = RawDataFileParser.CLUSTER_PATTERN.matcher("");
        
        // Read and parse all the content of file
        try (
            final BufferedReader reader = Files.newBufferedReader(filePath, StandardCharsets.UTF_8)
        ) {
            @Nullable String line;
            // Flag indicating whether all the points are bi-dimensional
            boolean biDimensionalPoints = true;
            // Number of the line being parsed
            int lineNumber = 1;
            do {
                // Read the next line from the file
                line = reader.readLine();
                
                // Skip blank lines
                if ((line != null) && !line.isBlank()) {
                    // Try to parse the line as a point
                    pointMatcher.reset(line);
                    if (pointMatcher.matches()) {
                        // Parse the raw point information
                        // noinspection NonShortCircuitBooleanExpression
                        biDimensionalPoints &=
                            RawDataFileParser.parsePoint(pointMatcher, identifiersToPointsMap);
                    } else {
                        // Try to parse the line as a cluster
                        clusterMatcher.reset(line);
                        if (clusterMatcher.matches()) {
                            // Parse the raw cluster information
                            RawDataFileParser.parseCluster(clusterMatcher,
                                identifiersToClustersMap
                            );
                        } else {
                            // The line is neither a point, nor a cluster
                            final String messageFormat =
                                "The line %d does not contain neither a point, nor a cluster";
                            throw new MalformedFileException(messageFormat.formatted(lineNumber));
                        }
                    }
                }
                // Move to the next line
                lineNumber++;
            } while (line != null);
            
            return biDimensionalPoints;
        }
    }
    
    /**
     * Parses a file line as a point.<br>
     *
     * @param pointMatcher Matcher that has matched the line of the file as a point.
     * @param identifiersToPointsMap Map that maps the identifiers of the points into their
     *     complete information.
     * @return {@code true} if exactly two coordinates are provided for the point, {@code false} if,
     *     instead, no coordinates or more than 2 coordinates are provided for the point.
     */
    private static boolean parsePoint(
        @NotNull final Matcher pointMatcher,
        @NotNull
        final Map<? super @NotNull String, ? super @NotNull ClusterPoint> identifiersToPointsMap
    ) {
        
        // Extract the parsed information
        final String pointIdentifier = pointMatcher.group("id");
        final String pointName = pointMatcher.group("name");
        final String coordinates = pointMatcher.group("coords");
        final String otherCoordinates = pointMatcher.group("othCoords");
        
        // Extract the coordinates, if provided
        final double x1;
        final double x2;
        boolean biDimensional = true;
        if ((coordinates != null) && !coordinates.isBlank()) {
            // Parse the coordinates
            x1 = Double.parseDouble(pointMatcher.group("x1"));
            x2 = Double.parseDouble(pointMatcher.group("x2"));
        } else {
            // No coordinates provided, hence the points are assumed not to be bi-dimensional
            x1 = 0;
            x2 = 0;
            biDimensional = false;
        }
        
        // Create the point
        final ClusterPoint point = new ClusterPoint(pointIdentifier, pointName, x1, x2);
        // Add the point
        identifiersToPointsMap.put(pointIdentifier, point);
        
        // If more than 2 coordinates have been provided, then the point is not bi-dimensional
        return ((otherCoordinates == null) || otherCoordinates.isBlank()) && biDimensional;
    }
    
    /**
     * Parse a file line as a cluster.
     *
     * @param clusterMatcher Matcher that has matched the line of the file as a cluster.
     * @param identifiersToClustersMap Map that maps the identifiers of the clusters into their
     *     complete information.
     */
    private static void parseCluster(
        @NotNull final Matcher clusterMatcher,
        @NotNull
        final Map<? super @NotNull String, ? super @NotNull Cluster> identifiersToClustersMap
    ) {
        
        // Extract the parsed information
        final String clusterIdentifier = clusterMatcher.group("id");
        final String clusterName = clusterMatcher.group("name");
        final String firstIdentifier = clusterMatcher.group("first");
        final String secondIdentifier = clusterMatcher.group("second");
        final double distance = Double.parseDouble(clusterMatcher.group("distance"));
        
        // Create the raw cluster information
        final Cluster information = new Cluster(clusterIdentifier,
            clusterName,
            firstIdentifier,
            secondIdentifier,
            distance
        );
        
        // Add the cluster
        identifiersToClustersMap.put(clusterIdentifier, information);
    }
}
