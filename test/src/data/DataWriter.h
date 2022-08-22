#ifndef FINAL_PROJECT_HPC_DATAWRITER_H
#define FINAL_PROJECT_HPC_DATAWRITER_H

#include <filesystem>
#include <vector>

namespace cluster::test::data {

/**
 * Writer that writes some information to files.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2 2022-08-22
 * @since 1.0
 */
class DataWriter {

public:
    /**
     * Outputs on the specified file the information about the points and the clusters.
     *
     * @param outputFilePath Path of the file where the information will be written to.
     * @param pointCoordinates Vector containing the coordinates of the points. The coordinates are
     * stored linearly in this vector.<br>
     * Therefore, if the file contains 3 points <code>(x1, y1, z1)</code>, <code>(x2, y2, z2)</code>
     * and  <code>(x3, y3, z3)</code>, this vector will contain the following values: <pre> [x1, y1,
     * z1, x2, y2, z2, x3, y3, z3]
     * </pre>
     * @param dimension Dimension of each point.
     * @param piVector Vector containing the values of <code>pi</code>.
     * @param lambdaVector Vector containing the values of <code>lambda</code>.
     * @throws IOException If an I/O error occurs while writing to the file.
     * @throws std::invalid_argument If the specified <code>pointCoordinates</code> vector contains
     * too few coordinates.
     */
    static void createVisualizerOutputFile(const std::filesystem::path &outputFilePath,
                                           const std::vector<double> &pointCoordinates,
                                           const std::size_t &dimension,
                                           const std::vector<std::size_t> &piVector,
                                           const std::vector<double> &lambdaVector);

    /**
     * Outputs a file containing code to visualize a dendrogram in Mathematica, given pi and lambda.
     *
     * @param first Path of the file where the output will be written.
     * @param second Vector containing the values of <code>pi</code>.
     * @param lambdaVector Vector containing the values of <code>lambda</code>.
     * @throws IOException If an I/O error occurs while writing to the file.
     */
    static void createMathematicaOutputFile(const std::filesystem::path &first,
                                            const std::vector<std::size_t> &second,
                                            const std::vector<double> &lambdaVector);

    /**
     * Writes to the specified file the values of pi and lambda, so to retrieve them instead of
     * re-executing the clustering algorithm.
     *
     * @param outputFilePath Path of the file where the values of pi and lambda will be written to.
     * @param piVector Vector containing the values of pi.
     * @param lambdaVector Vector containing the values of lambda.
     * @throws IOException If an I/O error occurs while writing to the file.
     */
    static void writePiLambda(const std::filesystem::path &outputFilePath,
                              const std::vector<std::size_t> &piVector,
                              const std::vector<double> &lambdaVector);

private:
    /**
     * Requires the specified file stream to be in a good state.
     *
     * @param fileStream File stream to check the state of.
     * @param outputFilePath Path of the output file the specified <code>fileStream</code> writes
     * to.
     * @throws IOException If the file stream is not in a good state.
     */
    static void requireFileGoodness(const std::ofstream &fileStream,
                                    const std::filesystem::path &outputFilePath);
};
}  // namespace cluster::test::data
#endif  // FINAL_PROJECT_HPC_DATAWRITER_H
