package it.debsite.dcv.main;

import it.debsite.dcv.main.CliArgumentsParser.CliArguments;
import it.debsite.dcv.model.ClusterFileReader;
import it.debsite.dcv.model.MalformedFileException;
import it.debsite.dcv.model.ParseResult;
import it.debsite.dcv.presenter.GraphPresenter;
import it.debsite.dcv.view.graph.GraphPrinter;

import javax.imageio.ImageIO;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import java.awt.Color;
import java.awt.EventQueue;
import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.IOException;

/**
 * Application that prints a graph containing points and the cluster enclosing them.
 *
 * @author DeB
 * @version 1.1 2022-08-04
 * @since 1.0
 */
public final class MainGraphPrinter {
    
    /**
     * The constructor is private so to avoid creating objects of this class, which offers only
     * static methods.
     */
    private MainGraphPrinter() {
    
    }
    
    /**
     * Main entry point of the application.
     *
     * @param args Arguments provided via command line.
     */
    @SuppressWarnings("MethodCanBeVariableArityMethod")
    public static void main(final String[] args) {
        
        // Parse the command line arguments
        final CliArguments arguments =
            CliArgumentsParser.parseCommandLineArguments(args, MainGraphPrinter::usage);
        
        // Draw the graph
        EventQueue.invokeLater(() -> MainGraphPrinter.start(arguments));
    }
    
    /**
     * Start function, called on the AWT thread, that prints the graph onto the image.
     *
     * @param parsedArguments Arguments specified on the command line.
     */
    @SuppressWarnings({"UseOfSystemOutOrSystemErr", "FeatureEnvy"})
    private static void start(final CliArguments parsedArguments) {
        
        try {
            // Parse the data of the input file
            final ParseResult parseResult =
                ClusterFileReader.parseDataFromFile(parsedArguments.getInputPath());
            
            // Require the points to be bi-dimensional
            if (!parseResult.isBiDimensional()) {
                System.err.println("Only bi-dimensional data can be processed by the GraphPrinter");
                MainGraphPrinter.usage();
                // noinspection CallToSystemExit
                System.exit(1);
            }
            
            // Make the look and feel of the application to match the system one
            // noinspection DuplicatedCode
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            // Create a white image
            final BufferedImage image = new BufferedImage(
                parsedArguments.getImageWidth(),
                parsedArguments.getImageHeight(),
                BufferedImage.TYPE_INT_ARGB
            );
            final Graphics2D context = image.createGraphics();
            context.setColor(Color.WHITE);
            context.fill(new Rectangle2D.Double(0, 0, image.getWidth(), image.getHeight()));
            
            // Create the printer that prints the graph
            final GraphPrinter graphPrinter = new GraphPrinter(image);
            
            // Print the graph
            final GraphPresenter graphPresenter = new GraphPresenter(image, graphPrinter);
            graphPresenter.drawGraph(parseResult.getPoints(), parseResult.getClusters());
            
            // Save the image
            ImageIO.write(image, "png", parsedArguments.getOutputPath().toFile());
        } catch (final MalformedFileException | IOException | ClassNotFoundException |
                       InstantiationException | IllegalAccessException |
                       UnsupportedLookAndFeelException exception) {
            // Report any error
            System.err.println(exception.getMessage());
            // noinspection CallToSystemExit
            System.exit(1);
        }
    }
    
    /**
     * Utility method that prints the usage of the program.
     */
    @SuppressWarnings("UseOfSystemOutOrSystemErr")
    private static void usage() {
        
        // noinspection LongLine
        System.out.println("""
                           SYNOPSYS
                               graph-printer -h
                               graph-printer INPUT_FILE_PATH OUTPUT_FILE_PATH IMAGE_WIDTH IMAGE_HEIGHT
                               
                           DESCRIPTION
                              Draws the graph reporting points and clusters specified in the INPUT_FILE_PATH
                              file. The graph is drawn onto an image of size IMAGE_WIDTH x IMAGE_HEIGHT and
                              then saved in the file OUTPUT_FILE_PATH.
                              The INPUT_FILE_PATH and OUTPUT_FILE_PATH can be either absolute or relative.
                            
                           OPTIONS
                               -h, --help    Prints this help.
                               
                           EXIT CODES
                               0    If the program successfully drawn the graph and saved it to the specified
                                    file.
                                    
                               1    If an error occurred while printing the graph.
                           """);
    }
}
