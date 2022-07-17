package it.debsite.dcv.model.file;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import lombok.AccessLevel;
import lombok.Getter;
import lombok.Setter;

import java.util.ArrayList;
import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-17
 * @since version date
 */
@Getter
public class ClusterData {
    
    @Setter(AccessLevel.PACKAGE)
    private boolean multiDimensional;
    
    private final List<ClusterPoint> points;
    
    private final List<Cluster> clusters;
    
    ClusterData() {
        
        this.multiDimensional = false;
        this.points = new ArrayList<>();
        this.clusters = new ArrayList<>();
    }
}
