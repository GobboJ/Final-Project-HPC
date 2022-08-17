/*
 * CliArguments implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2 2022-08-06
 * @since 1.0
 */
#include "CliArguments.h"

namespace cluster::test::cli {

/**
 * Returns the number of the first column of each line of the input file to be considered containing
 * the coordinates of the point.
 *
 * @return The number of the first column to consider as coordinates of the point.
 */
std::size_t CliArguments::getFirstColumnNumber() const {

    return this->firstColumnIndex;
}

/**
 * Sets the number of the first column of each line of the input file to be considered containing
 * the coordinates of the point.
 *
 * @param firstColumnNumber Value of the number of the first column to consider as coordinates of
 * the point.
 */
void CliArguments::setFirstColumnNumber(size_t firstColumnNumber) {

    CliArguments::firstColumnIndex = firstColumnNumber;
}

/**
 * Returns the number of the last column of each line of the input file to be considered containing
 * the coordinates of the point.
 *
 * @return The number of the last column to consider as coordinates of the point.
 */
std::size_t CliArguments::getLastColumnNumber() const {

    return this->lastColumnIndex;
}

/**
 * Sets the number of the last column of each line of the input file to be considered containing the
 * coordinates of the point.
 *
 * @param lastColumnNumber Value of the number of the last column to consider as coordinates of
 * the point.
 */
void CliArguments::setLastColumnNumber(size_t lastColumnNumber) {

    CliArguments::lastColumnIndex = lastColumnNumber;
}

/**
 * Returns the number of the first line of the input file to be considered containing points to
 * cluster.
 *
 * @return The number of the first line of the input file to consider as a point to cluster.
 */
std::size_t CliArguments::getFirstLineNumber() const {

    return this->firstLineIndex;
}

/**
 * Sets the number of the first line of the input file to be considered containing points to
 * cluster.
 *
 * @param firstLineNumber Value of the number of the first line of the input file to consider as a
 * point to cluster.
 */
void CliArguments::setFirstLineNumber(const std::size_t firstLineNumber) {

    CliArguments::firstLineIndex = firstLineNumber;
}

/**
 * Returns the number of the last line of the input file to be considered containing points to
 * cluster.
 *
 * @return The index of the last line of the input file to consider as a point to cluster.
 */
std::size_t CliArguments::getLastLineNumber() const {

    return this->lastLineIndex;
}

/**
 * Sets the number of the last line of the input file to be considered containing points to
 * cluster.
 *
 * @param lastLineIndex Value of the number of the last line of the input file to consider as a
 * point to cluster.
 */
void CliArguments::setLastLineNumber(size_t lastLineIndex) {

    CliArguments::lastLineIndex = lastLineIndex;
}

/**
 * Returns the number of threads to use when computing the distance between two points to
 * cluster.
 *
 * @return The number of threads to use when computing the distance between two points to
 * cluster.
 */
std::size_t CliArguments::getDistanceComputationThreadsCount() const {

    return this->distanceComputationThreadsCount;
}

/**
 * Sets the number of threads to use when computing the distance between two points to
 * cluster.
 *
 * @param distanceComputationThreadsCount Number of threads to use when computing the
 * distance between two points to cluster.
 */
void CliArguments::setDistanceComputationThreadsCount(
        const std::size_t distanceComputationThreadsCount) {

    CliArguments::distanceComputationThreadsCount = distanceComputationThreadsCount;
}

/**
 * Returns the number of threads to use when fixing the structure of the dendrogram after a new
 * point is added.
 *
 * @return The number of threads to use when fixing the structure of the dendrogram after a new
 * point is added.
 */
std::size_t CliArguments::getStructuralFixThreadsCount() const {

    return this->structuralFixThreadsCount;
}

/**
 * Sets the number of threads to use when fixing the structure of the dendrogram after a new
 * point is added.
 *
 * @param structuralFixThreadsCount Number of threads to use when fixing the structure of the
 * dendrogram after a new point is added
 */
void CliArguments::setStructuralFixThreadsCount(const std::size_t structuralFixThreadsCount) {

    CliArguments::structuralFixThreadsCount = structuralFixThreadsCount;
}

/**
 * Returns the number of threads to use when computing the square root of the distances if the
 * algorithm has not already computed them.
 *
 * @return The number of threads to use when computing the square root of the distances if the
 * algorithm has not already computed them
 */
std::size_t CliArguments::getSqrtComputationThreadsCount() const {

    return this->sqrtComputationThreadsCount;
}

/**
 * Sets the number of threads to use when computing the square root of the distances if the
 * algorithm has not already computed them.
 *
 * @param sqrtComputationThreadsCount Number of threads to use when computing the square root
 * of the distances if the algorithm has not already computed them
 */
void CliArguments::setSqrtComputationThreadsCount(const std::size_t sqrtComputationThreadsCount) {

    CliArguments::sqrtComputationThreadsCount = sqrtComputationThreadsCount;
}

/**
 * Returns whether the user has requested to create the output files.
 *
 * @return Whether the user has requested to create the output files.
 */
bool CliArguments::isOutputEnabled() const {

    return this->outputEnabled;
}

/**
 * Sets whether the user has requested to create the output files.
 *
 * @param outputEnabled Whether the user has requested to create the output files.
 */
void CliArguments::setOutputEnabled(const bool outputEnabled) {

    CliArguments::outputEnabled = outputEnabled;
}

/**
 * Returns the path of the file where the information about the points and clusters are stored.
 *
 * @return The path of the file where the information about the points and clusters are stored.
 */
const std::filesystem::path& CliArguments::getOutputFilePath() const {

    return this->outputFilePath;
}

/**
 * Sets the path of the file where the information about the points and clusters are stored.
 *
 * @param outputFilePath Path of the file where the information about the points and clusters
 * are stored.
 */
void CliArguments::setOutputFilePath(const std::filesystem::path& outputFilePath) {

    CliArguments::outputFilePath = outputFilePath;
}

/**
 * Returns the path of the file where the Mathematica script that generates the dendrogram will
 * be stored.
 *
 * @return The path of the file where the Mathematica script that generates the dendrogram will
 * be stored.
 */
const std::filesystem::path& CliArguments::getMathematicaOutputFilePath() const {

    return this->mathematicaOutputFilePath;
}

/**
 * Sets the path of the file where the Mathematica script that generates the dendrogram will
 * be stored.
 *
 * @param mathematicaOutputFilePath Path of the file where the Mathematica script that generates
 * the dendrogram will be stored.
 */
void CliArguments::setMathematicaOutputFilePath(
        const std::filesystem::path& mathematicaOutputFilePath) {

    CliArguments::mathematicaOutputFilePath = mathematicaOutputFilePath;
}

/**
 * Returns whether the user has requested to execute a parallel implementation of the clustering
 * algorithm.
 *
 * @return Whether the user has requested to execute a parallel implementation of the clustering
 * algorithm.
 */
bool CliArguments::isParallel() const {

    return this->parallel;
}

/**
 * Sets whether the user has requested to execute a parallel implementation of the clustering
 * algorithm.
 *
 * @param parallel Whether the user has requested to execute a parallel implementation of the
 * clustering algorithm.
 */
void CliArguments::setParallel(const bool parallel) {

    CliArguments::parallel = parallel;
}

/**
 * Returns the version of the clustering algorithm to execute.
 *
 * @return The version of the clustering algorithm to execute.
 */
std::size_t CliArguments::getAlgorithmVersion() const {

    return this->algorithmVersion;
}

/**
 * Sets the version of the clustering algorithm to execute.
 *
 * @param algorithmVersion Version of the clustering algorithm to execute.
 */
void CliArguments::setAlgorithmVersion(const std::size_t algorithmVersion) {

    CliArguments::algorithmVersion = algorithmVersion;
}

/**
 * Returns whether the test mode is enabled.
 *
 * @return Whether the test mode is enabled.
 */
bool CliArguments::isTestModeEnabled() const {
    return this->testModeEnabled;
}

/**
 * Sets whether the test mode is enabled.
 *
 * @param testModeEnabled Whether the test mode is enabled.
 */
void CliArguments::setTestModeEnabled(const bool testModeEnabled) {

    CliArguments::testModeEnabled = testModeEnabled;
}

/**
 * Returns whether, to check the results of the requested clustering algorithm, instead of
 * executing the sequential clustering algorithm and then verify if they match, the results of
 * the sequential clustering are read from an external file.
 *
 * @return Whether, to check the results of the requested clustering algorithm, instead of
 * executing the sequential clustering algorithm and then verify if they match, the results of
 * the sequential clustering are read from an external file.
 */
bool CliArguments::isPreviousTestResultsToBeUsed() const {

    return this->previousTestResultsToBeUsed;
}

/**
 * Sets whether, to check the results of the requested clustering algorithm, instead of
 * executing the sequential clustering algorithm and then verify if they match, the results of
 * the sequential clustering are read from an external file.
 *
 * @param previousTestResultsToBeUsed Whether, to check the results of the requested clustering
 * algorithm, instead of executing the sequential clustering algorithm and then verify if they
 * match, the results of the sequential clustering are read from an external file.
 */
void CliArguments::setPreviousTestResultsToBeUsed(const bool previousTestResultsToBeUsed) {

    CliArguments::previousTestResultsToBeUsed = previousTestResultsToBeUsed;
}

/**
 * Returns the path of the file that stores the result of the sequential execution of the
 * clustering algorithm.
 *
 * @return The path of the file that stores the result of the sequential execution of the
 * clustering algorithm.
 */
const std::filesystem::path& CliArguments::getTestResultsFilePath() const {

    return this->testResultsFilePath;
}

/**
 * Sets the path of the file that stores the result of the sequential execution of the
 * clustering algorithm.
 *
 * @param testResultsFilePath Path of the file that stores the result of the sequential
 * execution of the clustering algorithm.
 */
void CliArguments::setTestResultsFilePath(const std::filesystem::path& testResultsFilePath) {

    CliArguments::testResultsFilePath = testResultsFilePath;
}

/**
 * Returns the path of the file that contains the coordinates of the points to cluster.
 *
 * @return The path of the file that contains the coordinates of the points to cluster.
 */
const std::filesystem::path& CliArguments::getInputFilePath() const {

    return this->inputFilePath;
}

/**
 * Sets the path of the file that contains the coordinates of the points to cluster.
 *
 * @param inputFilePath Path of the file that contains the coordinates of the points to cluster.
 */
void CliArguments::setInputFilePath(const std::filesystem::path& inputFilePath) {

    CliArguments::inputFilePath = inputFilePath;
}

}  // namespace cluster::test::cli
