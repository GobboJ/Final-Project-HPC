package it.debsite.dcv.presenter.utils;

import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.view.GraphPoint;

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
    
    private final double[][] matrix;
    
    private final Map<String, Integer> pointsPositions;
    
    public DistanceMatrix(final List<ClusterPoint> points) {
        
        this.matrix = new double[points.size()][points.size()];
        this.pointsPositions = new HashMap<>();
        
        for (int i = 0; i < points.size(); i++) {
            this.pointsPositions.put(points.get(i).getIdentifier(), i);
            for (int j = 0; j < points.size(); j++) {
                this.matrix[i][j] = DistanceMatrix.computeDistance(points.get(i), points.get(j));
            }
        }
    }
    
    public double getDistance(final ClusterPoint firstPoint, final ClusterPoint secondPoint) {
        
        final int firstIndex = this.pointsPositions.get(firstPoint.getIdentifier());
        final int secondIndex = this.pointsPositions.get(secondPoint.getIdentifier());
        
        return this.matrix[firstIndex][secondIndex];
    }
    
    private static double computeDistance(
        final ClusterPoint firstPoint, final ClusterPoint secondPoint
    ) {
        
        final double x1Difference = firstPoint.getX1() - secondPoint.getX1();
        final double x2Difference = firstPoint.getX2() - secondPoint.getX2();
        
        return StrictMath.sqrt((x1Difference * x1Difference) + (x2Difference * x2Difference));
    }
}
