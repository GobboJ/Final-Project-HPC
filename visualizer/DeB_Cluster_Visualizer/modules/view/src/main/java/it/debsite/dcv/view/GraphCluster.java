package it.debsite.dcv.view;

import lombok.AllArgsConstructor;
import lombok.Getter;
import org.jetbrains.annotations.NotNull;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
@AllArgsConstructor
@Getter
public class GraphCluster {
    
    @NotNull
    private String name;
    
    private double minX;
    
    private double minY;
    
    private double maxX;
    
    private double maxY;
}
