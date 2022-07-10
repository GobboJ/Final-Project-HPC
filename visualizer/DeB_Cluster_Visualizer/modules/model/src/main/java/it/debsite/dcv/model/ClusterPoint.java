package it.debsite.dcv.model;

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
public class ClusterPoint {

    @NotNull
    private String name;
    private double x1;
    private double x2;
}
