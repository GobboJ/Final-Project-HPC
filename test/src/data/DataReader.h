#ifndef FINAL_PROJECT_HPC_DATAREADER_H
#define FINAL_PROJECT_HPC_DATAREADER_H

#include <filesystem>
#include <string>
#include <vector>

namespace cluster::test::data {
/**
 * Reader that reads the data from files.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2.1 2022-08-22
 * @since 1.0
 */
class DataReader {

public:
    /**
     * Reads and parses the data from the specified file.
     *
     * @param inputFilePath Path of the file that contains the data to parse.
     * @param parsedData Vector where the parsed data will be placed. The coordinates of the points
     * are stored in the vector one after the other. Moreover, also the points are stored one after
     * the other. Therefore, if the file contains 3 points <code>(x1, y1, z1)</code>, <code>(x2, y2,
     * z2)</code> and  <code>(x3, y3, z3)</code>, this vector will contain the following values:
     * <pre>
     * [x1, y1, z1, x2, y2, z2, x3, y3, z3]
     * </pre>
     * @param firstLineNumber Number of the first line to parse of the file. If not specified, or if
     * <code>0</code> is specified, then this method parses the lines starting from the first one in
     * the file.
     * @param lastLineNumber Number of the last line to parse of the file. If not specified, or if
     * <code>0</code> is specified, then this method parses all the lines from the specified
     * <code>firstLineNumber</code> to the last line of the file.
     * @param firstColumnNumber Number of the first column of each line to parse as a coordinate of
     * the point. If not specified, or if <code>0</code> is specified, then the first column of the
     * line is parsed as the first coordinate of the point.
     * @param lastColumnNumber Number of the last column of each line to parse as a coordinate of
     * the point. If not specified, or if <code>0</code> is specified, then the last column of the
     * line is parsed as the last coordinate of the point.
     * @return The dimension of each point.
     * @throws MalformedFileException If the file does not follow the correct format.
     * @throws IOException If an I/O error occurs while reading the file.
     * @throws std::invalid_argument If the specified <code>lastColumnNumber</code> is smaller than
     * the specified <code>firstColumnNumber</code>, or if the file with the specified path does not
     * exist.
     */
    static std::size_t readAndParseData(const std::filesystem::path &inputFilePath,
                                        std::vector<double> &parsedData,
                                        std::size_t firstLineNumber = 0,
                                        std::size_t lastLineNumber = 0,
                                        std::size_t firstColumnNumber = 0,
                                        std::size_t lastColumnNumber = 0);

    /**
     * Reads from the specified file the values of pi and lambda to use in the tests.
     *
     * @param inputFilePath Path to read the information from.
     * @param piVector Vector where the read values of pi will be put.
     * @param lambdaVector Vector where the read values of lambda will be put.
     * @throws MalformedFileException If the file does not follow the correct format.
     * @throws IOException If an I/O error occurs while reading the file.
     * @throws std::invalid_argument If the file with the specified path does not exist.
     */
    static void readPiLambda(const std::filesystem::path &inputFilePath,
                             std::vector<std::size_t> &piVector,
                             std::vector<double> &lambdaVector);

private:
    /**
     * Extracts a <code>double</code> value from the specified string.
     *
     * @param string String where to extract the value.
     * @param lineNumber Number of the line that contains the string to parse.
     * @param columnNumber Number of the column that contains the string to parse.
     * @return The extracted value.
     * @throws MalformedFileException If some errors occur while parsing the string.
     */
    static double parseDouble(
            const std::string &string, std::size_t lineNumber, std::size_t columnNumber);

    /**
     * Extracts a <code>std::size_t</code> value from the specified string.
     *
     * @param string String where to extract the value.
     * @param lineNumber Number of the line that contains the string to parse.
     * @param columnNumber Number of the column that contains the string to parse.
     * @return The extracted value.
     * @throws MalformedFileException If some errors occur while parsing the string.
     */
    static std::size_t parseSizeT(
            const std::string &string, std::size_t lineNumber, std::size_t columnNumber);
};
}  // namespace cluster::test::data
#endif  // FINAL_PROJECT_HPC_DATAREADER_H
