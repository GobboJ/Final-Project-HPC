package it.debsite.dcv.main;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.model.file.ClusterFileReader;
import it.debsite.dcv.model.file.MalformedFileException;
import it.debsite.dcv.presenter.DendrogramPresenter;
import it.debsite.dcv.presenter.GraphPresenter;
import it.debsite.dcv.view.DendrogramPrinter;
import it.debsite.dcv.view.GraphPrinter;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.LookAndFeel;
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
 * @version 1.0 2022-07-10
 * @since version date
 */
public class MainDendrogramPrinter {
    
    public static void main(final String[] args) {
        
        EventQueue.invokeLater(MainDendrogramPrinter::start);
    }
    
    private static void start() {
        
        final List<ClusterPoint> points = new ArrayList<>();
        final List<Cluster> clusters = new ArrayList<>();
        try {
            final Path inputPath = Path.of("..", "..", "out", "out.txt");
            final ClusterFileReader reader = new ClusterFileReader(inputPath, points, clusters);
        } catch (final MalformedFileException | IOException exception) {
            // TODO: Auto-generated block
            exception.printStackTrace();
        }
        
        try {
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            final BufferedImage image = new BufferedImage(1920, 1080, BufferedImage.TYPE_INT_ARGB);
            
            final Graphics2D context = image.createGraphics();
            context.setColor(Color.WHITE);
            context.fill(new Rectangle2D.Double(0, 0, image.getWidth(), image.getHeight()));
            
            final DendrogramPrinter printer = new DendrogramPrinter(image);
            final DendrogramPresenter presenter =
                new DendrogramPresenter(image, printer, clusters, points);
            
            final Path outputPath = Path.of("..", "..", "out", "dendrogram.png");
            ImageIO.write(image, "png", outputPath.toAbsolutePath().toFile());
        } catch (final IOException | ClassNotFoundException | InstantiationException |
                       IllegalAccessException | UnsupportedLookAndFeelException exception) {
            // TODO: Auto-generated block
            exception.printStackTrace();
        }
    }
    
}
