/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-23
 * @since version date
 */
#include <fstream>
#include <stdlib.h>
#include <filesystem>
#include <iostream>
#include <random>

bool parseSizeT(char *string, std::size_t &result);

std::size_t computeNumberDigits(std::size_t number);

void usage();

int main(int argc, char *argv[]) {

    if (argc < 3) {
        std::cerr << "Too few arguments." << std::endl;
        usage();
        exit(1);
    }

    //  std::cout << "data-generator ROWS ATTRIBUTES [FILEPATH]" << std::endl;
    std::size_t rowsCount = 0;
    if (!parseSizeT(argv[1], rowsCount)) {
        std::cerr << "Wrong rows count." << std::endl;
        usage();
        exit(1);
    }
    std::size_t attributesCount = 0;
    if (!parseSizeT(argv[2], attributesCount)) {
        std::cerr << "Wrong attributes count." << std::endl;
        usage();
        exit(1);
    }
    std::filesystem::path outputPath{".."};
    if (argc == 4) {
        outputPath = argv[3];
    } else {
        outputPath = outputPath / ".." / "test" / "resources" / "generated.data";
    }
    std::size_t minValue = 0;
    std::size_t maxValue = 100;
    if (argc > 5) {
        if (argc == 6) {
            if (!parseSizeT(argv[4], minValue)) {
                std::cerr << "Wrong min value." << std::endl;
                usage();
                exit(1);
            }
            if (!parseSizeT(argv[5], maxValue)) {
                std::cerr << "Wrong max value." << std::endl;
                usage();
                exit(1);
            }
        } else {
            std::cerr << "Wrong min and max arguments." << std::endl;
            usage();
            exit(1);
        }
    }

    std::cout << "Generating file '"
              << std::filesystem::absolute(outputPath).lexically_normal().string() << "' with"
              << ' ' << rowsCount << " rows and" << ' ' << attributesCount
              << " attributes, using values from" << ' ' << minValue << " and" << ' ' << maxValue
              << std::endl;

    std::size_t rowsCountLength = computeNumberDigits(rowsCount);
    std::cout << "Generated 0 / " << rowsCount << " rows";
    std::cout.flush();
    std::size_t lastPrintedRowNumber = 0;

    auto seed = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch()
                                           .count());
    std::mt19937_64 randomNumberGenerator{seed};
    
    std::ofstream file{outputPath};
    
    for (std::size_t rowNumber = 0; rowNumber < rowsCount; rowNumber++) {
        for (std::size_t attributeNumber = 0; attributeNumber < attributesCount;
             attributeNumber++) {
            
            uint64_t number = randomNumberGenerator();
            
            
            file << minValue + (number % (maxValue - minValue));
            if (!file) {
                std::cout << std::endl;
                std::cerr << "Error while writing the file at row" << ' ' << rowNumber << std::endl;
                return 1;
            }
            if (attributeNumber != attributesCount - 1) {
                file << ",";
                if (!file) {
                    std::cout << std::endl;
                    std::cerr << "Error while writing the file at row" << ' ' << rowNumber
                              << std::endl;
                    return 1;
                }
            }
        }
        file << '\n';
        if (!file) {
            std::cout << std::endl;
            std::cerr << "Error while writing the file at row" << ' ' << rowNumber << std::endl;
            return 1;
        }

        if (rowNumber % 1000 == 999) {
            std::size_t lastRowNumberLength = computeNumberDigits(lastPrintedRowNumber);
            std::cout << "\033[" << (5 + rowsCountLength + 3 + lastRowNumberLength) << "D"
                      << rowNumber + 1 << " / " << rowsCount << " rows";
            std::cout.flush();
            lastPrintedRowNumber = rowNumber + 1;
        }
    }
    std::size_t lastRowNumberLength = computeNumberDigits(lastPrintedRowNumber);
    std::cout << "\033[" << (5 + rowsCountLength + 3 + lastRowNumberLength) << "D" << rowsCount
              << " / " << rowsCount << " rows";
    std::cout.flush();
    std::cout << std::endl << "Generation terminated successfully" << std::endl;

    return 0;
}

bool parseSizeT(char *string, std::size_t &result) {

    char *nextNonParsedIndex = string;
    std::size_t convertedValue = std::strtoul(string, &nextNonParsedIndex, 10);

    if (nextNonParsedIndex == string) {
        return false;
    }

    result = convertedValue;
    return true;
}

std::size_t computeNumberDigits(std::size_t number) {

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

    std::cout << "data-generator ROWS ATTRIBUTES [FILEPATH] [MIN_VALUE MAX_VALUE]" << std::endl;
}
