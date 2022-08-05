package it.debsite.dcv.main;

import lombok.AllArgsConstructor;
import lombok.Getter;

import java.nio.file.Files;
import java.nio.file.Path;

/**
 * Utility class allowing to parse the command line arguments.
 *
 * @author DeB
 * @version 1.0 2022-08-04
 * @since 1.0
 */
final class CliArgumentsParser {
    
    /**
     * A utility interface that wraps the function that prints the usage of the program.
     */
    @FunctionalInterface
    interface UsageFunction {
        
        /**
         * Prints the usage of the program.
         */
        void usage();
    }
    
    /**
     * Container containing the parsed arguments provided via the command line.
     *
     * @author DeB
     * @version 1.0 2022-08-04
     * @since 1.0
     */
    @AllArgsConstructor
    @Getter
    static class CliArguments {
        
        /**
         * Path of the file to read and parse to extract the information about the points and the
         * clusters.
         */
        private final Path inputPath;
        
        /**
         * Path of the image file where the graph or dendrogram will be printed.
         */
        private final Path outputPath;
        
        /**
         * Width of the image.
         */
        private int imageWidth;
        
        /**
         * Height of the image.
         */
        private int imageHeight;
    }
    
    /**
     * The constructor is private so to avoid creating objects of this class, which offers only
     * static methods.
     */
    private CliArgumentsParser() {
    
    }
    
    /**
     * Parse the arguments provided via the command line. If an error occur while parsing the
     * arguments, the program is terminated exiting with code 1.
     *
     * @param args Arguments provided via the command line.
     * @param usageFunction Function that prints the usage of the program.
     * @return The parsed arguments.
     */
    @SuppressWarnings({"UseOfSystemOutOrSystemErr", "CallToSystemExit"})
    static CliArguments parseCommandLineArguments(
        final String[] args, final UsageFunction usageFunction
    ) {
        
        // Check if the help option has been provided
        // noinspection CallToSuspiciousStringMethod
        if ((args.length == 1) && ("-h".equals(args[0]) || "--help".equals(args[0]))) {
            usageFunction.usage();
            System.exit(0);
        }
        
        // Check the command line arguments
        if (args.length != 4) {
            System.err.println("This program requires 4 arguments.");
            usageFunction.usage();
            System.exit(1);
        }
        // Extract the path to the input file
        final String inputFilePath = args[0];
        // Check its validity
        final Path inputPath = Path.of(inputFilePath).toAbsolutePath().normalize();
        if (!Files.isRegularFile(inputPath)) {
            // The input file does not exist
            System.err.printf("The input file '%s' does not exist.%n", inputPath);
            usageFunction.usage();
            System.exit(1);
        }
        // Extract the path of the output file
        final String outputFilePath = args[1];
        // Check its validity
        final Path outputPath = Path.of(outputFilePath).toAbsolutePath().normalize();
        if (!Files.isDirectory(outputPath.getParent())) {
            // The directory where the output file should be created does not exist
            System.err.printf(
                "The output file '%s' cannot be created into a non-existent directory '%s'.%n",
                outputPath.getFileName(),
                outputPath
            );
            usageFunction.usage();
            System.exit(1);
        }
        
        // Extract the size of the image
        try {
            // Extract the image width
            final String imageWidthString = args[2];
            final int imageWidth = Integer.parseInt(imageWidthString);
            // Check its validity
            if (imageWidth <= 0) {
                System.err.printf("The specified image width '%d' is non-positive.%n", imageWidth);
                usageFunction.usage();
                System.exit(1);
            }
            // Extract the image height
            final String imageHeightString = args[3];
            final int imageHeight = Integer.parseInt(imageHeightString);
            // Check its validity
            if (imageHeight <= 0) {
                System.err.printf(
                    "The specified image height '%d' is non-positive.%n",
                    imageHeight
                );
                usageFunction.usage();
                System.exit(1);
            }
            
            return new CliArguments(inputPath, outputPath, imageWidth, imageHeight);
        } catch (final NumberFormatException exception) {
            // An error occurred while parsing the size
            System.err.println(exception.getMessage());
        }
        usageFunction.usage();
        System.exit(1);
        throw new IllegalStateException("This cannot happen");
    }
}
