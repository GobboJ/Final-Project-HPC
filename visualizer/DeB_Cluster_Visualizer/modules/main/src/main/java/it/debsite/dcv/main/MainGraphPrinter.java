package it.debsite.dcv.main;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.model.file.ClusterData;
import it.debsite.dcv.model.file.ClusterFileReader;
import it.debsite.dcv.model.file.MalformedFileException;
import it.debsite.dcv.presenter.GraphPresenter;
import it.debsite.dcv.view.GraphPrinter;

import javax.imageio.ImageIO;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import java.awt.Color;
import java.awt.EventQueue;
import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class MainGraphPrinter {
    
    public static void main(final String[] args) {
        
        EventQueue.invokeLater(MainGraphPrinter::start);
    }
    
    private static void start() {
        
        try {
            final Path inputPath = Path.of("..", "..", "out", "out.txt");
            ClusterFileReader reader = new ClusterFileReader();
            final ClusterData clusterData = reader.readData(inputPath);
            
            if (clusterData.isMultiDimensional()) {
                System.err.println("Only bi-dimensional data can be processed by the GraphPrinter");
                System.exit(1);
            }
            
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            final BufferedImage image = new BufferedImage(620, 620, BufferedImage.TYPE_INT_ARGB);
            Graphics2D context = image.createGraphics();
            context.setColor(Color.WHITE);
            context.fill(new Rectangle2D.Double(0, 0, image.getWidth(), image.getHeight()));
            
            final GraphPrinter graphPrinter = new GraphPrinter(image);
            
            final GraphPresenter graphPresenter = new GraphPresenter(clusterData.getPoints(),
                clusterData.getClusters(),
                image,
                graphPrinter
            );
            
            final Path outputPath = Path.of("..", "..", "out", "out.png");
            ImageIO.write(image, "png", outputPath.toAbsolutePath().toFile());
        } catch (final MalformedFileException | IOException | ClassNotFoundException |
                       InstantiationException | IllegalAccessException |
                       UnsupportedLookAndFeelException exception) {
            // TODO: Auto-generated block
            exception.printStackTrace();
        }
    }
}
