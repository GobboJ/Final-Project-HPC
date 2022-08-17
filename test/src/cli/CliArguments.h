#ifndef FINAL_PROJECT_HPC_CLIARGUMENTS_H
#define FINAL_PROJECT_HPC_CLIARGUMENTS_H

#include <cstddef>
#include <filesystem>

namespace cluster::test::cli {

/**
 * Class representing the values specified via command line.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2 2022-08-06
 * @since 1.0
 */
class CliArguments {

public:
    /**
     * Returns the number of the first column of each line of the input file to be considered
     * containing the coordinates of the point.<br>
     * This value ranges from 0 on, where <code>0</code> means that no value has been specified.
     *
     * @return The number of the first column to consider as coordinates of the point.
     */
    std::size_t getFirstColumnNumber() const;

    /**
     * Sets the number of the first column of each line of the input file to be considered
     * containing the coordinates of the point.<br>
     * This value ranges from 0 on, where <code>0</code> means that no value has been specified.
     *
     * @param firstColumnNumber Value of the number of the first column to consider as coordinates
     * of the point.
     */
    void setFirstColumnNumber(std::size_t firstColumnNumber);

    /**
     * Returns the number of the last column of each line of the input file to be considered
     * containing the coordinates of the point.<br>
     * This value ranges from 0 on, where <code>0</code> means that no value has been specified.
     *
     * @return The index of the last column to consider as coordinates of the point.
     */
    std::size_t getLastColumnNumber() const;

    /**
     * Sets the number of the last column of each line of the input file to be considered
     * containing the coordinates of the point.<br>
     * This value ranges from 0 on, where <code>0</code> means that no value has been specified.
     *
     * @param lastColumnNumber Value of the number of the last column to consider as coordinates of
     * the point.
     */
    void setLastColumnNumber(std::size_t lastColumnNumber);

    /**
     * Returns the number of the first line of the input file to be considered containing points to
     * cluster.<br>
     * This value ranges from 0 on, where <code>0</code> means that no value has been specified.
     *
     * @return The number of the first line of the input file to consider as a point to cluster.
     */
    std::size_t getFirstLineNumber() const;

    /**
     * Sets the number of the first line of the input file to be considered containing points to
     * cluster.<br>
     * This value ranges from 0 on, where <code>0</code> means that no value has been specified.
     *
     * @param firstLineNumber Value of the number of the first line of the input file to consider as
     * a point to cluster.
     */
    void setFirstLineNumber(std::size_t firstLineNumber);

    /**
     * Returns the number of the last line of the input file to be considered containing points to
     * cluster.<br>
     * This value ranges from 0 on, where <code>0</code> means that no value has been specified.
     *
     * @return The number of the last line of the input file to consider as a point to cluster.
     */
    std::size_t getLastLineNumber() const;

    /**
     * Sets the number of the last line of the input file to be considered containing points to
     * cluster.<br>
     * This value ranges from 0 on, where <code>0</code> means that no value has been specified.
     *
     * @param lastLineIndex Value of the number of the last line of the input file to consider as a
     * point to cluster.
     */
    void setLastLineNumber(std::size_t lastLineIndex);

    /**
     * Returns the number of threads to use when computing the distance between two points to
     * cluster.<br>
     * The value <code>0</code> means that OpenMP will use the default value.
     *
     * @return The number of threads to use when computing the distance between two points to
     * cluster.
     */
    std::size_t getDistanceComputationThreadsCount() const;

    /**
     * Sets the number of threads to use when computing the distance between two points to
     * cluster.<br>
     * The value <code>0</code> means that OpenMP will use the default value.
     *
     * @param distanceComputationThreadsCount Number of threads to use when computing the
     * distance between two points to cluster.
     */
    void setDistanceComputationThreadsCount(std::size_t distanceComputationThreadsCount);

    /**
     * Returns the number of threads to use when fixing the structure of the dendrogram after a new
     * point is added.<br>
     * The value <code>0</code> means that OpenMP will use the default value.
     *
     * @return The number of threads to use when fixing the structure of the dendrogram after a new
     * point is added.
     */
    std::size_t getStructuralFixThreadsCount() const;

    /**
     * Sets the number of threads to use when fixing the structure of the dendrogram after a new
     * point is added.<br>
     * The value <code>0</code> means that OpenMP will use the default value.
     *
     * @param structuralFixThreadsCount Number of threads to use when fixing the structure of the
     * dendrogram after a new point is added
     */
    void setStructuralFixThreadsCount(std::size_t structuralFixThreadsCount);

    /**
     * Returns the number of threads to use when computing the square root of the distances if the
     * algorithm has not already computed them.<br>
     * The value <code>0</code> means that OpenMP will use the default value.
     *
     * @return The number of threads to use when computing the square root of the distances if the
     * algorithm has not already computed them
     */
    std::size_t getSqrtComputationThreadsCount() const;

    /**
     * Sets the number of threads to use when computing the square root of the distances if the
     * algorithm has not already computed them.<br>
     * The value <code>0</code> means that OpenMP will use the default value.
     *
     * @param sqrtComputationThreadsCount Number of threads to use when computing the square root
     * of the distances if the algorithm has not already computed them
     */
    void setSqrtComputationThreadsCount(std::size_t sqrtComputationThreadsCount);

    /**
     * Returns <code>true</code> if the user has requested to create the output files,
     * <code>false</code> otherwise.
     *
     * @return <code>true</code> if the user has requested to create the output files,
     * <code>false</code> otherwise.
     */
    bool isOutputEnabled() const;

    /**
     * Sets whether the user has requested to create the output files.
     *
     * @param outputEnabled <code>true</code> if the user has requested to create the output files,
     * <code>false</code> otherwise.
     */
    void setOutputEnabled(bool outputEnabled);

    /**
     * Returns the path of the file where the information about the points and clusters are stored.
     *
     * @return The path of the file where the information about the points and clusters are stored.
     */
    const std::filesystem::path& getOutputFilePath() const;

    /**
     * Sets the path of the file where the information about the points and clusters are stored.
     *
     * @param outputFilePath Path of the file where the information about the points and clusters
     * are stored.
     */
    void setOutputFilePath(const std::filesystem::path& outputFilePath);

    /**
     * Returns the path of the file where the Mathematica script that generates the dendrogram will
     * be stored.
     *
     * @return The path of the file where the Mathematica script that generates the dendrogram will
     * be stored.
     */
    const std::filesystem::path& getMathematicaOutputFilePath() const;

    /**
     * Sets the path of the file where the Mathematica script that generates the dendrogram will
     * be stored.
     *
     * @param mathematicaOutputFilePath Path of the file where the Mathematica script that generates
     * the dendrogram will be stored.
     */
    void setMathematicaOutputFilePath(const std::filesystem::path& mathematicaOutputFilePath);

    /**
     * Returns <code>true</code> if the user has requested the execution of a parallel
     * implementation of the clustering algorithm, <code>false</code> otherwise.
     *
     * @return <code>true</code> if the user has requested the execution of a parallel
     * implementation of the clustering algorithm, <code>false</code> otherwise.
     */
    bool isParallel() const;

    /**
     * Sets whether the user has requested to execute a parallel implementation of the clustering
     * algorithm.
     *
     * @param parallel <code>true</code> if the user has requested the execution of a parallel
     * implementation of the clustering algorithm, <code>false</code> otherwise.
     */
    void setParallel(bool parallel);

    /**
     * Returns the version of the clustering algorithm, either parallel or sequential, to execute.
     *
     * @return The version of the clustering algorithm, either parallel or sequential, to execute.
     */
    std::size_t getAlgorithmVersion() const;

    /**
     * Sets the version of the clustering algorithm, either parallel or sequential, to execute.
     *
     * @param algorithmVersion Version of the clustering algorithm, either parallel or sequential,
     * to execute.
     */
    void setAlgorithmVersion(std::size_t algorithmVersion);

    /**
     * Returns <code>true</code> is the test mode has been enabled, <code>false</code> otherwise.
     *
     * @return <code>true</code> is the test mode has been enabled, <code>false</code> otherwise.
     */
    bool isTestModeEnabled() const;

    /**
     * Sets whether the test mode is enabled.
     *
     * @param testModeEnabled <code>true</code> is the test mode is enabled, <code>false</code>
     * otherwise.
     */
    void setTestModeEnabled(bool testModeEnabled);

    /**
     * Returns whether, to check the results of the requested clustering algorithm, instead of
     * executing the sequential clustering algorithm and then verify if they match, the results of
     * the sequential clustering are read from an external file.
     *
     * @return <code>true</code> if the results stored in a file of a previous execution of the
     * sequential clustering algorithm are to be used, <code>false</code> otherwise.
     */
    bool isPreviousTestResultsToBeUsed() const;

    /**
     * Sets whether, to check the results of the requested clustering algorithm, instead of
     * executing the sequential clustering algorithm and then verify if they match, the results of
     * the sequential clustering are read from an external file.
     *
     * @param previousTestResultsToBeUsed <code>true</code> if the results stored in a file of a
     * previous execution of the sequential clustering algorithm are to be used, <code>false</code>
     * otherwise.
     */
    void setPreviousTestResultsToBeUsed(bool previousTestResultsToBeUsed);

    /**
     * Returns the path of the file that stores the result of the sequential execution of the
     * clustering algorithm.
     *
     * @return The path of the file that stores the result of the sequential execution of the
     * clustering algorithm.
     */
    const std::filesystem::path& getTestResultsFilePath() const;

    /**
     * Sets the path of the file that stores the result of the sequential execution of the
     * clustering algorithm.
     *
     * @param testResultsFilePath Path of the file that stores the result of the sequential
     * execution of the clustering algorithm.
     */
    void setTestResultsFilePath(const std::filesystem::path& testResultsFilePath);

    /**
     * Returns the path of the file that contains the coordinates of the points to cluster.
     *
     * @return The path of the file that contains the coordinates of the points to cluster.
     */
    const std::filesystem::path& getInputFilePath() const;

    /**
     * Sets the path of the file that contains the coordinates of the points to cluster.
     *
     * @param inputFilePath Path of the file that contains the coordinates of the points to cluster.
     */
    void setInputFilePath(const std::filesystem::path& inputFilePath);

private:
    /**
     * Index of the first column of the input file to consider as a coordinate of the point.
     */
    std::size_t firstColumnIndex;

    /**
     * Index of the last column of the input file to consider as a coordinate of the point.
     */
    std::size_t lastColumnIndex;

    /**
     * Index of the first row of the input file to consider as a point.
     */
    std::size_t firstLineIndex;

    /**
     * Index of the last row of the input file to consider as a point.
     */
    std::size_t lastLineIndex;

    /**
     * Number of threads to use to compute the distance between two points.
     */
    std::size_t distanceComputationThreadsCount;

    /**
     * Number of threads to use to fix the structure of the clusters.
     */
    std::size_t structuralFixThreadsCount;

    /**
     * Number of threads to use to compute the square root of the distances.
     */
    std::size_t sqrtComputationThreadsCount;

    /**
     * Flag indicating whether the generation of the output files is enabled.
     */
    bool outputEnabled;

    /**
     * Path of the file where the information about the points and the clusters will be stored.
     */
    std::filesystem::path outputFilePath;

    /**
     * Path of the file where the Mathematica script that generates the dendrogram tree will be
     * stored.
     */
    std::filesystem::path mathematicaOutputFilePath;

    /**
     * Flag indicating whether the parallel implementation of the clustering algorithm has been
     * requested.
     */
    bool parallel;

    /**
     * Version of the clustering algorithm to be executed, either parallel of sequential.
     */
    std::size_t algorithmVersion;

    /**
     * Flag indicating whether the test mode is enabled.
     */
    bool testModeEnabled;

    /**
     * Flag indicating whether to use the results of the sequential implementation of the clustering
     * algorithm computed in a previous test and stored in a file instead of executing it again.
     */
    bool previousTestResultsToBeUsed;

    /**
     * Path of the file to read to retrieve the results of the sequential implementation.
     */
    std::filesystem::path testResultsFilePath;

    /**
     * Path of the file containing the coordinates of the points to cluster.
     */
    std::filesystem::path inputFilePath;
};

}  // namespace cluster::test::cli

#endif  // FINAL_PROJECT_HPC_CLIARGUMENTS_H
