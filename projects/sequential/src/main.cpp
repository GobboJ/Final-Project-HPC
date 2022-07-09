#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>

bool readData(std::vector<double> &data, const std::string &fileName) {
    std::ifstream file{fileName};
    std::string line{};

    if (file) {
        while (file) {
            std::getline(file, line);
            if (file.good() || file.eof()) {
                if (!line.empty()) {
                    std::size_t commaIndex = line.find_first_of(',');

                    double number = std::strtod(line.substr(0, commaIndex).c_str(), nullptr);
                    data.push_back(number);
                }
            } else {
                std::cerr << "Error while reading the file." << std::endl;
                return false;
            }
        }
    } else {
        std::cerr << "Unable to open file." << std::endl;
        return false;
    }
    file.close();
    return true;
}

/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
int main() {

    std::vector<double> data{};

    if (readData(data, "iris.data")) {
    
    } else {
        return 1;
    }

    return 0;
}
