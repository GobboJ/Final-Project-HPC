package it.debsite.dcv.main;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.presenter.GraphPresenter;
import it.debsite.dcv.view.GraphPrinter;

import javax.imageio.ImageIO;
import java.awt.Color;
import java.awt.EventQueue;
import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class MainApplication {
    
    public static void main(final String[] args) {
        
        EventQueue.invokeLater(MainApplication::start);
    }
    
    private static void start() {
        
        final ClusterPoint a = new ClusterPoint("A", 1, 1);
        final ClusterPoint b = new ClusterPoint("B", 1.5, 1.5);
        final ClusterPoint c = new ClusterPoint("C", 5, 5);
        final ClusterPoint d = new ClusterPoint("D", 3, 4);
        final ClusterPoint e = new ClusterPoint("E", 4, 4);
        final ClusterPoint f = new ClusterPoint("F", 3, 3.5);
        
        final List<ClusterPoint> points = new ArrayList<>();
        Collections.addAll(points, a, b, c, d, e, f);
        
        final List<ClusterPoint> abCluster = new ArrayList<>();
        Collections.addAll(abCluster, a, b);
        
        final List<ClusterPoint> dfCluster = new ArrayList<>();
        Collections.addAll(dfCluster, d, f);
        
        final List<ClusterPoint> dfeCluster = new ArrayList<>();
        Collections.addAll(dfeCluster, d, f, e);
        
        final List<ClusterPoint> dfecCluster = new ArrayList<>();
        Collections.addAll(dfecCluster, d, f, e, c);
        
        final List<ClusterPoint> allCluster = new ArrayList<>();
        Collections.addAll(allCluster, a, b, d, f, e, c);
        
        final List<Cluster> clusters = new ArrayList<>();
        Collections.addAll(
            clusters,
            new Cluster("AB", abCluster),
            new Cluster("DF", dfCluster),
            new Cluster("DFE", dfeCluster),
            new Cluster("DFEC", dfecCluster),
            new Cluster("ALL", allCluster)
        );
        
        try {
            final BufferedImage image = new BufferedImage(620, 620, BufferedImage.TYPE_INT_ARGB);
            Graphics2D context = image.createGraphics();
            context.setColor(Color.WHITE);
            context.fill(new Rectangle2D.Double(0, 0, image.getWidth(), image.getHeight()));
            
            final GraphPrinter graphPrinter = new GraphPrinter(image);
            
            final GraphPresenter graphPresenter =
                new GraphPresenter(points, clusters, image, graphPrinter);
            
            final Path outputPath = Path.of("..", "..", "out", "out.png");
            ImageIO.write(image, "png", outputPath.toAbsolutePath().toFile());
        } catch (final IOException exception) {
            // TODO: Auto-generated block
            exception.printStackTrace();
        }
    }
}
