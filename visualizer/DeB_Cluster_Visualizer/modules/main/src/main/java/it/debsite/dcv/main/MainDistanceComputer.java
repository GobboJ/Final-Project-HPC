package it.debsite.dcv.main;

import org.jetbrains.annotations.NotNull;

import java.math.BigDecimal;
import java.math.MathContext;
import java.math.RoundingMode;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Scanner;

/**
 * Application that allows to compute the distance between two points.
 *
 * @author DeB
 * @version 1.1 2022-08-04
 * @since 1.0
 */
public final class MainDistanceComputer {
    
    /**
     * The constructor is private so to avoid creating objects of this class, which offers only
     * static methods.
     */
    private MainDistanceComputer() {
    
    }
    
    /**
     * Main entry point of the application.
     *
     * @param args Command line arguments, which are not used because the program is
     *     interactive.
     */
    @SuppressWarnings("UseOfSystemOutOrSystemErr")
    public static void main(final String... args) {
        
        // Initialize the reader from the console
        try (final Scanner consoleReader = new Scanner(System.in, StandardCharsets.UTF_8)) {
            System.out.print("Dimension of the points: ");
            System.out.flush();
            // Ask the dimension of the points
            final int dimension = consoleReader.nextInt();
            if (dimension < 1) {
                System.err.println("The dimension must be at least 1");
                // noinspection CallToSystemExit
                System.exit(1);
            }
            
            // Read the coordinates of the first point
            final BigDecimal @NotNull [] firstPointCoordinates =
                MainDistanceComputer.readPointCoordinates(consoleReader, dimension, "x");
            
            // Read the coordinates of the second point
            final BigDecimal @NotNull [] secondPointCoordinates =
                MainDistanceComputer.readPointCoordinates(consoleReader, dimension, "y");
            
            // Print the two points
            System.out.print("First point : ");
            System.out.println(Arrays.toString(firstPointCoordinates));
            System.out.print("Second point: ");
            System.out.println(Arrays.toString(secondPointCoordinates));
            
            // Compute the distance
            BigDecimal sum = new BigDecimal(0);
            for (int i = 0; i < dimension; i++) {
                final BigDecimal difference =
                    firstPointCoordinates[i].subtract(secondPointCoordinates[i]);
                final BigDecimal square = difference.multiply(difference);
                sum = sum.add(square);
            }
            // Print the distance
            System.out.print("Distance: ");
            System.out.println(sum.sqrt(new MathContext(3, RoundingMode.HALF_EVEN)));
        }
    }
    
    /**
     * Reads the coordinates of the point from the console.
     *
     * @param consoleReader Reader that reads the console.
     * @param dimension Dimension of the point.
     * @param pointName Name of the point.
     * @return The read coordinates.
     */
    @SuppressWarnings("UseOfSystemOutOrSystemErr")
    private static BigDecimal @NotNull [] readPointCoordinates(
        @NotNull final Scanner consoleReader, final int dimension, @NotNull final String pointName
    ) {
        
        // Read the coordinates of the first point
        final BigDecimal @NotNull [] pointCoordinates = new BigDecimal[dimension];
        
        for (int i = 0; i < dimension; i++) {
            System.out.printf("[%d/%d] %s%d: ", i + 1, dimension, pointName, i + 1);
            System.out.flush();
            pointCoordinates[i] = consoleReader.nextBigDecimal();
        }
        return pointCoordinates;
    }
}
