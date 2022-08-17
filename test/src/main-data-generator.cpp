/*
 * Generator generating random data samples.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1.1 2022-08-16
 * @since 1.0
 */
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

/**
 * Utility function that extracts a <code>std::size_t</code> from a C string.
 *
 * @param string String to parse.
 * @param result Reference to the variable where the parsed value will be placed.
 * @return <code>true</code> if the the string has been successfully parsed, <code>false</code>
 * otherwise.
 */
bool parseSizeT(char *string, std::size_t &result);

/**
 * Utility function that computes the number of digits of the specified number.
 *
 * @param number Number to compute the number of digits of.
 * @return The computed number of digits.
 */
std::size_t computeNumberDigits(std::size_t number);

/**
 * Prints the usage and exits.
 */
void usage();

/**
 * Main entrypoint of the application.
 *
 * @param argc Number of arguments specified in the command line.
 * @param argv Array containing the arguments specified in the command line.
 * @return The exit code.
 */
int main(int argc, char *argv[]) {

    // Check if the help option has been specified
    if (argc == 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        usage();
        return 0;
    }

    // Check the number of arguments
    if (argc < 3) {
        std::cerr << "Too few arguments." << std::endl;
        usage();
        return 1;
    }

    // Extract the number of samples to generate
    std::size_t samplesCount = 0;
    if (!parseSizeT(argv[1], samplesCount)) {
        std::cerr << "Wrong samples count." << std::endl;
        usage();
        return 1;
    }

    // Extract the number of attributes of each generated sample
    std::size_t attributesCount = 0;
    if (!parseSizeT(argv[2], attributesCount)) {
        std::cerr << "Wrong attributes count." << std::endl;
        usage();
        return 1;
    }
    // Extract the output path, if specified
    std::filesystem::path outputPath{".."};
    if (argc == 4) {
        outputPath = argv[3];
    } else {
        outputPath = outputPath / ".." / "test" / "resources" / "generated.data";
    }
    // Check the existence of the enclosing directory
    std::filesystem::path absolutePath = absolute(outputPath).lexically_normal();
    if (!exists(absolutePath.parent_path())) {
        std::cerr << "The directory" << ' ' << absolutePath.string()
                  << " where the generated file will be placed does not exist." << std::endl;
        usage();
        return 1;
    }

    // Extract the minimum and maximum values for the generated attributes
    std::size_t minValue = 0;
    std::size_t maxValue = 100;
    if (argc > 5) {
        if (argc == 6) {
            if (!parseSizeT(argv[4], minValue)) {
                std::cerr << "Wrong min value." << std::endl;
                usage();
                return 1;
            }
            if (!parseSizeT(argv[5], maxValue)) {
                std::cerr << "Wrong max value." << std::endl;
                usage();
                return 1;
            }
        } else {
            std::cerr << "Wrong min and max arguments." << std::endl;
            usage();
            return 1;
        }
    }

    // Print the summary of the generation
    std::cout << "Generating file '"
              << std::filesystem::absolute(outputPath).lexically_normal().string()
              << "' with:" << std::endl;
    std::cout << "    " << samplesCount << " data samples" << std::endl;
    std::cout << "    " << attributesCount << " attributes" << std::endl;
    std::cout << "    "
              << "using values from" << ' ' << minValue << " to" << ' ' << maxValue << std::endl;
    // Print the initial progress
    std::size_t rowsCountLength = computeNumberDigits(samplesCount);
    std::cout << "Generated 0 /" << ' ' << samplesCount << " rows";
    std::cout.flush();
    std::size_t lastPrintedRowNumber = 0;

    // Initialize the random number generator
    auto seed = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::mt19937_64 randomNumberGenerator{seed};

    // Open the output file
    std::ofstream file{outputPath};
    if (!file) {
        std::cerr << "Error while opening the file" << ' ' << absolutePath.string() << std::endl;
        return 2;
    }

    // Generate the requested number of samples
    for (std::size_t rowNumber = 0; rowNumber < samplesCount; rowNumber++) {
        // Generate the requested number of attributes
        for (std::size_t attributeNumber = 0; attributeNumber < attributesCount;
             attributeNumber++) {

            // Write a random number in the range
            uint64_t number = randomNumberGenerator();
            file << minValue + (number % (maxValue - minValue));
            if (!file) {
                std::cout << std::endl;
                std::cerr << "Error while writing the file at row" << ' ' << rowNumber << std::endl;
                return 2;
            }
            // Put a comma between two consecutive data samples
            if (attributeNumber != attributesCount - 1) {
                file << ",";
                if (!file) {
                    std::cout << std::endl;
                    std::cerr << "Error while writing the file at row" << ' ' << rowNumber
                              << std::endl;
                    return 2;
                }
            }
        }
        // Terminate the line
        file << std::endl;
        if (!file) {
            std::cout << std::endl;
            std::cerr << "Error while writing the file at row" << ' ' << rowNumber << std::endl;
            return 2;
        }

        // Update the progress
        if (rowNumber % 1000 == 999) {
            std::size_t lastRowNumberLength = computeNumberDigits(lastPrintedRowNumber);
            std::cout << "\033[" << (5 + rowsCountLength + 3 + lastRowNumberLength) << "D"
                      << rowNumber + 1 << " / " << samplesCount << " rows";
            std::cout.flush();
            lastPrintedRowNumber = rowNumber + 1;
        }
    }
    // Print the last progress
    std::size_t lastRowNumberLength = computeNumberDigits(lastPrintedRowNumber);
    std::cout << "\033[" << (5 + rowsCountLength + 3 + lastRowNumberLength) << "D" << samplesCount
              << " / " << samplesCount << " rows";
    std::cout.flush();
    std::cout << std::endl << "Generation terminated successfully" << std::endl;

    return 0;
}

/**
 * Utility function that extracts a <code>std::size_t</code> from a C string.
 *
 * @param string String to parse.
 * @param result Reference to the variable where the parsed value will be placed.
 * @return <code>true</code> if the the string has been successfully parsed, <code>false</code>
 * otherwise.
 */
bool parseSizeT(char *string, std::size_t &result) {

    char *nextNonParsedIndex = string;
    std::size_t convertedValue = std::strtoul(string, &nextNonParsedIndex, 10);

    if (nextNonParsedIndex == string) {
        return false;
    }

    result = convertedValue;
    return true;
}

/**
 * Utility function that computes the number of digits of the specified number.
 *
 * @param number Number to compute the number of digits of.
 * @return The computed number of digits.
 */
[[gnu::const]] std::size_t computeNumberDigits(std::size_t number) {

    if (number == 0) {
        return 1;
    }
    std::size_t digits = 0;
    while (number > 0) {
        digits++;
        number /= 10;
    }

    return digits;
}

void usage() {

    std::cout <<
            R""(SYNOPSYS
    data-generator -h | --help
    data-generator SAMPLES ATTRIBUTES [FILE_PATH] [MIN_VALUE MAX_VALUE]
    
DESCRIPTION
    Generates a random dataset with SAMPLES data samples, each with ATTRIBUTES
    attributes.
    The dataset is stored in the specified FILE_PATH file, overwriting it if
    already exists. If the FILE_PATH argument is not specified, then the dataset
    is stored in the '../../test/resources/generated.data' file.
    The values of the generated attributes range in the interval
    [MIN_VALUE; MAX_VALUE). If those two values are not specified, then they
    default to 0 and 100 respectively.

OPTIONS
    -h, --help
        Prints this help.

EXIT CODE
    0   If the file is generated successfully.
    1   If the specified arguments are not correct.
    2   If some errors occur while generating the file.

)"";
}
