/*
 * DataWriter implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-08-06
 * @since 1.0
 */
#include "DataWriter.h"
#include "IoException.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace cluster::test::data {

/**
 * Outputs on the specified file the information about the points and the clusters.
 *
 * @param outputFilePath Path of the file where the information will be written to.
 * @param pointCoordinates Vector containing the coordinates of the points.
 * @param dimension Dimension of each point.
 * @param piVector Vector containing the values of pi.
 * @param lambdaVector Vector containing the values of lambda.
 * @throws IOException If an I/O error occurs while writing to the file.
 */
void DataWriter::createOutputFile(const std::filesystem::path &outputFilePath,
                                  const std::vector<double> &pointCoordinates,
                                  const std::size_t &dimension,
                                  const std::vector<size_t> &piVector,
                                  const std::vector<double> &lambdaVector) {

    using namespace std::literals::string_literals;

    // Open the output file
    std::ofstream fileOutputStream{outputFilePath};
    if (fileOutputStream) {

        // Map containing the association between the identifiers of the points and the identifier
        // of the last printed cluster they belong to.
        // Initially, every point is associated to itself.
        std::unordered_map<std::string, std::string> names{};

        // Number identifying the point
        std::size_t pointIdentifier = 0;

        // Print the points
        for (std::size_t i = 0; i < pointCoordinates.size(); i += dimension) {
            // Print the name
            const std::string pointName{"P"s + std::to_string(pointIdentifier)};
            fileOutputStream << pointName << ": \"" << std::to_string(pointIdentifier + 1) << "\" ";
            // Print the coordinates
            for (std::size_t j = 0; j < dimension; j++) {
                // Write the coordinate
                fileOutputStream << pointCoordinates[i * dimension + j];
                DataWriter::requireFileGoodness(fileOutputStream);
                // Write a comma to separate the coordinates, if it is not the last one
                if (j == (dimension - 1)) {
                    fileOutputStream << std::endl;
                } else {
                    fileOutputStream << ' ';
                }
                DataWriter::requireFileGoodness(fileOutputStream);
            }
            // Map the point with itself
            names[pointName] = std::to_string(pointIdentifier + 1);
            // Increase the identifier of the points
            pointIdentifier++;
        }

        // Create pi and lambda according to the values of lambda, in increasing order.
        // Instead of sorting the two vectors, we create a vector of indexes, and we sort it using
        // the values of lambda, i.e., if result[i] < result[j] for some i and j, then lambda[i] <
        // lambda[j].
        std::vector<std::size_t> result{};
        result.resize(piVector.size());
        for (std::size_t i = 0; i < piVector.size(); i++) {
            result[i] = i;
        }
        // Order the vector of indexes by lambda
        std::sort(result.begin(),
                  result.end(),
                  [&lambdaVector](const std::size_t &first, const std::size_t &second) -> bool {
                      return lambdaVector[first] < lambdaVector[second];
                  });

        // Map associating, for each point, the identifier of the last printed cluster it belongs to
        // and the name of that cluster. Initially, every point is associated to itself
        std::unordered_map<std::size_t, std::pair<std::string, std::string>> clusters{};

        // Initialize the map
        for (std::size_t i = 0; i < piVector.size(); i++) {
            std::string firstPointName = "P" + std::to_string(i);
            clusters[i] = {firstPointName, names[firstPointName]};
        }

        // Identifier of the cluster
        std::size_t clusterIdentifier = 0;

        for (const auto index : result) {
            if (index != piVector[index]) {
                // Compose the identifier of the cluster
                const std::string clusterName{"C"s + std::to_string(clusterIdentifier)};
                // Extract the two sub-clusters
                const auto &[firstClusterIndex, firstName] = clusters[index];
                const auto &[secondClusterIndex, secondName] = clusters[piVector[index]];

                // Compose the name of the cluster
                std::string fullClusterName = firstName;
                fullClusterName += secondName;
                // Truncate it at 10 characters
                if (fullClusterName.length() > 10) {
                    fullClusterName = fullClusterName.substr(0, 7) + "...";
                }

                // Output the row
                fileOutputStream << clusterName << ": \"" << fullClusterName << "\" "
                                 << firstClusterIndex << ' ' << secondClusterIndex << ' '
                                 << lambdaVector[index] << '\n';
                DataWriter::requireFileGoodness(fileOutputStream);

                // Update the last printed cluster the points belong to
                clusters[index] = {clusterName, fullClusterName};
                clusters[piVector[index]] = {clusterName, fullClusterName};

                // Increase the identifier
                clusterIdentifier++;
            }
        }
    } else {
        throw IOException(
                "Unable to open output file where to store the information about the points and "
                "the clusters.");
    }
}

/**
 * Outputs a file containing code to visualize a dendrogram in Mathematica, given pi and lambda.
 *
 * @param outputFilePath Path of the file where the output will be written.
 * @param piVector Vector containing the values of <code>pi</code>.
 * @param lambdaVector Vector containing the values of <code>lambda</code>.
 * @throws IOException If an I/O error occurs while writing to the file.
 */
void DataWriter::createMathematicaOutputFile(const std::filesystem::path &outputFilePath,
                                             const std::vector<size_t> &piVector,
                                             const std::vector<double> &lambdaVector) {

    using namespace std::literals::string_literals;

    // Open the output file
    std::ofstream fileOutputStream{outputFilePath};
    if (fileOutputStream) {

        // Vector containing the triple (i, pi[i], lambda[i])
        std::vector<std::tuple<std::size_t, std::size_t, double>> ordered{};
        for (std::size_t i = 0; i < lambdaVector.size(); i++) {
            ordered.emplace_back(std::make_tuple(i, piVector[i], lambdaVector[i]));
        }
        // Sort the vector of triples by lambda
        std::sort(
                ordered.begin(), ordered.end(), [](const auto &first, const auto &second) -> bool {
                    return std::get<2>(first) < std::get<2>(second);
                });

        // Map associating the index of the point to the name of the variable identifying the
        // cluster and the number of elements in the cluster
        std::unordered_map<std::size_t, std::pair<std::string, std::size_t>> map{};
        for (std::size_t i = 0; i < ordered.size(); i++) {
            map[i] = {std::to_string(i + 1), 1};
        }

        // Print the heading
        fileOutputStream << "Needs[\"HierarchicalClustering`\"]\n";
        DataWriter::requireFileGoodness(fileOutputStream);

        for (std::size_t i = 0; i < ordered.size() - 1; i++) {
            // Extract a triple (i, pi[i], lambda[i])
            const auto &[index, piValue, lambdaValue] = ordered[i];
            // Output the definition of the cluster
            fileOutputStream << "c" << i << " = Cluster[" << map[index].first << ", "
                             << map[piValue].first << ", " << lambdaValue << ", "
                             << map[index].second << ", " << map[index].second << "]\n";
            DataWriter::requireFileGoodness(fileOutputStream);

            // Update the name of the cluster the two points belongs to, together with the number of
            // points in the cluster
            map[piValue].first = "c"s + std::to_string(i);
            map[piValue].second += map[index].second;
            map[index].first = "c"s + std::to_string(i);
            map[index].second += map[piValue].second;
        }
        // Print the command to generate the dendrogram
        fileOutputStream << "DendrogramPlot[c" << ordered.size() - 2 << ", LeafLabels ->(#&)]\n";
        DataWriter::requireFileGoodness(fileOutputStream);
    } else {
        throw IOException("Unable to open output file.");
    }
}

/**
 * Writes to the specified file the values of pi and lambda.
 *
 * @param outputFilePath Path of the file where the values of pi and lambda will be written to.
 * @param piVector Vector containing the values of pi.
 * @param lambdaVector Vector containing the values of lambda.
 * @throws IOException If an I/O error occurs while writing to the file.
 */
void DataWriter::writePiLambda(const std::filesystem::path &outputFilePath,
                               const std::vector<std::size_t> &piVector,
                               const std::vector<double> &lambdaVector) {

    // Open the file
    std::ofstream fileOutputStream{outputFilePath};
    if (fileOutputStream) {
        // Print all the pi values
        const std::size_t lastPiValue = piVector.size() - 1;
        for (std::size_t i = 0; i < piVector.size(); i++) {
            // Print the value
            fileOutputStream << piVector[i];
            DataWriter::requireFileGoodness(fileOutputStream);
            // Print a comma to separate the value from the following one, if it is not the last one
            if (i != lastPiValue) {
                fileOutputStream << ",";
                DataWriter::requireFileGoodness(fileOutputStream);
            }
        }
        // Move to the next line
        fileOutputStream << std::endl;
        DataWriter::requireFileGoodness(fileOutputStream);

        // Print all the lambda values
        const std::size_t lastLambdaValue = lambdaVector.size() - 1;
        for (std::size_t i = 0; i < piVector.size(); i++) {
            // Print the value
            fileOutputStream << lambdaVector[i];
            DataWriter::requireFileGoodness(fileOutputStream);
            // Print a comma to separate the value from the following one, if it is not the last one
            if (i != lastLambdaValue) {
                fileOutputStream << ",";
                DataWriter::requireFileGoodness(fileOutputStream);
            }
        }
    } else {
        throw IOException("Unable to open output file where the test results will be stored.");
    }
}

/**
 * Requires the specified file stream to be in a good state.
 *
 * @param fileStream File stream to check the state of.
 * @throws IOException If the file stream is not in a good state.
 */
void DataWriter::requireFileGoodness(const std::ofstream &fileStream) {

    if (!fileStream.good()) {
        throw IOException("Error while writing into the file");
    }
}
}  // namespace cluster::test::data
