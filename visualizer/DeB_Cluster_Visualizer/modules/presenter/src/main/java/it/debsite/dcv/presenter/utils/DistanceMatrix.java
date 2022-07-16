package it.debsite.dcv.presenter.utils;

import it.debsite.dcv.model.ClusterPoint;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-16
 * @since version date
 */
public class DistanceMatrix {
    
    private final double[] matrix;
    
    private final int columnsCount;
    
    private final Map<String, Integer> pointsIndexes;
    
    public DistanceMatrix(final List<ClusterPoint> points) {
        
        this.columnsCount = points.size();
        final int size = this.sumFirstN(this.columnsCount - 1);
        this.matrix = new double[size];
        
        this.pointsIndexes = new HashMap<>();
        
        for (int i = 0; i < points.size(); i++) {
            this.pointsIndexes.put(points.get(i).getIdentifier(), i);
            for (int j = i + 1; j < points.size(); j++) {
                this.matrix[this.computeIndex(i, j)] =
                    DistanceMatrix.computeDistance(points.get(i), points.get(j));
            }
        }
    }
    
    public double getDistance(final ClusterPoint firstPoint, final ClusterPoint secondPoint) {
        
        final int firstIndex = this.pointsIndexes.get(firstPoint.getIdentifier());
        final int secondIndex = this.pointsIndexes.get(secondPoint.getIdentifier());
        
        final int rowIndex;
        final int columnIndex;
        if (firstIndex < secondIndex) {
            rowIndex = firstIndex;
            columnIndex = secondIndex;
        } else {
            rowIndex = secondIndex;
            columnIndex = firstIndex;
        }
        
        return this.matrix[this.computeIndex(rowIndex, columnIndex)];
    }
    
    private static double computeDistance(
        final ClusterPoint firstPoint, final ClusterPoint secondPoint
    ) {
        
        final double x1Difference = firstPoint.getX1() - secondPoint.getX1();
        final double x2Difference = firstPoint.getX2() - secondPoint.getX2();
        
        return StrictMath.sqrt((x1Difference * x1Difference) + (x2Difference * x2Difference));
    }
    
    private int computeIndex(final int row, final int column) {
        
        /*
         * colCount = 8
         * p (3, 5) -> all cols in row 0 + all cols in row 1 + all cols in row 2 + col 5
         *          -> (colCount -1) + (colCount -2) + (colCount - 3) + (5-(3+1))
         *          -> 7 + 6 + 5 + 1 = 19
         *
         *          -> colCount * 3 + (-1-2-3) + (5-(3+1))
         *          -> 8 * 3 -6 + 1
         *          -> 24 - 6 + 1
         *          -> 18 +1 = 19
         *
         * p (6, 7) = 7 + 6 + 5 + 4 + 3 + 2 + 1 = 27
         *
         * colCount = 6
         * p (1, 3) -> colCount -1 + (3-(1+1))
         *          -> 5 + (3 - 2)
         *          -> 5 + 1 = 6
         */
        
        // 6 +
        
        return ((this.columnsCount * row) - this.sumFirstN(row)) + (column - (row + 1));
    }
    
    private int sumFirstN(final int n) {
        
        return (n * (n + 1)) / 2;
    }
}
