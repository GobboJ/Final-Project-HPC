/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-25
 * @since version date
 */
#include "CliArgumentsParser.h"
#include <filesystem>
#include <iostream>

[[gnu::pure]] bool CliArgumentsParser::CliArguments::isTestEnabled() const {
    return testEnabled;
}

void CliArgumentsParser::CliArguments::setTestEnabled(bool testEnabled) {
    CliArguments::testEnabled = testEnabled;
}

[[gnu::pure]] bool CliArgumentsParser::CliArguments::isParallel() const {
    return parallel;
}

void CliArgumentsParser::CliArguments::setParallel(bool parallel) {
    CliArguments::parallel = parallel;
}

[[gnu::pure]] std::size_t CliArgumentsParser::CliArguments::getParallelVersion() const {
    return parallelVersion;
}

void CliArgumentsParser::CliArguments::setParallelVersion(std::size_t parallelVersion) {
    CliArguments::parallelVersion = parallelVersion;
}

[[gnu::pure]] std::size_t CliArgumentsParser::CliArguments::getStartColumnIndex() const {
    return startColumnIndex;
}

void CliArgumentsParser::CliArguments::setStartColumnIndex(std::size_t startColumnIndex) {
    CliArguments::startColumnIndex = startColumnIndex;
}

[[gnu::pure]] std::size_t CliArgumentsParser::CliArguments::getEndColumnIndex() const {
    return endColumnIndex;
}

void CliArgumentsParser::CliArguments::setEndColumnIndex(std::size_t endColumnIndex) {
    CliArguments::endColumnIndex = endColumnIndex;
}

[[gnu::pure]] const std::filesystem::path &CliArgumentsParser::CliArguments::getFilePath() const {
    return filePath;
}

void CliArgumentsParser::CliArguments::setFilePath(const std::filesystem::path &filePath) {
    CliArguments::filePath = filePath;
}

bool CliArgumentsParser::CliArguments::isOutputEnabled() const {
    return outputEnabled;
}

void CliArgumentsParser::CliArguments::setOutputEnabled(bool outputEnabled) {
    CliArguments::outputEnabled = outputEnabled;
}

const std::unordered_map<std::string, std::tuple<std::string, std::size_t, std::size_t>>
        CliArgumentsParser::TESTS{// Sequential :  6 s 362.728.561 ns
                                  // Parallel v1:  3 s 984.201.053 ns
                                  // Parallel v2:  3 s 910.884.877 ns
                                  // Parallel v3:  3 s 932.660.255 ns
                                  {"birm", {"Parking Birmingham.data", 2, 3}},
                                  {"slide", {"slide.data", 1, 2}},
                                  // Sequential: 122.690 ns
                                  {"iris", {"iris.data", 1, 4}},
                                  // Sequential :  2 m 10 s 448.206.284 ns
                                  // Parallel v1:  1 m 44 s 508.938.089 ns
                                  // Parallel v2:  1 m 38 s 773.972.452 ns
                                  // Parallel v3:  1 m 33 s 123.307.274 ns
                                  {"acc", {"accelerometer.csv", 3, 5}},
                                  {"gen", {"generated.data", 1, 32}},
                                  {"two-points", {"two-points.data", 1, 2}},
                                  {"samples", {"samples.data", 1, 2}},
                                  {"same-distance", {"same-distance.data", 1, 2}},
                                  {"slide", {"slide.data", 1, 2}}};

CliArgumentsParser::CliArguments CliArgumentsParser::parseArguments(int argc, char *argv[]) {

    CliArguments result{};

    std::size_t nextOptionIndex = CliArgumentsParser::parseTestOption(argc, argv, 1, result);
    nextOptionIndex = CliArgumentsParser::parseOutputOption(argc, argv, nextOptionIndex, result);
    nextOptionIndex = CliArgumentsParser::parseParallelOption(argc, argv, nextOptionIndex, result);
    CliArgumentsParser::parseFileName(argc, argv, nextOptionIndex, result);

    return result;
}

std::size_t CliArgumentsParser::parseTestOption(
        int argc, char *argv[], std::size_t nextOptionIndex, CliArguments &result) {

    if (argc <= nextOptionIndex) {
        std::cerr << "Error while parsing -t option: Too few arguments." << std::endl;
        usage();
        exit(1);
    }

    std::string testOption{argv[nextOptionIndex]};

    if (testOption == "-t") {
        result.setTestEnabled(true);
        return nextOptionIndex + 1;
    }

    result.setTestEnabled(false);
    return nextOptionIndex;
}

std::size_t CliArgumentsParser::parseOutputOption(int argc,
                                                  char **argv,
                                                  std::size_t nextOptionIndex,
                                                  CliArgumentsParser::CliArguments &result) {
    if (argc <= nextOptionIndex) {
        std::cerr << "Error while parsing -o option: Too few arguments." << std::endl;
        usage();
        exit(1);
    }
    
    std::string option{argv[nextOptionIndex]};

    if (option == "-o") {
        result.setOutputEnabled(true);
        return nextOptionIndex + 1;
    }

    result.setOutputEnabled(false);
    return nextOptionIndex;
}



std::size_t CliArgumentsParser::parseParallelOption(int argc,
                                                    char **argv,
                                                    std::size_t nextOptionIndex,
                                                    CliArgumentsParser::CliArguments &result) {

    if (argc <= nextOptionIndex) {
        std::cerr << "Unable to parse options -p/-s: Too few arguments." << std::endl;
        usage();
        exit(1);
    }

    std::string option{argv[nextOptionIndex]};
    nextOptionIndex++;
    std::size_t version = 0;

    if (option == "-p") {
        if (argc <= nextOptionIndex) {
            std::cerr << "Too few arguments for option -p." << std::endl;
            usage();
            return 1;
        }
        if (!parseSizeT(argv[nextOptionIndex], version)) {
            std::cerr << "Wrong version number for option -p." << std::endl;
            usage();
            return 1;
        }
        result.setParallel(true);
        result.setParallelVersion(version);
        return nextOptionIndex + 1;
    }
    if (option == "-s") {
        result.setParallel(false);
        return nextOptionIndex;
    }

    std::cerr << "Unknown option" << ' ' << option << std::endl;
    usage();
    exit(1);
}

std::size_t CliArgumentsParser::parseFileName(int argc,
                                              char **argv,
                                              std::size_t nextOptionIndex,
                                              CliArgumentsParser::CliArguments &result) {

    if (argc <= nextOptionIndex) {
        std::cerr << "Unable to parse file name: Too few arguments." << std::endl;
        usage();
        exit(1);
    }

    std::string fileName{argv[nextOptionIndex]};
    nextOptionIndex++;

    std::size_t startColumnIndex = 0;
    std::size_t endColumnIndex = 0;

    if (TESTS.contains(fileName)) {
        std::tuple<std::string, std::size_t, std::size_t> test = TESTS.at(fileName);
        fileName = std::get<0>(test);
        startColumnIndex = std::get<1>(test);
        endColumnIndex = std::get<2>(test);
    } else {
        if (argc <= nextOptionIndex + 2) {
            std::cerr << "Too few arguments for start and end" << std::endl;
            usage();
            exit(1);
        }

        if (!parseSizeT(argv[nextOptionIndex], startColumnIndex)) {
            std::cerr << "Wrong start column index" << std::endl;
            usage();
            exit(1);
        }
        if (!parseSizeT(argv[nextOptionIndex + 1], endColumnIndex)) {
            std::cerr << "Wrong end column index" << std::endl;
            usage();
            exit(1);
        }

        if (endColumnIndex < startColumnIndex) {
            std::cerr << "Wrong start and end column index" << std::endl;
            exit(1);
        }
        nextOptionIndex += 2;
    }

    std::filesystem::path resourcesPath{".."};
    resourcesPath = resourcesPath / ".." / "test" / "resources";

    result.setFilePath(resourcesPath / fileName);
    result.setStartColumnIndex(startColumnIndex - 1);
    result.setEndColumnIndex(endColumnIndex - 1);

    return nextOptionIndex;
}

bool CliArgumentsParser::parseSizeT(char *string, std::size_t &result) {

    char *nextNonParsedIndex = string;
    std::size_t convertedValue = std::strtoul(string, &nextNonParsedIndex, 10);

    if (nextNonParsedIndex == string) {
        return false;
    }

    result = convertedValue;
    return true;
}

void CliArgumentsParser::usage() {

    std::cout << "Usage" << std::endl;
    std::cout << "main [-t] [-o] -s FILENAME [START END] " << std::endl;
    std::cout << "main [-t] [-o] -p VERSION FILENAME [START END] " << std::endl;
}
