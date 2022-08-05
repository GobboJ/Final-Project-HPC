package it.debsite.dcv.main;

import it.debsite.dcv.main.CliArgumentsParser.CliArguments;
import it.debsite.dcv.model.ClusterFileReader;
import it.debsite.dcv.model.MalformedFileException;
import it.debsite.dcv.model.ParseResult;
import it.debsite.dcv.presenter.DendrogramPresenter;
import it.debsite.dcv.view.dendrogram.DendrogramPrinter;

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
 * Application that prints a dendrogram tree build starting from the information contained into a
 * file.
 *
 * @author DeB
 * @version 1.1 2022-08-04
 * @since 1.0
 */
public final class MainDendrogramPrinter {
    
    /**
     * The constructor is private so to avoid creating objects of this class, which offers only
     * static methods.
     */
    private MainDendrogramPrinter() {
    
    }
    
    /**
     * Main entrypoint of the application.
     *
     * @param args Arguments specified via the command line.
     */
    @SuppressWarnings("MethodCanBeVariableArityMethod")
    public static void main(final String[] args) {
        
        // Parse the command line arguments
        final CliArguments arguments =
            CliArgumentsParser.parseCommandLineArguments(args, MainDendrogramPrinter::usage);
        
        // Draw the dendrogram
        EventQueue.invokeLater(() -> MainDendrogramPrinter.start(arguments));
    }
    
    /**
     * Start function, called on the AWT thread, that prints the dendrogram onto the image.
     *
     * @param parsedArguments Arguments specified on the command line.
     */
    @SuppressWarnings({"UseOfSystemOutOrSystemErr", "FeatureEnvy"})
    private static void start(final CliArguments parsedArguments) {
        
        try {
            // Parse the data of the input file
            final ParseResult parseResult =
                ClusterFileReader.parseDataFromFile(parsedArguments.getInputPath());
            
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
            
            // Create the printer that prints teh dendrogram
            final DendrogramPrinter printer = new DendrogramPrinter(image);
            
            // Print the dendrogram
            final DendrogramPresenter presenter = new DendrogramPresenter(image, printer);
            presenter.printDendrogram(parseResult.getPoints(), parseResult.getClusters());
            
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
                               dendrogram-printer -h
                               dendrogram-printer INPUT_FILE_PATH OUTPUT_FILE_PATH IMAGE_WIDTH IMAGE_HEIGHT
                               
                           DESCRIPTION
                              Draws the dendrogram tree built from the points and clusters specified in the
                              INPUT_FILE_PATH file. The dendrogram is drawn onto an image of size
                              IMAGE_WIDTH x IMAGE_HEIGHT and then saved in the file OUTPUT_FILE_PATH.
                              The INPUT_FILE_PATH and OUTPUT_FILE_PATH can be either absolute or relative.
                              
                           OPTIONS
                               -h, --help    Prints this help.
                               
                           EXIT CODES
                               0    If the program successfully drawn the dendrogram and saved it to the
                                    specified file.
                                    
                               1    If an error occurred while printing the graph.
                           """);
    }
}
