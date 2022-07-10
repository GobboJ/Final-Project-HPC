package it.debsite.dcv.model.file;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import lombok.AllArgsConstructor;
import lombok.Getter;

import java.io.BufferedReader;
import java.io.FileInputStream;
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
    private static enum Type {
        POINT_POINT(true, true),
        CLUSTER_POINT(false, true),
        POINT_CLUSTER(true, false),
        CLUSTER_CLUSTER(false, false);
        
        private final boolean firstPoint;
        
        private final boolean secondPoint;
    }
    
    @AllArgsConstructor
    @Getter
    private static class ClusterInfo {
        
        private String name;
        
        private Type type;
        
        private int firstIndex;
        
        private int secondIndex;
    }
    
    private static final Pattern POINT_PATTERN = Pattern.compile(
        "P(?<num>\\d+): \"(?<name>[^\"]*)\" (?<x1>[-]?\\d+[.]?\\d*) (?<x2>[-]?\\d+[.]?\\d*)");
    
    private static final Pattern CLUSTER_PATTERN = Pattern.compile(
        "C(?<num>\\d+): \"(?<name>[^\"]*)\" (?<cOrP1>[CP])(?<first>\\d+) (?<cOrP2>[CP])" +
            "(?<second>\\d+) (?<distance>[-]?\\d+[.]?\\d*)");
    
    public ClusterFileReader(
        final Path filePath, final List<ClusterPoint> points, final List<Cluster> clusters
    ) throws MalformedFileException, IOException {
        
        final Map<Integer, ClusterPoint> pointsMap = new HashMap<>();
        final Map<Integer, ClusterInfo> clustersInfo = new HashMap<>();
        
        try (
            final BufferedReader reader = Files.newBufferedReader(filePath, StandardCharsets.UTF_8)
        ) {
            String line;
            do {
                line = reader.readLine();
                if ((line != null) && !line.isBlank()) {
                    final Matcher pointMatcher =
                        ClusterFileReader.POINT_PATTERN.matcher(line.trim());
                    
                    if (pointMatcher.matches()) {
                        final int pointNumber = Integer.parseInt(pointMatcher.group("num"));
                        final String pointName = pointMatcher.group("name");
                        final double x1 = Double.parseDouble(pointMatcher.group("x1"));
                        final double x2 = Double.parseDouble(pointMatcher.group("x2"));
                        
                        final ClusterPoint point = new ClusterPoint(pointName, x1, x2);
                        pointsMap.put(pointNumber, point);
                        points.add(point);
                    } else {
                        final Matcher clusterMatcher =
                            ClusterFileReader.CLUSTER_PATTERN.matcher(line);
                        if (clusterMatcher.matches()) {
                            final int clusterNumber = Integer.parseInt(clusterMatcher.group("num"));
                            final String clusterName = clusterMatcher.group("name");
                            final boolean isFirstPoint = clusterMatcher.group("cOrP1").equals("P");
                            final boolean isSecondPoint = clusterMatcher.group("cOrP2").equals("P");
                            final int firstIndex = Integer.parseInt(clusterMatcher.group("first"));
                            final int secondIndex = Integer.parseInt(clusterMatcher.group("second"));
                            
                            final Type type;
                            if (isFirstPoint && isSecondPoint) {
                                type = Type.POINT_POINT;
                            } else if (!isFirstPoint && !isSecondPoint) {
                                type = Type.CLUSTER_CLUSTER;
                            } else if (isFirstPoint) {
                                type = Type.POINT_CLUSTER;
                            } else {
                                type = Type.CLUSTER_POINT;
                            }
                            
                            clustersInfo.put(clusterNumber,
                                new ClusterInfo(clusterName, type, firstIndex, secondIndex)
                            );
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
                    clusters
                );
            }
        }
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
        if (clusterInfo.getType().isFirstPoint()) {
            ClusterFileReader.addPoint(clusterPoints,
                clusterInfo.getFirstIndex(),
                clusterIndex,
                pointsMap
            );
        } else {
            ClusterFileReader.addCluster(clusterPoints,
                clusterInfo.getFirstIndex(),
                clusterIndex,
                clustersInfo,
                pointsMap,
                clustersMap,
                clusters
            );
        }
        
        if (clusterInfo.getType().isSecondPoint()) {
            ClusterFileReader.addPoint(clusterPoints,
                clusterInfo.getSecondIndex(),
                clusterIndex,
                pointsMap
            );
        } else {
            ClusterFileReader.addCluster(clusterPoints,
                clusterInfo.getSecondIndex(),
                clusterIndex,
                clustersInfo,
                pointsMap,
                clustersMap,
                clusters
            );
        }
        
        final Cluster cluster = new Cluster(clusterInfo.getName(), clusterPoints);
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
