package it.debsite.dcv.model;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.ToString;
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
@ToString
public class ClusterPoint {

    @NotNull
    private String name;
    private double x1;
    private double x2;
}
