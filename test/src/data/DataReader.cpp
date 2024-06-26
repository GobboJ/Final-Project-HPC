/*
 * DataReader implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2.1 2022-08-22
 * @since 1.0
 */
#include "DataReader.h"
#include "IoException.h"
#include "MalformedFileException.h"
#include <fstream>

namespace cluster::test::data {
/**
 * Reads and parses the data from the specified file.
 *
 * @param inputFilePath Path of the file that contains the data to parse.
 * @param firstLineNumber Number of the first line to parse of the file.
 * @param lastLineNumber Number of the last line to parse of the file.
 * @param firstColumnNumber Number of the first column of each line to parse as a coordinate of
 * the point.
 * @param lastColumnNumber Number of the last column of each line to parse as a coordinate of
 * the point.
 * @return The read data.
 * @throws MalformedFileException If the file does not follow the correct format.
 */
std::size_t DataReader::readAndParseData(const std::filesystem::path &inputFilePath,
                                         std::vector<double> &parsedData,
                                         std::size_t firstLineNumber,
                                         std::size_t lastLineNumber,
                                         std::size_t firstColumnNumber,
                                         std::size_t lastColumnNumber) {

    // Clear the result
    parsedData.clear();

    bool firstLine = true;
    std::size_t dimension = 0;

    // Check the arguments
    if (firstColumnNumber != 0 && lastColumnNumber != 0 && lastColumnNumber < firstColumnNumber) {
        throw std::invalid_argument("Invalid first and last column numbers.");
    }

    // Check the existence of the file
    if (!exists(inputFilePath)) {
        using namespace std::literals::string_literals;
        throw std::invalid_argument("The file '"s + inputFilePath.string() + "' does not exist.");
    }

    // Open the input file
    std::ifstream file{inputFilePath};
    if (file) {
        // Number of the line being parsed
        std::size_t lineNumber = 1;
        // Read all the content of the file
        while (file) {
            // Read the next line of the file
            std::string line{};
            std::getline(file, line);
            if (file.good() || file.eof()) {
                // Skip lines outside the range and empty lines
                if (!line.empty() &&
                    (firstLineNumber == 0 || lastLineNumber == 0 ||
                     (lineNumber >= firstLineNumber && lineNumber <= lastLineNumber))) {

                    // Parse the line
                    std::stringstream lineValuesStream{line};
                    std::string columnString{};
                    std::size_t columnNumber = 1;
                    std::size_t lineDimension = 0;
                    while (std::getline(lineValuesStream, columnString, ',') &&
                           (lastColumnNumber == 0 || columnNumber <= lastColumnNumber)) {
                        if (firstColumnNumber == 0 || columnNumber >= firstColumnNumber) {
                            // Add the parsed coordinate
                            parsedData.push_back(
                                    parseDouble(columnString, lineNumber, columnNumber));
                            lineDimension++;
                        }
                        columnNumber++;
                    }
                    // Check if all the dimensions has been filled
                    if (!firstLine) {
                        if (lineDimension != dimension) {
                            using namespace std::literals::string_literals;
                            throw MalformedFileException(
                                    "Line"s + ' ' + std::to_string(lineNumber) + " contains " +
                                    std::to_string(lineDimension) +
                                    " coordinates, but the first line contains" + ' ' +
                                    std::to_string(dimension) + " coordinates");
                        }
                    } else {
                        firstLine = false;
                        dimension = lineDimension;
                    }

                    // Pass to the next line
                    lineNumber++;
                }
            } else {
                using namespace std::literals::string_literals;
                throw IOException("Error while reading the file"s + ' ' + inputFilePath.string());
            }
        }
    } else {
        using namespace std::literals::string_literals;
        throw IOException("Error while reading the file"s + ' ' + inputFilePath.string());
    }

    // Return the dimension
    return dimension;
}

/**
 * Reads from the specified file the values of pi and lambda to use in the tests.
 *
 * @param inputFilePath Path to read the information from.
 * @param piVector Vector where the read values of pi will be put.
 * @param lambdaVector Vector where the read values of lambda will be put.
 * @throws MalformedFileException If the file does not follow the correct format.
 */
void DataReader::readPiLambda(const std::filesystem::path &inputFilePath,
                              std::vector<std::size_t> &piVector,
                              std::vector<double> &lambdaVector) {

    // Clear the result
    piVector.clear();
    lambdaVector.clear();

    // Check the existence of the file
    if (!exists(inputFilePath)) {
        using namespace std::literals::string_literals;
        throw std::invalid_argument("The file '"s + inputFilePath.string() + "' does not exist.");
    }

    // Open the input file
    std::ifstream file{inputFilePath};
    if (file) {
        // Read the first line of the file
        std::string line{};
        std::getline(file, line);
        if (file.good() || file.eof()) {
            // Parse the first line
            std::stringstream firstLineValueStream{line};
            std::string value;
            std::size_t columnNumber = 1;
            // Parse the first line
            columnNumber = 1;
            while (std::getline(firstLineValueStream, value, ',')) {
                piVector.push_back(parseSizeT(value, 1, columnNumber));
                columnNumber++;
            }

            // Read the second line of the file
            std::getline(file, line);
            if (file.good() || file.eof()) {
                std::stringstream secondLineValueStream{line};
                // Parse the second line
                columnNumber = 1;
                while (std::getline(secondLineValueStream, value, ',')) {
                    lambdaVector.push_back(parseDouble(value, 2, columnNumber));
                    columnNumber++;
                }
            } else {
                using namespace std::literals::string_literals;
                throw IOException("Error while reading the file"s + ' ' + inputFilePath.string());
            }
        } else {
            using namespace std::literals::string_literals;
            throw IOException("Error while reading the file"s + ' ' + inputFilePath.string());
        }
    } else {
        using namespace std::literals::string_literals;
        throw IOException("Error while reading the file"s + ' ' + inputFilePath.string());
    }
}

/**
 * Extracts a <code>double</code> value from the specified string.
 *
 * @param string String where to extract the value.
 * @param lineNumber Number of the line that contains the string to parse.
 * @param columnNumber Number of the column that contains the string to parse.
 * @return The extracted value.
 * @throws MalformedFileException If some errors occur while parsing the string.
 */
double DataReader::parseDouble(
        const std::string &string, const std::size_t lineNumber, const std::size_t columnNumber) {

    // Index of the first non parsed character
    std::size_t firstNonParsedCharacter = 0;

    // Parsed value
    // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
    double value;
    try {
        // Parse the value
        value = std::stod(string, &firstNonParsedCharacter);
    } catch (std::exception &exception) {
        using namespace std::literals::string_literals;
        throw MalformedFileException("Line"s + ' ' + std::to_string(lineNumber) +
                                     " contains the invalid value '" + string + "' at column " +
                                     std::to_string(columnNumber) + ": " + exception.what());
    }

    // Check the validity
    if (firstNonParsedCharacter == 0) {
        using namespace std::literals::string_literals;
        throw MalformedFileException("Line"s + ' ' + std::to_string(lineNumber) +
                                     " contains the invalid value '" + string + "' at column " +
                                     std::to_string(columnNumber));
    }

    return value;
}

/**
 * Extracts a <code>std::size_t</code> value from the specified string.
 *
 * @param string String where to extract the value.
 * @param lineNumber Number of the line that contains the string to parse.
 * @param columnNumber Number of the column that contains the string to parse.
 * @return The extracted value.
 * @throws MalformedFileException If some errors occur while parsing the string.
 */
std::size_t DataReader::parseSizeT(
        const std::string &string, const std::size_t lineNumber, const std::size_t columnNumber) {

    // Index of the first non parsed character
    std::size_t firstNonParsedCharacter = 0;

    // Parsed value
    // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
    std::size_t value;
    try {
        // Parse the value
        value = std::stoull(string, &firstNonParsedCharacter);
    } catch (std::exception &exception) {
        using namespace std::literals::string_literals;
        throw MalformedFileException("Line"s + ' ' + std::to_string(lineNumber) +
                                     " contains the invalid value '" + string + "' at column " +
                                     std::to_string(columnNumber) + ": " + exception.what());
    }

    // Check the validity
    if (firstNonParsedCharacter == 0) {
        using namespace std::literals::string_literals;
        throw MalformedFileException("Line"s + ' ' + std::to_string(lineNumber) +
                                     " contains the invalid value '" + string + "' at column " +
                                     std::to_string(columnNumber));
    }

    return value;
}
}  // namespace cluster::test::data
