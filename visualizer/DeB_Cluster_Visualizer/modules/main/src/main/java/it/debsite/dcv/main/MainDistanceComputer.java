package it.debsite.dcv.main;

import java.awt.EventQueue;
import java.awt.geom.Point2D;
import java.math.BigDecimal;
import java.math.MathContext;
import java.math.RoundingMode;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-10
 * @since version date
 */
public class MainDistanceComputer {
    
    public static void main(final String[] args) {
        
        final Point2D p1 = new Point2D.Double(4, 4);
        final Point2D p2 = new Point2D.Double(3, 3.5);
        BigDecimal xDiff = BigDecimal.valueOf(p1.getX() - p2.getX());
        BigDecimal yDiff = BigDecimal.valueOf(p1.getY() - p2.getY());
        xDiff = xDiff.multiply(xDiff);
        yDiff = yDiff.multiply(yDiff);
        final BigDecimal sum = xDiff.add(yDiff);
        System.out.println(sum.sqrt(new MathContext(3, RoundingMode.HALF_EVEN)));
        //System.out.println(sum.sqrt(MathContext.DECIMAL128));
    }
}
