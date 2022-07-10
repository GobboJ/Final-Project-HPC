package it.debsite.dcv.model;

import lombok.AllArgsConstructor;
import lombok.Getter;
import org.jetbrains.annotations.NotNull;

import java.util.Collections;
import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
@AllArgsConstructor
public class Cluster {
    
    @NotNull
    @Getter
    private String name;
    
    @NotNull
    private List<@NotNull ClusterPoint> clusterPoints;
    
    public List<ClusterPoint> getClusterPoints() {
        
        return Collections.unmodifiableList(this.clusterPoints);
    }
}
