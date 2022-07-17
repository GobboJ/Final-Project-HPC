package it.debsite.dcv.model.file;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import lombok.AllArgsConstructor;
import lombok.Getter;

import java.io.BufferedReader;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-10
 * @since version date
 */
public class ClusterFileReader {
    
    @AllArgsConstructor
    @Getter
    private static class ClusterInfo {
        
        private String identifier;
        
        private String name;
        
        private String leftIdentifier;
        
        private int leftIndex;
        
        private String rightIdentifier;
        
        private int rightIndex;
        
        private double distance;
        
    }
    
    @SuppressWarnings("ConstantExpression")
    private static final Pattern POINT_PATTERN = Pattern.compile(
        "(?<id>P(?<num>\\d+)): \"(?<name>[^\"]*)\"(?<other> (?<x1>[-]?\\d+[.]?\\d*) " +
            "(?<x2>[-]?\\d+[.]?\\d*)(?<multi>[\\s\\d.]*))?");
    
    @SuppressWarnings("ConstantExpression")
    private static final Pattern CLUSTER_PATTERN = Pattern.compile(
        "(?<id>C(?<num>\\d+)): \"(?<name>[^\"]*)\" (?<first>[CP](?<firstIndex>\\d+)) " +
            "(?<second>[CP](?<secondIndex>\\d+)) (?<distance>[-]?\\d+[.]?\\d*)");
    
    public ClusterData readData(final Path filePath) throws MalformedFileException, IOException {
        
        final Map<Integer, ClusterPoint> pointsMap = new HashMap<>();
        final Map<Integer, ClusterInfo> clustersInfo = new HashMap<>();
        final ClusterData clusterData = new ClusterData();
    
        final Matcher pointMatcher =
            ClusterFileReader.POINT_PATTERN.matcher("");
        final Matcher clusterMatcher =
            ClusterFileReader.CLUSTER_PATTERN.matcher("");
        
        try (
            final BufferedReader reader = Files.newBufferedReader(filePath, StandardCharsets.UTF_8)
        ) {
            String line;
            do {
                line = reader.readLine();
                if ((line != null) && !line.isBlank()) {
                    pointMatcher.reset(line);
                    
                    if (pointMatcher.matches()) {
                        final String pointIdentifier = pointMatcher.group("id");
                        final int pointNumber = Integer.parseInt(pointMatcher.group("num"));
                        final String pointName = pointMatcher.group("name");
                        final String other = pointMatcher.group("other");
                        final String multi = pointMatcher.group("multi");
                        
                        final double x1;
                        final double x2;
                        if ((other != null) && !other.isBlank()) {
                            x1 = Double.parseDouble(pointMatcher.group("x1"));
                            x2 = Double.parseDouble(pointMatcher.group("x2"));
                        } else {
                            x1 = 0;
                            x2 = 0;
                        }
                        
                        final ClusterPoint point =
                            new ClusterPoint(pointIdentifier, pointName, x1, x2);
                        pointsMap.put(pointNumber, point);
                        clusterData.getPoints().add(point);
                        
                        if ((multi != null) && !multi.isBlank()) {
                            clusterData.setMultiDimensional(true);
                        }
                    } else {
                        clusterMatcher.reset(line);
                        if (clusterMatcher.matches()) {
                            final String clusterIdentifier = clusterMatcher.group("id");
                            final int clusterNumber = Integer.parseInt(clusterMatcher.group("num"));
                            final String clusterName = clusterMatcher.group("name");
                            
                            final String leftIdentifier = clusterMatcher.group("first");
                            final int leftIndex = Integer.parseInt(clusterMatcher.group("firstIndex"));
                            
                            final String rightIdentifier = clusterMatcher.group("second");
                            final int rightIndex =
                                Integer.parseInt(clusterMatcher.group("secondIndex"));
                            
                            final double distance =
                                Double.parseDouble(clusterMatcher.group("distance"));
                            
                            clustersInfo.put(clusterNumber, new ClusterInfo(clusterIdentifier,
                                clusterName,
                                leftIdentifier,
                                leftIndex,
                                rightIdentifier,
                                rightIndex,
                                distance
                            ));
                        } else {
                            throw new MalformedFileException("Malformed line '%s'".formatted(line));
                        }
                        
                    }
                }
            } while (line != null);
        }
        
        // Loop over the clusters
        final Map<Integer, Cluster> clustersMap = new HashMap<>();
        for (final Entry<Integer, ClusterInfo> clusterInfoEntry : clustersInfo.entrySet()) {
            final int clusterIndex = clusterInfoEntry.getKey();
            
            if (!clustersMap.containsKey(clusterIndex)) {
                ClusterFileReader.addCluster(clustersInfo,
                    clusterIndex,
                    pointsMap,
                    clustersMap,
                    clusterData.getClusters()
                );
            }
        }
        
        return clusterData;
    }
    
    private static void addCluster(
        final Map<Integer, ClusterInfo> clustersInfo,
        final int clusterIndex,
        final Map<Integer, ClusterPoint> pointsMap,
        final Map<Integer, Cluster> clustersMap,
        final List<Cluster> clusters
    ) throws MalformedFileException {
        
        final ClusterInfo clusterInfo = clustersInfo.get(clusterIndex);
        if (clusterInfo == null) {
            throw new MalformedFileException("Cluster %d is invalid".formatted(clusterIndex));
        }
        final List<ClusterPoint> clusterPoints = new ArrayList<>();
        
        // Check if the first is a point
        if (clusterInfo.getLeftIdentifier().startsWith("P")) {
            ClusterFileReader.addPoint(clusterPoints,
                clusterInfo.getLeftIndex(),
                clusterIndex,
                pointsMap
            );
        } else {
            ClusterFileReader.addCluster(clusterPoints,
                clusterInfo.getLeftIndex(),
                clusterIndex,
                clustersInfo,
                pointsMap,
                clustersMap,
                clusters
            );
        }
        
        if (clusterInfo.getRightIdentifier().startsWith("P")) {
            ClusterFileReader.addPoint(clusterPoints,
                clusterInfo.getRightIndex(),
                clusterIndex,
                pointsMap
            );
        } else {
            ClusterFileReader.addCluster(clusterPoints,
                clusterInfo.getRightIndex(),
                clusterIndex,
                clustersInfo,
                pointsMap,
                clustersMap,
                clusters
            );
        }
        
        final Cluster cluster = new Cluster(clusterInfo.getIdentifier(),
            clusterInfo.getName(),
            clusterInfo.getLeftIdentifier(),
            clusterInfo.getRightIdentifier(),
            clusterInfo.getDistance(),
            clusterPoints
        );
        clusters.add(cluster);
        clustersMap.put(clusterIndex, cluster);
    }
    
    private static void addPoint(
        final List<ClusterPoint> clusterPoints,
        final int pointIndex,
        final int clusterIndex,
        final Map<Integer, ClusterPoint> pointsMap
    
    ) throws MalformedFileException {
        
        final ClusterPoint point = pointsMap.get(pointIndex);
        if (point == null) {
            throw new MalformedFileException("Cluster %d contains invalid point %d".formatted(clusterIndex,
                pointIndex
            ));
        }
        clusterPoints.add(point);
    }
    
    private static void addCluster(
        final List<ClusterPoint> clusterPoints,
        final int otherClusterIndex,
        final int clusterIndex,
        final Map<Integer, ClusterInfo> clustersInfo,
        final Map<Integer, ClusterPoint> pointsMap,
        final Map<Integer, Cluster> clustersMap,
        final List<Cluster> clusters
    ) throws MalformedFileException {
        
        if (!clustersMap.containsKey(otherClusterIndex)) {
            ClusterFileReader.addCluster(clustersInfo,
                otherClusterIndex,
                pointsMap,
                clustersMap,
                clusters
            );
        }
        final Cluster cluster = clustersMap.get(otherClusterIndex);
        if (cluster == null) {
            throw new MalformedFileException("Cluster %d contains invalid point %d".formatted(clusterIndex,
                otherClusterIndex
            ));
        }
        clusterPoints.addAll(cluster.getClusterPoints());
    }
}
