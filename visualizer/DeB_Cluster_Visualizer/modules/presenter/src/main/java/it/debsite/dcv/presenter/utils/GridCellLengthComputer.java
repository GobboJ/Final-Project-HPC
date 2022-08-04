package it.debsite.dcv.presenter.utils;

/**
 * Utility class allowing to compute the length of the grid cells in the {@code x1-x2} plane along
 * one of the two directions ({@code x1} or {@code x2}), according to the predefined lengths.
 *
 * @author DeB
 * @version 1.1 2022-08-03
 * @since 1.0
 */
public final class GridCellLengthComputer {
    
    /**
     * Factor used to move from one order of magnitudes to another.
     */
    private static final double ORDER_OF_MAGNITUDE_FACTOR = 10;
    
    /**
     * Predefined cell lengths.
     */
    private static final double[] PREDEFINED_LENGTHS = new double[]{1, 2, 4, 5, 10};
    
    /**
     * The constructor is private so to avoid creating objects of this class, which offers only
     * static methods.
     */
    private GridCellLengthComputer() {
    
    }
    
    /**
     * Computes the appropriate length, with the appropriate order of magnitude, that best
     * approximates the specified grid cell in the {@code x1-x2} plane along one of the two
     * directions ({@code x1} or {@code x2}).<br> Examples:
     * <table summary="Examples">
     *      <thead>
     *          <tr><td>{@code gridCellLength}</td><td>predefined cell size</td></tr>
     *      </thead>
     *      <tbody>
     *      <tr><td>{@code 2.3}</td><td>{@code 4}</td></tr>
     *      <tr><td>{@code 4.5}</td><td>{@code 5}</td></tr>
     *      <tr><td>{@code 15}</td><td>{@code 20}</td></tr>
     *      <tr><td>{@code 350}</td><td>{@code 400}</td></tr>
     *      <tr><td>{@code 0.9}</td><td>{@code 0.5}</td></tr>
     *      <tr><td>{@code 0.15}</td><td>{@code 0.1}</td></tr>
     *      </tbody>
     *  </table>
     *
     * @param gridCellLength Length of the grid cell in the {@code x1-x2} plane.
     * @return The computed length.
     */
    public static double computeCellLength(final double gridCellLength) {
        
        if (gridCellLength > 1) {
            return GridCellLengthComputer.computeBiggerLength(gridCellLength);
        }
        
        return GridCellLengthComputer.computeSmallerLength(gridCellLength);
    }
    
    /**
     * Computes the first of the predefined lengths, with the appropriate order of magnitude, that
     * exceeds the specified grid cell length.<br>
     * <p>
     * Examples:
     * <table summary="Examples">
     *     <thead>
     *         <tr><td>{@code gridCellLength}</td><td>predefined cell size</td></tr>
     *     </thead>
     *     <tbody>
     *     <tr><td>{@code 2.3}</td><td>{@code 4}</td></tr>
     *     <tr><td>{@code 4.5}</td><td>{@code 5}</td></tr>
     *     <tr><td>{@code 7.9}</td><td>{@code 10}</td></tr>
     *     <tr><td>{@code 15}</td><td>{@code 20}</td></tr>
     *     <tr><td>{@code 350}</td><td>{@code 400}</td></tr>
     *     <tr><td>{@code 5000}</td><td>{@code 5000}</td></tr>
     *     </tbody>
     * </table>
     *
     * @param gridCellLength Length of the grid cell in the {@code x1-x2} plane.
     * @return The computed length.
     */
    private static double computeBiggerLength(
        double gridCellLength
    ) {
        
        // Compute the order of magnitude
        int orderOfMagnitude = 0;
        while (gridCellLength > GridCellLengthComputer.ORDER_OF_MAGNITUDE_FACTOR) {
            gridCellLength /= GridCellLengthComputer.ORDER_OF_MAGNITUDE_FACTOR;
            orderOfMagnitude++;
        }
        // Compute the first predefined length that exceeds the grid cell length stripped of the
        // order of magnitude
        int predefinedLengthIndex = 0;
        while (GridCellLengthComputer.PREDEFINED_LENGTHS[predefinedLengthIndex] < gridCellLength) {
            predefinedLengthIndex++;
        }
        
        // Compute the final value
        return GridCellLengthComputer.PREDEFINED_LENGTHS[predefinedLengthIndex] * StrictMath.pow(
            GridCellLengthComputer.ORDER_OF_MAGNITUDE_FACTOR,
            orderOfMagnitude
        );
    }
    
    /**
     * Computes the first of the predefined lengths, with the appropriate order of magnitude, that
     * is inferior to the specified grid cell length.<br>
     * <p>
     * Examples:
     * <table summary="Examples">
     *     <thead>
     *         <tr><td>{@code gridCellLength}</td><td>predefined cell size</td></tr>
     *     </thead>
     *     <tbody>
     *     <tr><td>{@code 0.9}</td><td>{@code 0.5}</td></tr>
     *     <tr><td>{@code 0.15}</td><td>{@code 0.1}</td></tr>
     *     <tr><td>{@code 0.07}</td><td>{@code 0.05}</td></tr>
     *     <tr><td>{@code 0.001}</td><td>{@code 0.001}</td></tr>
     *     </tbody>
     * </table>
     *
     * @param gridCellLength Length of the grid cell in the {@code x1-x2} plane.
     * @return The computed length.
     */
    private static double computeSmallerLength(
        double gridCellLength
    ) {
        
        // Compute the order of magnitude
        int orderOfMagnitude = 0;
        while (gridCellLength < 1) {
            gridCellLength *= GridCellLengthComputer.ORDER_OF_MAGNITUDE_FACTOR;
            orderOfMagnitude++;
        }
        // Compute the first predefined length that exceeds the grid cell length stripped of the
        // order of magnitude
        int predefinedLengthIndex = 0;
        while (gridCellLength > GridCellLengthComputer.PREDEFINED_LENGTHS[predefinedLengthIndex]) {
            predefinedLengthIndex++;
        }
        
        // Compute the final value
        return GridCellLengthComputer.PREDEFINED_LENGTHS[predefinedLengthIndex] * (1 /
                                                                                       StrictMath.pow(
                                                                                           GridCellLengthComputer.ORDER_OF_MAGNITUDE_FACTOR,
                                                                                           orderOfMagnitude
                                                                                       ));
        
    }
    
}
