#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <iomanip>
#include <chrono>
#include <filesystem>

void printDuration(std::uint64_t durationNumber) {
    
    using namespace std::literals::chrono_literals;
    
    auto duration = std::chrono::nanoseconds(durationNumber);
    
    // Extract each part of the duration
    auto nanoseconds = duration % 1us;
    auto microseconds = (duration % 1ms) / 1us;
    auto milliseconds = (duration % 1s) / 1ms;
    auto seconds = (duration % 1min) / 1s;
    auto minute = duration / 1min;

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
        char fill = (milliseconds > 0) ? '0' : ' ';
        std::cout << std::setw(3) << std::setfill(fill) << microseconds;
        std::cout << ".";
    } else {
        std::cout << "    ";
    }
    // Print the nanoseconds, properly padded
    char fill = (microseconds > 0) ? '0' : ' ';
    std::cout << std::setw(3) << std::setfill(fill) << nanoseconds.count();

    // Clear the remaining part of the line
    std::cout << " ns)" << std::endl;
}

std::uint64_t readFromFile(std::ifstream &inputFileStream, std::size_t &lineNumber) {

    std::string line{};
    std::getline(inputFileStream, line);
    
    std::size_t last = 0;
    
    try {
        std::uint64_t number = std::stoull(line, &last);
        if (last == 0) {
            using namespace std::literals::string_literals;
            throw std::invalid_argument("Wrong number \""s + line + "\" at line " + std::to_string(lineNumber));
        }
        
        lineNumber++;
        return number;
    } catch (std::exception &e) {
        using namespace std::literals::string_literals;
        throw std::invalid_argument("Wrong number \""s + line + "\" at line " + std::to_string(lineNumber));
    }
}

/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-09-10
 * @since version date
 */
int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Too few arguments." << std::endl << std::endl;
        std::cerr << "Usage: main FILE" << std::endl;
        return 1;
    }

    if (!std::filesystem::is_regular_file(argv[1])) {
        std::cerr << "The path " << argv[1] << " does not exist" << std::endl;
    }
    
    std::string line{};
    std::ifstream inputFileStream{argv[1]};
    
    if (inputFileStream) {
        std::uint64_t serviceTime = 0;
        std::uint64_t firstStage = 0;
        std::uint64_t secondStage = 0;
        std::uint64_t thirdStage = 0;
        std::uint64_t fourthStage = 0;
        std::uint64_t fifthStage = 0;
        std::uint64_t total = 0;
        
        std::size_t lineNumber = 1;

        for (int iteration = 0; iteration < 3; iteration++) {
            serviceTime += readFromFile(inputFileStream, lineNumber);
            firstStage += readFromFile(inputFileStream, lineNumber);
            secondStage += readFromFile(inputFileStream, lineNumber);
            thirdStage += readFromFile(inputFileStream, lineNumber);
            fourthStage += readFromFile(inputFileStream, lineNumber);
            fifthStage += readFromFile(inputFileStream, lineNumber);
            total += readFromFile(inputFileStream, lineNumber);
            std::getline(inputFileStream, line);
            lineNumber++;
        }

        std::cout << std::endl << "[!] Mean" << std::endl;
        std::cout << "Service:" << ' ';
        printDuration(serviceTime / 3);
        std::cout << "Stage 1:" << ' ';
        printDuration(firstStage / 3);
        std::cout << "Stage 2:" << ' ';
        printDuration(secondStage / 3);
        std::cout << "Stage 3:" << ' ';
        printDuration(thirdStage / 3);
        std::cout << "Stage 4:" << ' ';
        printDuration(fourthStage / 3);
        std::cout << "Stage 5:" << ' ';
        printDuration(fifthStage / 3);
        std::cout << "Total  : ";
        printDuration(total / 3);
        std::cout << std::endl;
    } else {
        std::cerr << "Error while reading the file " << argv[1] << '.' << std::endl;
        return 2;
    }
}
