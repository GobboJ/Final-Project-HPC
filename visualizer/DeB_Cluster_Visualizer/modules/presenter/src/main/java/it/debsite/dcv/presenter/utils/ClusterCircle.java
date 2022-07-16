package it.debsite.dcv.presenter.utils;

import lombok.AllArgsConstructor;
import lombok.Getter;

import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-16
 * @since version date
 */
@AllArgsConstructor
@Getter
public class ClusterCircle {
    
    private final String name;
    
    private final double centerX1;
    
    private final double centerX2;
    
    private final double radius;
    
    private final List<ClusterCircle> internalClusterCircles;
    
}
