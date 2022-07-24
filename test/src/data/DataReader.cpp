/*
 * DataReader implementation.
 *
 * @author DeB, Jonathan
 * @version 1.1 2022-07-18
 * @since 1.0
 */
#include "DataReader.h"
#include "MalformedFileException.h"
#include <fstream>

using namespace std::literals::string_literals;

/**
 * Reads the data from the file.
 *
 * @param startColumnIndex Index of the first column to parse of each line in the data file.
 * @param endColumnIndex Index of the last column to parse of each line in the data file.
 * @param fileName Name of the file.
 * @return The parsed data.
 */
std::vector<double *> DataReader::readData(const std::size_t startColumnIndex,
                                           const std::size_t endColumnIndex,
                                           const std::filesystem::path &inputPath) {

    // Clear the result
    std::vector<double *> data{};

    // Check the dimension
    const std::size_t dimension = endColumnIndex - startColumnIndex + 1;
    if (dimension < 2) {
        throw MalformedFileException("The data file must list points at least bi-dimensional.");
    }

    std::filesystem::path inputAbsolutePath = absolute(inputPath);
    if (!exists(inputAbsolutePath)) {
        using namespace std::literals::string_literals;
        throw MalformedFileException(
                "The file '"s + inputAbsolutePath.string() + "' does not exist.");
    }

    // Open the input file
    std::ifstream file{inputAbsolutePath};
    if (file) {
        // Number of the line being parsed
        std::size_t lineNumber = 1;
        // Read all the content of the file
        while (file) {
            // Read the next line of the file
            std::string line{};
            std::getline(file, line);
            if (file.good() || file.eof()) {
                // Exclude empty lines
                if (!line.empty()) {
                    // Create the point
                    auto *const point = new double[dimension];

                    // Parse the line
                    std::size_t start = 0;
                    std::size_t length = 0;
                    std::size_t elementIndex = 0;
                    std::size_t columnIndex = 0;

                    // Loop over the characters of the line
                    std::string::const_iterator iterator = line.cbegin();
                    while (iterator != line.end() && columnIndex <= endColumnIndex) {
                        // Split the columns when a comma is found
                        if (*iterator == ',') {
                            // Include the column only if in the range
                            if (columnIndex >= startColumnIndex) {
                                // Extract the column substring
                                std::string columnString{line.substr(start, length)};
                                // Parse its value
                                std::size_t nextCharacterToParseIndex = 0;
                                double coordinate =
                                        std::stod(columnString, &nextCharacterToParseIndex);

                                // Check the validity
                                if (nextCharacterToParseIndex == 0) {
                                    throw MalformedFileException(
                                            "Line "s + std::to_string(lineNumber) +
                                            "has invalid column " + std::to_string(columnIndex));
                                }

                                // Add the coordinate
                                if (elementIndex < dimension) {
                                    point[elementIndex] = coordinate;
                                }
                                // Pass to the next coordinate
                                elementIndex++;
                            }
                            // Start a new column
                            start += length + 1;
                            length = 0;
                            columnIndex++;
                        } else {
                            length++;
                        }
                        ++iterator;
                    }
                    // Add the last column, if not empty
                    if (start < line.length()) {
                        if (elementIndex < dimension) {
                            point[elementIndex] =
                                    std::strtod(line.substr(start, line.length()).c_str(), nullptr);
                        }
                        elementIndex++;
                    }

                    // Check if all the dimensions has been filled
                    if (elementIndex < dimension) {
                        throw MalformedFileException("Line "s + std::to_string(lineNumber) +
                                                     " lists " + std::to_string(elementIndex) +
                                                     "coordinates, but " +
                                                     std::to_string(dimension) + " are required");
                    }

                    // Add the point to the data
                    data.emplace_back(point);
                    // Pass to the next line
                    lineNumber++;
                }
            } else {
                throw MalformedFileException("Error while reading the file");
            }
        }
    } else {
        throw MalformedFileException("Error while reading the file");
    }

    // Return the parsed data
    return data;
}
