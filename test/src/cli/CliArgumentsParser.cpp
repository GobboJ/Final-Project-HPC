/*
 * CliArgumentsParser implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2 2022-08-22
 * @since 1.0
 */
#include "CliArgumentException.h"
#include "CliArgumentsParser.h"
#include <cstring>
#include <filesystem>
#include <iostream>

namespace cluster::test::cli {

/**
 * Map containing the information of the well-known tests.
 */
const std::unordered_map<std::string, std::tuple<std::string, std::size_t, std::size_t>>
        // NOLINTNEXTLINE(cert-err58-cpp)
        CliArgumentsParser::WELL_KNOWN_TESTS{{"birm", {"Parking Birmingham.data", 2, 3}},
                                             {"slide", {"slide.data", 1, 2}},
                                             {"iris", {"iris.data", 1, 4}},
                                             {"acc", {"accelerometer.csv", 3, 5}},
                                             {"gen", {"generated.data", 1, 32}},
                                             {"two-points", {"two-points.data", 1, 2}},
                                             {"samples", {"samples.data", 1, 2}},
                                             {"same-distance", {"same-distance.data", 1, 2}},
                                             {"slide", {"slide.data", 1, 2}}};

/**
 * Creates a new parser of the command line arguments.
 *
 * @param argc Number of command line arguments.
 * @param argv Vector containing the command line arguments.
 */
CliArgumentsParser::CliArgumentsParser(const int argc, const char *const *argv) :
    argumentsCount{static_cast<std::size_t>(argc)},
    nextArgumentIndex{1},
    argumentsVector{argv} {

    // Check the arguments
    if (argc < 0) {
        throw std::invalid_argument("The number of arguments must be non-negative");
    }
}

/**
 * Parses the command line arguments.
 *
 * @return A container containing the parsed command line arguments.
 * @throws CliArgumentException If the specified command line arguments are not correct.
 */
CliArguments CliArgumentsParser::parseArguments() {

    // Create the result container
    CliArguments result{};

    // Check if the help has been requested
    if (this->argumentsCount == 2 && (strcmp(this->argumentsVector[1], "-h") == 0 ||
                                      strcmp(this->argumentsVector[1], "--help") == 0)) {
        // Print the usage and exit
        usage();
        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        exit(0);
    }

    // Flag indicating whether the version of the clustering algorithm to use have been specified
    bool versionSpecified = false;

    // Parse all the arguments
    while (this->nextArgumentIndex < this->argumentsCount - 1) {
        // Extract the argument
        const std::string argument{this->argumentsVector[this->nextArgumentIndex]};
        this->nextArgumentIndex++;

        // Parse the argument
        if (argument == "-c") {
            this->parseColumnsOption(result);
        } else if (argument == "-l") {
            this->parseLinesOption(result);
        } else if (argument.starts_with("--mathematica-output-path=")) {
            CliArgumentsParser::parseMathematicaOutputOption(result, argument);
        } else if (argument == "-n") {
            this->parseNumberOfThreadsOption(result);
        } else if (argument == "-p") {
            versionSpecified = true;
            this->parseAlgorithmVersion(result, true);
        } else if (argument == "-s") {
            versionSpecified = true;
            this->parseAlgorithmVersion(result, false);
        } else if (argument == "-t") {
            CliArgumentsParser::parseTestOption(result);
        } else if (argument.starts_with("--test-results-path=")) {
            CliArgumentsParser::parseTestResultsPath(result, argument);
        } else if (argument.starts_with("--visualizer-output-path=")) {
            CliArgumentsParser::parseVisualizerOutputOption(result, argument);
        } else {
            using namespace std::literals::string_literals;
            throw CliArgumentException("Unknown option"s + ' ' + argument);
        }
    }
    // Extract the input file path
    this->parseInputFilePath(result);

    // Require the mandatory parameters to be specified
    if (!versionSpecified) {
        throw CliArgumentException("None of the mandatory options -p and -s has been specified.");
    }

    return result;
}

/**
 * Parses the -c option.
 *
 * @param result Container that will contain the parsed arguments.
 * @throws CliArgumentException If the specified command line arguments are not correct.
 */
void CliArgumentsParser::parseColumnsOption(CliArguments &result) {

    // Extract the first column number
    const std::string firstColumnNumberString = this->getCurrentArgumentAndMoveNext(
            "The number of the first column is missing in the -c option");
    // Extract the last column number
    const std::string lastColumnNumberString = this->getCurrentArgumentAndMoveNext(
            "The number of the last column is missing in the -c option");

    // Parse the values
    const std::size_t firstColumnNumber =
            parseSizeT(firstColumnNumberString,
                       "The number of the first column specified in the -c option is not valid");
    const std::size_t lastColumnNumber =
            parseSizeT(lastColumnNumberString,
                       "The number of the last column specified in the -c option is not valid");

    // Check for their validity
    if (firstColumnNumber < 1) {
        throw CliArgumentException(
                "The number of the first column specified in the -c option must be greater than 0");
    }
    if (lastColumnNumber < 1) {
        throw CliArgumentException(
                "The number of the last column specified in the -c option must be greater than 0");
    }
    if (lastColumnNumber < firstColumnNumber) {
        throw CliArgumentException(
                "The number of the first column must be smaller than or equal to the number of the "
                "last column in the -c option");
    }
    // Set the values in the result
    result.setFirstColumnNumber(firstColumnNumber);
    result.setLastColumnNumber(lastColumnNumber);
}

/**
 * Parses the -l option.
 *
 * @param result Container that will contain the parsed arguments.
 * @throws CliArgumentException If the specified command line arguments are not correct.
 */
void CliArgumentsParser::parseLinesOption(CliArguments &result) {

    // Extract the first line number
    const std::string firstLineNumberString = this->getCurrentArgumentAndMoveNext(
            "The number of the first line is missing in the -l option");
    // Extract the last line number
    const std::string lastLineNumberString = this->getCurrentArgumentAndMoveNext(
            "The number of the last line is missing in the -l option");

    // Parse the values
    const std::size_t firstLineNumber =
            parseSizeT(firstLineNumberString,
                       "The number of the first line specified in the -l option is not valid");
    const std::size_t lastLineNumber =
            parseSizeT(lastLineNumberString,
                       "The number of the last line specified in the -l option is not valid");

    // Check for their validity
    if (firstLineNumber < 1) {
        throw CliArgumentException(
                "The number of the first line specified in the -l option must be greater than 0");
    }
    if (lastLineNumber < 1) {
        throw CliArgumentException(
                "The number of the last line specified in the -l option must be greater than 0");
    }
    if (lastLineNumber < firstLineNumber) {
        throw CliArgumentException(
                "The number of the first line must be smaller than or equal to the number of the "
                "last line in the -l option");
    }
    // Set the values in the result
    result.setFirstLineNumber(firstLineNumber);
    result.setLastLineNumber(lastLineNumber);
}

/**
 * Parses the -n option.
 *
 * @param result Container that will contain the parsed arguments.
 * @throws CliArgumentException If the specified command line arguments are not correct.
 */
void CliArgumentsParser::parseNumberOfThreadsOption(CliArguments &result) {

    // Extract the number of threads to compute the distance between points
    const std::string distanceThreadsString = this->getCurrentArgumentAndMoveNext(
            "The number of threads to use to compute the distance between points is missing in the "
            "-n option");
    // Extract the number of threads to compute the stage 4
    const std::string stage4ThreadsString = this->getCurrentArgumentAndMoveNext(
            "The number of threads to use to compute stage 4 is missing in the -n option");
    // Extract the number of threads to compute the stage 5
    const std::string stage5ThreadsString = this->getCurrentArgumentAndMoveNext(
            "The number of threads to use to compute stage 5 is missing in the -n option");

    // Parse the values
    const std::size_t distanceThreads =
            parseSizeT(distanceThreadsString,
                       "The number of threads to use to compute the distance between points "
                       "specified in the -n option is not valid");
    const std::size_t stage4Threads = parseSizeT(stage4ThreadsString,
                                                 "The number of threads to use to compute stage 4 "
                                                 "specified in the -n option is not valid");
    const std::size_t stage5Threads = parseSizeT(stage5ThreadsString,
                                                 "The number of threads to use to compute stage 5 "
                                                 "specified in the -n option is not valid");

    // Set the values in the result
    result.setDistanceComputationThreadsCount(distanceThreads);
    result.setStructuralFixThreadsCount(stage4Threads);
    result.setSqrtComputationThreadsCount(stage5Threads);
}

/**
 * Parses the --visualizer-output-path option.
 *
 * @param result Container where the parsed arguments will be placed.
 * @param option The string containing the full argument specified in the command line.
 * @throws CliArgumentException If the specified command line arguments are not correct.
 */
void CliArgumentsParser::parseVisualizerOutputOption(CliArguments &result,
                                                     const std::string &option) {

    // Extract the path
    const std::string outputPathString = option.substr(VISUALIZER_OUTPUT_OPTION_LENGTH);

    // Resolve the path
    std::filesystem::path outputPath{outputPathString};
    outputPath = absolute(outputPath).lexically_normal();

    // Check its validity
    using namespace std::literals::string_literals;
    requireFilePathValidity(
            outputPath,
            "The directory"s + ' ' + outputPath.parent_path().string() +
                    " where the output file will be generated does not exist",
            "The output file path"s + ' ' + outputPath.string() + " refers to a non-regular file");

    // Set the values in the result
    result.setVisualizerOutputEnabled(true);
    result.setVisualizerOutputFilePath(outputPath);
}

/**
 * Parses the --mathematica-output-path option.
 *
 * @param result Container where the parsed arguments will be placed.
 * @param option The string containing the full argument specified in the command line.
 * @throws CliArgumentException If the specified command line arguments are not correct.
 */
void CliArgumentsParser::parseMathematicaOutputOption(CliArguments &result,
                                                      const std::string &option) {

    // Extract the path
    const std::string mathematicaOutputPathString = option.substr(MATHEMATICA_OUTPUT_OPTION_LENGTH);

    // Resolve the path
    std::filesystem::path mathematicaOutputPath{mathematicaOutputPathString};
    mathematicaOutputPath = absolute(mathematicaOutputPath).lexically_normal();

    // Check its validity
    using namespace std::literals::string_literals;

    requireFilePathValidity(
            mathematicaOutputPath,
            "The directory"s + ' ' + mathematicaOutputPath.parent_path().string() +
                    " where the Mathematica script file will be generated does not exist",
            "The Mathematica script file path"s + ' ' + mathematicaOutputPath.string() +
                    " refers to a non-regular file");

    // Set the values in the result
    result.setMathematicaOutputEnabled(true);
    result.setMathematicaOutputFilePath(mathematicaOutputPath);
}

/**
 * Parses the version specified in the -p and -s options.
 *
 * @param result Container that will contain the parsed arguments.
 * @throws CliArgumentException If the specified command line arguments are not correct.
 */
void CliArgumentsParser::parseAlgorithmVersion(CliArguments &result, bool isParallel) {

    using namespace std::literals::string_literals;

    // Extract the version number
    const std::string versionString = this->getCurrentArgumentAndMoveNext(
            "The version is missing in the"s + ' ' + ((isParallel) ? "-p" : "-s") + " option");

    // Parse the version number
    const std::size_t version =
            parseSizeT(versionString,
                       "The version specified in the"s + ' ' + ((isParallel) ? "-p" : "-s") +
                               " option is not valid");

    // Set the values in the result
    result.setParallel(isParallel);
    result.setAlgorithmVersion(version);
}

/**
 * Parses the --test-results-path option.
 *
 * @param result Container that will contain the parsed arguments.
 * @throws CliArgumentException If the specified command line arguments are not correct.
 */
void CliArgumentsParser::parseTestOption(CliArguments &result) {

    // Enable the test mode
    result.setTestModeEnabled(true);
}

/**
 * Parses the --test-results-path option.
 *
 * @param result Container where the parsed arguments will be placed.
 * @param option The string containing the full argument specified in the command line.
 * @throws CliArgumentException If the specified command line argument is incorrect.
 */
void CliArgumentsParser::parseTestResultsPath(CliArguments &result, const std::string &option) {

    // Extract the path
    const std::string textResultsPathString = option.substr(TEST_RESULTS_PATH_OPTION_LENGTH);

    // Normalize the path
    std::filesystem::path textResultsPath{textResultsPathString};
    textResultsPath = absolute(textResultsPath).lexically_normal();

    // Check the path
    using namespace std::literals::string_literals;
    requireFilePathValidity(textResultsPath,
                            "The directory"s + ' ' + textResultsPath.parent_path().string() +
                                    " where the test results file will be generated does not exist",
                            "The path of the file containing the test results"s + ' ' +
                                    textResultsPath.string() + " refers to a non-regular file");
    // Set the path
    result.setTestModeEnabled(true);
    result.setPreviousTestResultsToBeUsed(true);
    result.setTestResultsFilePath(textResultsPath);
}

/**
 * Parses the input file path.
 *
 * @param result Container that will contain the parsed arguments.
 * @throws CliArgumentException If the specified command line arguments are not correct.
 */
void CliArgumentsParser::parseInputFilePath(CliArguments &result) {

    // Extract the path
    const std::string inputFilePathString =
            this->getCurrentArgumentAndMoveNext("The input file path is missing");
    std::filesystem::path inputFilePath{};

    // Check if it is a known test
    if (WELL_KNOWN_TESTS.contains(inputFilePathString)) {
        // Extract the information
        const std::tuple<std::string, std::size_t, std::size_t> &testInformation =
                WELL_KNOWN_TESTS.at(inputFilePathString);
        // Set the path
        inputFilePath = "../../test/resources";
        inputFilePath = inputFilePath / std::get<0>(testInformation);

        // Set also the columns, if they have not been specified
        if (result.getLastColumnNumber() == 0) {
            result.setFirstColumnNumber(std::get<1>(testInformation));
            result.setLastColumnNumber(std::get<2>(testInformation));
        }
    } else {
        // Extract the specified path
        inputFilePath = inputFilePathString;
    }

    // Normalize the path
    inputFilePath = std::filesystem::absolute(inputFilePath).lexically_normal();

    // Check the path existence
    if (!std::filesystem::exists(inputFilePath)) {
        using namespace std::literals::string_literals;
        throw CliArgumentException(
                "The input file"s + ' ' + inputFilePath.string() + " does not exist");
    }
    if (!std::filesystem::is_regular_file(inputFilePath)) {
        using namespace std::literals::string_literals;
        throw CliArgumentException(
                "The input file"s + ' ' + inputFilePath.string() + " refers to a non-regular file");
    }

    // Set the path
    result.setInputFilePath(inputFilePath);
}

/**
 * Returns the first non-parsed argument, and marks it as parsed.
 *
 * @param exceptionMessage Message of the exception thrown if the first non-parsed argument is not
 * valid.
 * @return The first non-parsed argument.
 * @throws CliArgumentException If the first non-parsed argument is not valid.
 */
std::string CliArgumentsParser::getCurrentArgumentAndMoveNext(const std::string &exceptionMessage) {

    // Require at least one non-parsed argument to be left
    if (this->nextArgumentIndex >= this->argumentsCount) {
        throw CliArgumentException(exceptionMessage);
    }

    // Get the current argument and store it in a non-const variable so to allow automatic move
    std::string argument{this->argumentsVector[this->nextArgumentIndex]};
    // Move to the next argument
    this->moveToNextArgument();

    // Require the argument not be an option
    if (isOption(argument)) {
        throw CliArgumentException(exceptionMessage);
    }

    return argument;
}

/**
 * Returns the first non-parsed command line argument.
 *
 * @return The first non-parsed command line argument.
 */
std::string CliArgumentsParser::peekCurrentArgument() {

    // Check if there are arguments to parse left
    if (this->argumentsCount >= this->nextArgumentIndex) {
        // Empty string signals that there are no arguments to parse left
        return "";
    }

    // Convert the C string into a std::string
    return {this->argumentsVector[nextArgumentIndex]};
}

/**
 * Marks the first non-parsed argument as parsed.
 */
void CliArgumentsParser::moveToNextArgument() {

    // Move to the next argument
    this->nextArgumentIndex++;
}

/**
 * Parses a string as a <code>std::size_t</code>.
 *
 * @param string String to parse.
 * @param exceptionMessage Message of the exception thrown if an error occur while parsing the
 * string.
 * @return The parsed value.
 * @throws CliArgumentException If an error occur while parsing the string.
 */
std::size_t CliArgumentsParser::parseSizeT(const std::string &string,
                                           const std::string &exceptionMessage) {

    // Index of the first character of the string that has not been parsed
    std::size_t firstNonParsedCharacterIndex = 0;

    // Parsed value
    // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
    std::size_t convertedValue;

    try {
        // Try to parse the value
        convertedValue = std::stoull(string, &firstNonParsedCharacterIndex);
    } catch (std::exception &exception) {
        // Compose the message of the exception
        std::string message{exceptionMessage};
        message += ": ";
        message += exception.what();

        // Throw the exception with the compose message
        throw CliArgumentException(message);
    }
    // Require at least one character to have been parsed
    if (firstNonParsedCharacterIndex == 0) {
        throw CliArgumentException(exceptionMessage);
    }

    return convertedValue;
}

/**
 * Checks whether the specified argument is an option.
 *
 * @param argument Argument to check.
 * @return Whether the specified argument is an option.
 */
bool CliArgumentsParser::isOption(const std::string &argument) {

    return argument == "-c" || argument == "-h" || argument == "-l" || argument == "-n" ||
           argument == "-o" || argument == "-p" || argument == "-s" || argument == "-t";
}

/**
 * Requires the specified file path to be valid.
 *
 * @param filePath Path to check.
 * @param directoryExceptionMessage Message of the exception thrown if the path refers to a
 * non-existent file in a non-valid directory.
 * @param fileExceptionMessage Message of the exception thrown if the path refers to a non-regular
 * file.
 * @throws CliArgumentException If the specified file path is not valid.
 */
void CliArgumentsParser::requireFilePathValidity(const std::filesystem::path &filePath,
                                                 const std::string &directoryExceptionMessage,
                                                 const std::string &fileExceptionMessage) {

    // Require the file to be in a valid directory
    if (!std::filesystem::is_directory(filePath.parent_path())) {
        throw CliArgumentException(directoryExceptionMessage);
    }

    // Require the file to be a regular file, if it exists
    if (std::filesystem::exists(filePath) && !std::filesystem::is_regular_file(filePath)) {
        throw CliArgumentException(fileExceptionMessage);
    }
}

/**
 * Prints the usage of the program.
 */
void CliArgumentsParser::usage() {

    std::cout <<
            R""(SYNOPSYS
    main -h | --help
    main OPTIONS... INPUT_FILE_PATH

DESCRIPTION
    Runs the clustering algorithm on the points specified in the INPUT_FILE_PATH
    file. This file must be list rows of comma-separated values, where each row
    describes a point, while each column of the row describe a coordinate of
    that point.
    The input file may contain more columns or more rows than needed. Use the
    -c and -r options to deal with these cases.
    
    There are some well-known files that do not require the specification of the
    full path, nor the columns range. These files are contained in the test
    resources directory, assumed to be in the path ../../test/resources.
    The aforementioned files can be used by specifying:
        - "acc"           for "accelerometer.csv",       columns from 3 to 5;
        - "birm"          for "Parking Birmingham.data", columns from 2 to 3;
        - "gen"           for "generated.data",          columns from 1 to 32;
        - "iris"          for "iris.data",               columns from 1 to 4;
        - "same-distance" for "same-distance.data",      columns from 1 to 2;
        - "samples"       for "samples.data",            columns from 1 to 2;
        - "slide"         for "slide.data",              columns from 1 to 2;
        - "two-points"    for "two-points.data",         columns from 1 to 2.

OPTIONS
    -c FIRST LAST
        Specifies the range of columns to be considered as coordinates of the
        points to cluster in each line of the INPUT_FILE_PATH file. Any other
        column outside this range is not considered as a coordinate.
        
        FIRST and LAST are inclusive, and they range from 1 on, where 1 means
        the first line, 2 the second line and so on so forth.

        If this option is not specified, then all the columns of each line of
        the INPUT_FILE_PATH file are considered containing coordinates of the
        points.

    -h, --help
        Prints this help.
    
    -l FIRST LAST
        Specifies the range of lines of the INPUT_FILE_PATH file that contains
        coordinates of points to cluster. Any other line outside this range
        is not considered as a point, hence it is excluded from the clustering
        algorithm.

        FIRST and LAST are inclusive, and they range from 1 on, where 1 means
        the first line, 2 the second line and so on so forth.

        If this option is not specified, then all the lines of the
        INPUT_FILE_PATH are considered as points to cluster.
    
    --mathematica-output-path=MATHEMATICA_OUTPUT_PATH
        Specifies the path of the output file that will be filled with the
        Mathematica code that generates the dendrogram tree of the clusters.
   
        If this option is omitted, then no output file is created.

    -n DISTANCE_THREADS STRUCTURAL_FIX_THREADS SQRT_THREADS
        Specifies the number of threads to use when parallelizing the stages of
        the clustering algorithm.
        
        In particular:
            - DISTANCE_THREADS specifies the number of threads to use when
              parallelizing the computation of the distance between two points;
            - STRUCTURAL_FIX_THREADS specifies the number of threads to use when
              parallelizing the fix of the structure of the dendrogram after a
              new point has been added;
            - SQRT_THREADS specifies the number of threads to use when
              parallelizing the computation of the square roots of the
              distances, if the clustering has used the squares.
        
        All the three numbers must be non-negative. If 0 is specified, then you
        let OpenMP use the default number of threads when parallelizing the
        corresponding activity.
        
        This option takes effect only if the -p option is specified as well.
        
        If this option is omitted, then all the values are assumed to be 0,
        hence OpenMP will use the default number of threads for all the three
        stages.

    -p VERSION
        Specifies the VERSION of the parallel clustering algorithm to use. See
        the PARALLEL VERSIONS section to know what number correspond to a
        particular version.
        
        This option is mandatory, and cannot be specified together with the -s
        option.
    
    -s VERSION
        Specifies the VERSION of the sequential clustering algorithm to use. See
        the SEQUENTIAL VERSIONS section to know what number correspond to a
        particular version.
        
        This option is mandatory, and cannot be specified together with the -p
        option.
    
    -t
        Enables the test mode. This implies that the results of the execution of
        the specified implementation of the clustering algorithm are checked
        against the results of the sequential implementation.
        
        If the two results are different, then the program terminates with exit
        code 1. Otherwise, it terminates with exit code 0.
        
        If this option is omitted, then the test mode is disabled.
    
    --test-results-path=TEST_RESULTS_PATH
        Avoids the execution of the sequential implementation every time the
        program is executed, by reading results of the sequential implementation
        from the TEST_RESULTS_PATH file.
        
        If the file does not exist, then the sequential implementation of the
        clustering algorithm is executed, and the results are stored in the
        specified path.

        If this option is omitted, then the sequential implementation of the
        clustering algorithm is always executed to retrieve the correct results.
        
        This option implies -t.
    
    --visualizer-output-path=OUTPUT_PATH
        Specifies the path of the output file that will be filled with the
        information about the points and the clusters. This file can be given to
        one of the DeB Cluster Visualizer applications.
    
        If this option is omitted, then no output file is created.

SEQUENTIAL VERSIONS
        1   Sequential implementation that uses a data structure with two levels
            of indirection to hold the samples to cluster.
        2   Sequential implementation that uses a unique contiguous array to
            hold the samples to cluster.
PARALLEL VERSIONS
        1   Parallel implementation that uses a data structure with two levels
            of indirection to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using threads.
        2   Parallel implementation that uses a data structure with two levels
            of indirection to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using SSE
            instructions.
        3   Parallel implementation that uses a data structure with two levels
            of indirection to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using AVX
            instructions.
        4   Parallel implementation that uses a unique aligned and contiguous
            array to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using both
            threads and AVX instructions.
        5   Parallel implementation that uses a data structure with two levels
            of indirection to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using both
            threads and SSE instructions. Moreover, the rearrangement of the
            structure of the dendrogram after a new point it added is performed
            in parallel by using threads.
        6   Parallel implementation that uses a data structure with two levels
            of indirection to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using both
            threads and SSE instructions. This implementation does not store
            into memory the partial sums when computing the distances, but it
            keeps them into the registers. Moreover, the rearrangement of the
            structure dendrogram after a new point it added is performed in
            parallel by using threads.
        7   Parallel implementation that uses a data structure with two levels
            of indirection to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using both
            threads and AVX instructions. This implementation does not store
            into memory the partial sums when computing the distances, but it
            keeps them into the registers. Moreover, the rearrangement of the
            structure dendrogram after a new point it added is performed in
            parallel by using threads.
        8   Parallel implementation that uses a unique aligned and contiguous
            array to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using both
            threads and SSE instructions. This implementation does not store
            into memory the partial sums when computing the distances, but it
            keeps them into the registers. Moreover, the rearrangement of the
            structure dendrogram after a new point it added is performed in
            parallel by using threads.
        9   Parallel implementation that uses a unique aligned and contiguous
            array to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using both
            threads and AVX instructions. This implementation does not store
            into memory the partial sums when computing the distances, but it
            keeps them into the registers. Moreover, the rearrangement of the
            structure dendrogram after a new point it added is performed in
            parallel by using threads.
        10  Parallel implementation that uses a unique aligned and contiguous
            array to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using both
            threads and AVX instructions. This implementation does not store
            into memory the partial sums when computing the distances, but it
            keeps them into the registers. Moreover, the rearrangement of the
            structure dendrogram after a new point it added is performed in
            parallel by using threads. In addition, this implementation does not
            compute the square root while computing the distances, but perform
            this computation at the end of the algorithm.
        11  Parallel implementation that uses a unique aligned and contiguous
            array to hold the samples to cluster, and parallelizes the
            computation of the distance between two data samples using both
            threads and AVX instructions. This implementation does not store
            into memory the partial sums when computing the distances, but it
            keeps them into the registers. Moreover, the rearrangement of the
            structure dendrogram after a new point it added is performed in
            parallel by using threads. In addition, this implementation does not
            compute the square root while computing the distances, but perform
            this computation in parallel using threads at the end of the
            algorithm.
        
EXIT CODE
    0   If the -t option has not been specified, of if it has been specified and
        the results of the specified implementation of the clustering algorithm
        match the results of the sequential implementation.

    1   If the -t option has been specified and the results of the specified
        implementation of the clustering algorithm do not match the results
        of the sequential implementation.

    2   If at least one of the specified arguments is wrong.

    3   If the specified input file path contains invalid information.

)"";
}

}  // namespace cluster::test::cli
