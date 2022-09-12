/*
 * Application that prints in a human-readable format the execution times measured by the
 * main-measurements application.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-09-12
 * @since 1.0
 */
#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

/**
 * Exception thrown if some errors occurred while parsing a number from the file containing the
 * measured times.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-09-11
 * @since 1.0
 */
class InvalidNumberException final : public std::runtime_error {

public:
    /**
     * Creates a new exception thrown if an error occurs while parsing a number read from the file
     * containing the measured times.
     *
     * @param message Message of the exception.
     */
    explicit InvalidNumberException(const char *message) : runtime_error(message) {
    }
};

/**
 * Prints to the console the specified duration in a human-readable format.
 *
 * @param durationNanoseconds Number of nanoseconds of the duration to print.
 */
void printDuration(std::uint64_t durationNanoseconds);

/**
 * Prints the specified measured times to the console.
 *
 * @param times Array containing the times measured in 3 executions of the
 * <code>main-measurements</code> application.
 */
void printTimes(const std::array<std::uint64_t, 3> &times);

/**
 * Prints to the specified file, in CSV format, all the times measured in one execution of the
 * <code>main-measurements</code> application.
 *
 * @param csvFile File where the execution times will be printed to.
 * @param iteration Number of the execution whose times print to the specified file.
 * @param serviceTimes Array containing the service times measured in 3 executions.
 * @param firstStages Array containing the time to execute the first step of the clustering
 * algorithm measured in the 3 executions.
 * @param secondStages Array containing the time to execute the second step of the clustering
 * algorithm measured in the 3 executions.
 * @param thirdStages Array containing the time to execute the third step of the clustering
 * algorithm measured in the 3 executions.
 * @param fourthStages Array containing the time to execute the fourth step of the clustering
 * algorithm measured in the 3 executions.
 * @param fifthStages Array containing the time to execute the fifth step of the clustering
 * algorithm measured in the 3 executions.
 * @param totalTimes Array containing the total times measured in the 3 executions.
 */
void printTimesToCsv(std::ofstream &csvFile,
                     std::size_t iteration,
                     const std::array<std::uint64_t, 3> &serviceTimes,
                     const std::array<std::uint64_t, 3> &firstStages,
                     const std::array<std::uint64_t, 3> &secondStages,
                     const std::array<std::uint64_t, 3> &thirdStages,
                     const std::array<std::uint64_t, 3> &fourthStages,
                     const std::array<std::uint64_t, 3> &fifthStages,
                     const std::array<std::uint64_t, 3> &totalTimes);

/**
 * Reads a number from the specified file input stream.
 *
 * @param inputFileStream File input stream from which the number will be read.
 * @param lineNumber Number of the current line. This method will take care of incrementing it by 1.
 * @return The read number.
 * @throws InvalidNumberException If some errors occur while parsing the read number.
 */
std::uint64_t readNumber(std::ifstream &inputFileStream, std::size_t &lineNumber);

/**
 * Prints the usage to the console.
 */
void usage();

/**
 * Main entrypoint of the application.
 *
 * @param argc Number of arguments specified in the command line.
 * @param argv Array containing the arguments specified in the command line.
 * @return The status code.
 */
int main(int argc, char *argv[]) {

    // Check if the help has been requested
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        usage();
        return 0;
    }

    // Check the number of arguments
    if (argc != 3) {
        std::cerr << "Wrong number of arguments." << std::endl << std::endl;
        usage();
        return 1;
    }

    // Check the existence of the input file
    std::filesystem::path inputFilePath{argv[1]};
    inputFilePath = absolute(inputFilePath).lexically_normal();

    if (!std::filesystem::is_regular_file(inputFilePath)) {
        std::cerr << "The input file '" << inputFilePath << "' does not exist." << std::endl;
        return 2;
    }

    // Open the file
    std::ifstream inputFileStream{argv[1]};
    if (inputFileStream) {
        // Clear the total times
        std::array<std::uint64_t, 3> serviceTimes{};
        serviceTimes.fill(UINTMAX_MAX);
        std::array<std::uint64_t, 3> firstStages{};
        firstStages.fill(UINTMAX_MAX);
        std::array<std::uint64_t, 3> secondStages{};
        secondStages.fill(UINTMAX_MAX);
        std::array<std::uint64_t, 3> thirdStages{};
        thirdStages.fill(UINTMAX_MAX);
        std::array<std::uint64_t, 3> fourthStages{};
        fourthStages.fill(UINTMAX_MAX);
        std::array<std::uint64_t, 3> fifthStages{};
        fifthStages.fill(UINTMAX_MAX);
        std::array<std::uint64_t, 3> totalTimes{};
        totalTimes.fill(UINTMAX_MAX);

        // Initialize the line number
        std::size_t lineNumber = 1;
        // Create an empty string to hold the separator line
        std::string line{};

        // Read the data of the three iterations
        for (std::size_t iteration = 0; iteration < 3; iteration++) {
            // Read the times
            try {
                serviceTimes[iteration] = readNumber(inputFileStream, lineNumber);
                firstStages[iteration] = readNumber(inputFileStream, lineNumber);
                secondStages[iteration] = readNumber(inputFileStream, lineNumber);
                thirdStages[iteration] = readNumber(inputFileStream, lineNumber);
                fourthStages[iteration] = readNumber(inputFileStream, lineNumber);
                fifthStages[iteration] = readNumber(inputFileStream, lineNumber);
                totalTimes[iteration] = readNumber(inputFileStream, lineNumber);

                // Read the separator line
                std::getline(inputFileStream, line);
                lineNumber++;
            } catch (std::exception &exception) {
                std::cerr << "Error while reading the data of the iteration" << ' ' << iteration
                          << ": " << exception.what() << std::endl;
            }
        }

        // Print the heading
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        std::cout << std::endl << "[!] Mean" << std::endl;
        std::cout << std::setfill(' ') << std::setw(7) << "Time";
        std::cout << "  |  ";
        std::cout << std::setfill(' ') << std::setw(44) << "Iteration 1";
        std::cout << "  |  ";
        std::cout << std::setfill(' ') << std::setw(44) << "Iteration 2";
        std::cout << "  |  ";
        std::cout << std::setfill(' ') << std::setw(44) << "Iteration3";
        std::cout << "  ||  ";
        std::cout << "Total" << std::endl;
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        // Print the times
        std::cout << "Service"
                  << "  |  ";
        printTimes(serviceTimes);
        std::cout << "Stage 1"
                  << "  |  ";
        printTimes(firstStages);
        std::cout << "Stage 2"
                  << "  |  ";
        printTimes(secondStages);
        std::cout << "Stage 3"
                  << "  |  ";
        printTimes(thirdStages);
        std::cout << "Stage 4"
                  << "  |  ";
        printTimes(fourthStages);
        std::cout << "Stage 5"
                  << "  |  ";
        printTimes(firstStages);
        std::cout << "Total"
                  << "    |  ";
        printTimes(totalTimes);
        std::cout << std::endl;

        // Open the CSV where to print the times
        std::filesystem::path csvFilePath{argv[2]};
        csvFilePath = absolute(csvFilePath).lexically_normal();
        std::ofstream csvFile{csvFilePath, std::ios::app};
        if (csvFile) {
            // Print the times to the CSV file
            for (std::size_t iteration = 0; iteration < 3; iteration++) {
                printTimesToCsv(csvFile,
                                iteration,
                                serviceTimes,
                                firstStages,
                                secondStages,
                                thirdStages,
                                fourthStages,
                                fifthStages,
                                totalTimes);
            }
            // Print two empty lines as separators
            csvFile << ",,,,,," << std::endl;
            csvFile << ",,,,,," << std::endl;
        } else {
            std::cerr << "Error while writing to the file '" << argv[2] << "'." << std::endl;
            return 3;
        }
    } else {
        std::cerr << "Error while reading the file '" << argv[1] << "'." << std::endl;
        return 2;
    }

    return 0;
}

/**
 * Prints to the console the specified duration in a human-readable format.
 *
 * @param durationNanoseconds Number of nanoseconds of the duration to print.
 */
void printDuration(const std::uint64_t durationNanoseconds) {

    using namespace std::literals::chrono_literals;

    // Check if some errors occurred while reading the number
    if (durationNanoseconds == UINTMAX_MAX) {
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        std::cout << std::setfill(' ') << std::setw(44) << "Error";
    } else {
        // Create a duration from the specified nanoseconds
        const auto duration = std::chrono::nanoseconds(durationNanoseconds);

        // Extract each part of the duration
        const auto nanoseconds = duration % 1us;
        const auto microseconds = (duration % 1ms) / 1us;
        const auto milliseconds = (duration % 1s) / 1ms;
        const auto seconds = (duration % 1min) / 1s;
        const auto minute = duration / 1min;

        // Print minutes, seconds and milliseconds
        std::cout << minute << " minutes " << std::setw(2) << std::setfill(' ') << seconds
                  << " seconds " << std::setw(3) << std::setfill(' ') << milliseconds << " ms (";

        // Print the milliseconds, properly padded
        if (milliseconds > 0) {
            std::cout << std::setw(3) << std::setfill(' ') << milliseconds;
            std::cout << ".";
        } else {
            std::cout << "    ";
        }
        // Print the microseconds, properly padded
        if (milliseconds > 0 || microseconds > 0) {
            const char fill = (milliseconds > 0) ? '0' : ' ';
            std::cout << std::setw(3) << std::setfill(fill) << microseconds;
            std::cout << ".";
        } else {
            std::cout << "    ";
        }
        // Print the nanoseconds, properly padded
        const char fill = (microseconds > 0) ? '0' : ' ';
        std::cout << std::setw(3) << std::setfill(fill) << nanoseconds.count();
        std::cout << " ns)";
    }
}

/**
 * Prints the specified measured times to the console.
 *
 * @param times Array containing the times measured in 3 executions of the
 * <code>main-measurements</code> application.
 */
void printTimes(const std::array<std::uint64_t, 3> &times) {

    // Print the times of the same iteration to the console
    printDuration(times[0]);
    std::cout << "  |  ";
    printDuration(times[1]);
    std::cout << "  |  ";
    printDuration(times[2]);
    std::cout << "  ||  ";
    printDuration((times[0] + times[1] + times[2]) / 3);
    std::cout << std::endl;
}

/**
 * Prints to the specified file, in CSV format, all the times measured in one execution of the
 * <code>main-measurements</code> application.
 *
 * @param csvFile File where the execution times will be printed to.
 * @param iteration Number of the execution whose times print to the specified file.
 * @param serviceTimes Array containing the service times measured in 3 executions.
 * @param firstStages Array containing the time to execute the first step of the clustering
 * algorithm measured in the 3 executions.
 * @param secondStages Array containing the time to execute the second step of the clustering
 * algorithm measured in the 3 executions.
 * @param thirdStages Array containing the time to execute the third step of the clustering
 * algorithm measured in the 3 executions.
 * @param fourthStages Array containing the time to execute the fourth step of the clustering
 * algorithm measured in the 3 executions.
 * @param fifthStages Array containing the time to execute the fifth step of the clustering
 * algorithm measured in the 3 executions.
 * @param totalTimes Array containing the total times measured in the 3 executions.
 */
void printTimesToCsv(std::ofstream &csvFile,
                     const std::size_t iteration,
                     const std::array<std::uint64_t, 3> &serviceTimes,
                     const std::array<std::uint64_t, 3> &firstStages,
                     const std::array<std::uint64_t, 3> &secondStages,
                     const std::array<std::uint64_t, 3> &thirdStages,
                     const std::array<std::uint64_t, 3> &fourthStages,
                     const std::array<std::uint64_t, 3> &fifthStages,
                     const std::array<std::uint64_t, 3> &totalTimes) {

    // Print the times, in CSV format, of the same iteration to the file
    csvFile << serviceTimes[iteration] << ',' << firstStages[iteration] << ','
            << secondStages[iteration] << ',' << thirdStages[iteration] << ','
            << fourthStages[iteration] << ',' << fifthStages[iteration] << ','
            << totalTimes[iteration] << std::endl;
}

/**
 * Reads a number from the specified file input stream.
 *
 * @param inputFileStream File input stream from which the number will be read.
 * @param lineNumber Number of the current line. This method will take care of incrementing it by 1.
 * @return The read number.
 * @throws InvalidNumberException If some errors occur while parsing the read number.
 */
std::uint64_t readNumber(std::ifstream &inputFileStream, size_t &lineNumber) {

    // Read a line
    std::string line{};
    if (!std::getline(inputFileStream, line)) {
        throw InvalidNumberException("Reached the end of file");
    }

    // Parse the read number
    try {
        // Parse the number
        std::size_t lastParsedCharacterIndex = 0;
        const std::uint64_t number = std::stoull(line, &lastParsedCharacterIndex);
        // Check if it has been correctly parsed
        if (lastParsedCharacterIndex == 0) {
            // Signals the error
            using namespace std::literals::string_literals;
            throw std::invalid_argument(
                    "Wrong number \""s + line + "\" at line" + ' ' + std::to_string(lineNumber));
        }
        // Move to the next line
        lineNumber++;

        // Return the parsed number
        return number;
    } catch (std::exception &) {
        // Signals the error
        using namespace std::literals::string_literals;
        throw std::invalid_argument(
                "Wrong number \""s + line + "\" at line" + ' ' + std::to_string(lineNumber));
    }
}

/**
 * Prints the usage to the console.
 */
void usage() {

    std::cout <<
            R""(SYNOPSYS
    main-mean-times-printer -h | --help
    main-mean-times-printer INPUT_FILE OUTPUT_CSV_FILE

DESCRIPTION
    Reads from the specified INPUT_FILE the times of 3 executions of the
    "main-measurements" application and computes the mean execution times.
    The computed mean times are written:
        - To the console in tabular form;
        - To the specified OUTPUT_CSV_FILE in CSV format. After the times, this
            application prints also two empty lines as separators.

OPTIONS
    -h, --help
        Prints this help.

EXIT CODE
    0   If the application, or the help has been requested.
    1   If a wrong number of arguments has been specified in the command line.
    2   If an error occurs while opening or reading from the specified
            INPUT_FILE.
    3   If an error occurs while opening or writing to the specified
            OUTPUT_CSV_FILE.

)"";
}
