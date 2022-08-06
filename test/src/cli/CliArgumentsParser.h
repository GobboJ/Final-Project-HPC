#ifndef FINAL_PROJECT_HPC_CLIARGUMENTSPARSER_H
#define FINAL_PROJECT_HPC_CLIARGUMENTSPARSER_H

#include "CliArguments.h"
#include <filesystem>
#include <string>
#include <tuple>
#include <unordered_map>

namespace cluster::test::cli {
/**
 * Description.
 *
 * @author DeB, Jonathan
 * @version 1.1.1 2022-08-06
 * @since 1.0
 */
class CliArgumentsParser {

private:
    /**
     * Map associating the name of the well-known tests to their information, namely the complete
     * file name, and the number fo the first and last column to consider containing coordinates of
     * points.
     */
    static const std::unordered_map<std::string, std::tuple<std::string, std::size_t, std::size_t>>
            WELL_KNOWN_TESTS;

public:
    /**
     * Creates a new parser that parses the specified command line arguments.
     *
     * @param argc Number of arguments specified via command line.
     * @param argv Vector containing the arguments specified via command line. The parser assumes
     * that <code>argv[0]</code> contains the name of the invoked binary file, hence the first
     * relevant argument is <code>argv[1]</code>.
     */
    CliArgumentsParser(int argc, const char *const *argv);

    /**
     * Parses the command line arguments specified in the constructor. If only one command line
     * argument is specified, and if this argument is the -h option, then this method prints the
     * usage and terminates the execution of the program.
     *
     * @return A container containing the parsed command line arguments.
     * @throws CliArgumentException If the specified command line arguments are not valid.
     */
    CliArguments parseArguments();

private:
    /**
     * Parses the -c option.
     *
     * @param result Container where the parsed arguments will be placed.
     * @throws CliArgumentException If there are not enough non-parsed arguments left, or if the
     * specified column numbers are not valid numbers.
     */
    void parseColumnsOption(CliArguments &result);

    /**
     * Parses the -l option.
     *
     * @param result Container where the parsed arguments will be placed.
     * @throws CliArgumentException If there are not enough non-parsed arguments left, or if the
     * specified line numbers are not valid numbers.
     */
    void parseLinesOption(CliArguments &result);

    /**
     * Parses the -n option.
     *
     * @param result Container where the parsed arguments will be placed.
     * @throws CliArgumentException If there are not enough non-parsed arguments left, or if the
     * specified threads counts are not valid numbers.
     */
    void parseNumberOfThreadsOption(CliArguments &result);

    /**
     * Parses the -o option.
     *
     * @param result Container where the parsed arguments will be placed.
     * @throws CliArgumentException If there are not enough non-parsed arguments left, or if the
     * specified file paths are not valid.
     */
    void parseOutputOption(CliArguments &result);

    /**
     * Parses the version of the clustering algorithm to invoke. This method is called for both the
     * -p and -s option.
     *
     * @param result Container where the parsed version will be placed.
     * @param isParallel <code>true</code> if this method is invoked to parse the version of the
     * parallel clustering algorithm, <code>false</code> if it parses the version of the sequential
     * clustering algorithm.
     * @throws CliArgumentException If there are not enough non-parsed arguments left, or if the
     * specified version is not a valid number.
     */
    void parseAlgorithmVersion(CliArguments &result, bool isParallel);

    /**
     * Parses the -t option.
     *
     * @param result Container where the parsed arguments will be placed.
     * @throws CliArgumentException If there are not enough non-parsed arguments left, or if the
     * specified file where the tests results will be read or stored is not a valid path.
     */
    void parseTestOption(CliArguments &result);

    /**
     * Parses the input file path.
     *
     * @param result Container where the parsed path will be placed.
     * @throws CliArgumentException If there are not enough non-parsed arguments left, or if the
     * specified file does not exist.
     */
    void parseInputFilePath(CliArguments &result);

    /**
     * Returns the first non-parsed command line argument, and mark it as parsed, so that the next
     * invocation of this method returns the next command line argument to parse.
     *
     * @param exceptionMessage Message of the exception thrown if there are not enough non-parsed
     * arguments left, or if the first non-parsed argument is an option.
     * @return The first non-parsed command line argument
     * @throws CliArgumentException If there are not enough non-parsed arguments left, or if the
     * first non-parsed argument is an option.
     */
    std::string getCurrentArgumentAndMoveNext(const std::string &exceptionMessage);

    /**
     * Returns the first non-parsed argument. This argument will not be marked as parsed, so that
     * future invocations of this method, or of the
     * <code>CliArgumentsParser::getCurrentArgumentAndMoveNext</code> method will return the same
     * command line argument.<br>
     * If there are no more arguments to parse, the empty string is returned.
     *
     * @return The first non-parsed command line argument.
     */
    std::string peekCurrentArgument();

    /**
     * Marks the first non-parsed command line argument as parsed, so that any subsequent call to
     * <code>CliArgumentsParser::peekCurrentArgument</code> or
     * <code>CliArgumentsParser::getCurrentArgumentAndMoveNext</code> return the next argument.
     */
    void moveToNextArgument();

    /**
     * Extracts from the specified string a <code>std::size_t</code> number. If the specified string
     * does not contain a valid <code>std::size_t</code> number, an exception is thrown.
     *
     * @param string String to parse.
     * @param exceptionMessage Message of the exception thrown if the specified string does not
     * contain a valid <code>std::size_t</code> number.
     * @return The extracted <code>std::size_t</code> number.
     * @throws CliArgumentException If the specified string does not contain a valid
     * <code>std::size_t</code> number.
     */
    static std::size_t parseSizeT(const std::string &string, const std::string &exceptionMessage);

    /**
     * Checks whether the specified argument is an option (e.g., -c, -h, -l, ...).
     *
     * @param argument Argument to check.
     * @return <code>true</code> if the specified argument is an option, <code>false</code>
     * otherwise.
     */
    static bool isOption(const std::string &argument);

    /**
     * Requires the specified file path to be valid. In particular, it is considered valid if:
     * <ul>
     *      <li>The path refers to a non-existent file, but the directory in the specified path that
     * contains the file exists;</li>
     *      <li>The path refers to a regular file.</li>
     * </ul>
     *
     * @param filePath Path to check.
     * @param directoryExceptionMessage Message of the exception thrown if the path refers to a
     * non-existent file, but the directory in the specified path that contains the file is not a
     * valid directory.
     * @param fileExceptionMessage Message of the exception thrown if the path refers to a
     * non-regular file.
     * @throws CliArgumentException If the specified file path is not valid.
     */
    static void requireFilePathValidity(const std::filesystem::path &filePath,
                                        const std::string &directoryExceptionMessage,
                                        const std::string &fileExceptionMessage);

    /**
     * Prints the usage of the program to the console.
     */
    static void usage();

    /**
     * Number of command line arguments, including the binary file path.
     */
    const std::size_t argumentsCount;

    /**
     * Index of the first non-parsed argument.
     */
    std::size_t nextArgumentIndex;

    /**
     * Vector containing the command line arguments.
     */
    const char *const *argumentsVector;
};
}  // namespace cluster::test::cli
#endif  // FINAL_PROJECT_HPC_CLIARGUMENTSPARSER_H
