package it.debsite.dcv.presenter.utils;

import it.debsite.dcv.model.ClusterPoint;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.jetbrains.annotations.UnmodifiableView;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Utility class holding the distance matrix. In particular, given a list of points, this class
 * computes and stores efficiently all the pairwise distances.
 *
 * @author DeB
 * @version 1.2 2022-08-04
 * @since 1.0
 */
public class DistanceMatrix {
    
    /**
     * Compacted distance matrix.
     */
    private final double @NotNull [] matrix;
    
    /**
     * Number of columns in the full distance matrix. Since the matrix is symmetric, this also
     * corresponds to the number of rows.
     */
    private final int columnsCount;
    
    /**
     * Maps mapping the identifiers of the points into the row index of the full distance matrix
     * that contains the pairwise distances between the point and all the other points.
     */
    @NotNull
    private final Map<@NotNull String, @NotNull Integer> pointsRowIndexes;
    
    /**
     * Computes and stores all the pairwise distances between the specified points. Any change made
     * in the coordinates of the points after the distance matrix has been computed is not
     * reflected.
     *
     * @param points Points to compute the pairwise distances of.
     */
    public DistanceMatrix(
        @NotNull @UnmodifiableView final List<? extends @NotNull ClusterPoint> points
    ) {
        
        this.columnsCount = points.size();
        
        // Allocate the compacted distance matrix
        final int size = DistanceMatrix.computeSumOfFirstN(this.columnsCount - 1);
        this.matrix = new double[size];
        
        // Compute the distance between all the points, and store the association between the
        // point identifier and the index of the row containing the distance between the point
        // and all the other points
        this.pointsRowIndexes = new HashMap<>();
        for (int i = 0; i < points.size(); i++) {
            // Add the association point identifier to row index
            this.pointsRowIndexes.put(points.get(i).getIdentifier(), i);
            // Compute the distance from all the previous points
            for (int j = i + 1; j < points.size(); j++) {
                this.matrix[this.computeIndex(i, j)] =
                    DistanceMatrix.computeDistance(points.get(i), points.get(j));
            }
        }
    }
    
    /**
     * Returns the distance between the two points. The points are searched in the distance matrix
     * by its identifier.
     *
     * @param firstPoint First point.
     * @param secondPoint Second point.
     * @return The distance between the two points.
     * @throws IllegalArgumentException If:
     *     <ul>
     *         <li>One of the specified points has not been found in the distance matrix;</li>
     *         <li>The specified two points are the same point.</li>
     *     </ul>
     * @see ClusterPoint#getIdentifier()
     */
    public double getDistance(
        @NotNull final ClusterPoint firstPoint, @NotNull final ClusterPoint secondPoint
    ) {
        
        // Extract the index of the row in the full distance matrix corresponding to the first point
        @Nullable
        final Integer firstPointRowIndex = this.pointsRowIndexes.get(firstPoint.getIdentifier());
        // Extract the index of the row in the full distance matrix corresponding to the second
        // point
        @Nullable
        final Integer secondPointRowIndex = this.pointsRowIndexes.get(secondPoint.getIdentifier());
        
        // Require that the two points exist in the matrix
        if (firstPointRowIndex == null) {
            throw new IllegalArgumentException("The specified first point has not been found");
        }
        if (secondPointRowIndex == null) {
            throw new IllegalArgumentException("The specified second point has not been found");
        }
        
        // Require the two points to be different
        if (firstPointRowIndex.equals(secondPointRowIndex)) {
            throw new IllegalArgumentException("The two specified points must be different");
        }
        
        // Since the full distance matrix is symmetric, one of the two row indexes becomes the
        // column index.
        // Moreover, since the distance matrix is stored compactly by this class, the smallest of
        // the two indexes becomes the index of the row to access in the full distance matrix,
        // while the other index becomes the index of the column to access in the full distance
        // matrix
        final int rowIndex;
        final int columnIndex;
        if (firstPointRowIndex < secondPointRowIndex) {
            rowIndex = firstPointRowIndex;
            columnIndex = secondPointRowIndex;
        } else {
            rowIndex = secondPointRowIndex;
            columnIndex = firstPointRowIndex;
        }
        
        // Extract the value of the distance matrix at the row and column index
        return this.matrix[this.computeIndex(rowIndex, columnIndex)];
    }
    
    /**
     * Computes the Euclidean distance between two points.
     *
     * @param firstPoint First point.
     * @param secondPoint Second point.
     * @return The distance between the two points.
     */
    @SuppressWarnings("FeatureEnvy")
    private static double computeDistance(
        @NotNull final ClusterPoint firstPoint, @NotNull final ClusterPoint secondPoint
    ) {
        
        final double x1Difference = firstPoint.getX1Coordinate() - secondPoint.getX1Coordinate();
        final double x2Difference = firstPoint.getX2Coordinate() - secondPoint.getX2Coordinate();
        
        return StrictMath.sqrt((x1Difference * x1Difference) + (x2Difference * x2Difference));
    }
    
    /**
     * Computes the index in the linearized distance matrix where element at row {@code row} and
     * column {@code column} is stored.
     *
     * @param row Row of the element to compute the index of.
     * @param column Column of the element to compute the index of.
     * @return The computed index.
     */
    private int computeIndex(final int row, final int column) {
        
        return ((this.columnsCount * row) - DistanceMatrix.computeSumOfFirstN(row)) +
                   (column - (row + 1));
    }
    
    /**
     * Sums the first {@code count} natural numbers.
     *
     * @param count Number of natural numbers to sum together.
     * @return The computed sum.
     */
    private static int computeSumOfFirstN(final int count) {
        
        return (count * (count + 1)) / 2;
    }
}
