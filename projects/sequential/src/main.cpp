#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <algorithm>
#include <unordered_map>

/**
 *
 * @param data
 * @param startColumnIndex Included.
 * @param endColumnIndex Included.
 * @param fileName
 * @return
 */
bool readData(std::vector<double *> &data,
              const std::size_t startColumnIndex,
              const std::size_t endColumnIndex,
              const std::string &fileName) {
    std::ifstream file{fileName};
    std::string line{};

    if (file) {
        while (file) {
            std::getline(file, line);
            if (file.good() || file.eof()) {
                if (!line.empty()) {
                    auto *const element = new double[endColumnIndex - startColumnIndex + 1];

                    std::size_t start = 0;
                    std::size_t length = 0;
                    std::size_t elementIndex = 0;
                    std::size_t columnIndex = 0;
                    
                    std::string::const_iterator iterator = line.cbegin();
                    while (iterator != line.end() && columnIndex <= endColumnIndex) {
                        if (*iterator == ',') {
                            if (columnIndex >= startColumnIndex) {
                                double value =
                                        std::strtod(line.substr(start, length).c_str(), nullptr);
                                element[elementIndex] = value;
                                elementIndex++;
                            }
                            start += length + 1;
                            length = 0;
                            columnIndex++;
                        } else {
                            length++;
                        }
                        ++iterator;
                    }
                    if (start < line.length()) {
                        element[elementIndex] =
                                std::strtod(line.substr(start, line.length()).c_str(), nullptr);
                    }

                    data.emplace_back(element);
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

double distance(const double *__restrict__ const firstPoint,
                const double *__restrict__ const secondPoint,
                const std::size_t dimension) {

    double sum = 0;
    for (std::size_t i = 0; i < dimension; i++) {
        sum += pow(firstPoint[i] - secondPoint[i], 2.0);
    }

    return sqrt(sum);
}

bool createOutputFile(const std::filesystem::path &outputPath,
                      const std::vector<double *> &data,
                      const std::size_t &dimension,
                      const std::vector<std::size_t> &pi,
                      const std::vector<double> &lambda) {

    std::ofstream file{outputPath};
    if (file) {
        int index = 0;
        std::size_t pointIndex = 1;
        std::unordered_map<std::string, std::string> names{};
        // Print the points
        for (const double *element : data) {
            std::string s{"P"};
            s += std::to_string(index);
            file << s << ": \"" << std::to_string(pointIndex) << "\" ";
            for (std::size_t i = 0; i < dimension; i++) {
                file << element[i];
                if (i == (dimension - 1)) {
                    file << '\n';
                } else {
                    file << ' ';
                }
            }
            names[s] = std::to_string(pointIndex);
            pointIndex++;
            index++;
        }
        // Merge pi and lambda
        std::vector<std::tuple<std::size_t, std::size_t, double>> result{};
        result.resize(pi.size());
        for (std::size_t i = 0; i < pi.size(); i++) {
            result[i] = std::make_tuple(i, pi[i], lambda[i]);
        }
        // Order by lambda
        std::sort(result.begin(),
                  result.end(),
                  [](const std::tuple<std::size_t, std::size_t, double> &first,
                     const std::tuple<std::size_t, std::size_t, double> &second) -> bool {
                      return std::get<2>(first) < std::get<2>(second);
                  });

        std::unordered_map<std::size_t, std::pair<std::string, std::string>> clusters{};
        for (std::size_t i = 0; i < pi.size(); i++) {
            std::string s{"P"};
            s += std::to_string(i);
            clusters[i] = {s, names[s]};
        }

        std::size_t clusterIndex = 0;
        for (const auto &[firstIndex, secondIndex, distance] : result) {
            if (firstIndex != secondIndex) {
                std::string clusterName{"C"};
                clusterName += std::to_string(clusterIndex);
                const auto &[firstClusterIndex, firstName] = clusters[firstIndex];
                const auto &[secondClusterIndex, secondName] = clusters[secondIndex];

                std::string fullClusterName = firstName;
                fullClusterName += secondName;

                file << clusterName << ": \"" << fullClusterName << "\" " << firstClusterIndex
                     << ' ' << secondClusterIndex << ' ' << distance << '\n';
                clusters[firstIndex] = {clusterName, fullClusterName};
                clusters[secondIndex] = {clusterName, fullClusterName};

                clusterIndex++;
            }
        }
        file.close();

        return true;
    }

    std::cerr << "Unable to open output file." << std::endl;
    return false;
}

bool createMathematicaOutputFile(const std::filesystem::path &outputPath,
                                  const std::vector<std::size_t> &pi,
                                  const std::vector<double> &lambda) {

    using namespace std::literals::string_literals;
    std::ofstream file{outputPath};
    if (file) {
        
        std::vector<std::tuple<std::size_t, std::size_t, double>> ordered{};
        for (std::size_t i = 0; i < lambda.size(); i++) {
            ordered.emplace_back(std::make_tuple(i, pi[i], lambda[i]));
        }
        std::sort(ordered.begin(), ordered.end(), [](const auto &a, const auto &b) -> bool {
            return std::get<2>(a) < std::get<2>(b);
        });

        std::unordered_map<std::size_t, std::pair<std::string, std::size_t>> map{};
        for (std::size_t i = 0; i < ordered.size(); i++) {
            map[i] = {std::to_string(i + 1), 1};
        }

        for (std::size_t i = 0; i < ordered.size() - 1; i++) {
            const auto &[index, pi, lambda] = ordered[i];
            file << "c" << i << " = Cluster[" << map[index].first << ", " << map[pi].first << ", "
                 << lambda << ", " << map[index].second << ", " << map[index].second << "]\n";
            map[pi].first = "c"s + std::to_string(i);
            map[pi].second += map[index].second;
            map[index].first = "c"s + std::to_string(i);
            map[index].second += map[pi].second;
        }
        file << "DendrogramPlot[c" << ordered.size() - 2 << ", LeafLabels ->(#&)]\n";

        return true;
    }

    std::cerr << "Unable to open output file." << std::endl;
    return false;
}

/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
int main(int argc, char *argv[]) {
    
    if (argc < 4) {
        std::cerr << "Too few arguments" << std::endl;
        return 1;
    }
    std::string fileName{argv[1]};
    char *nextNonParsedIndex = &(argv[2][0]);
    unsigned long startColumnIndex = std::strtoul(argv[2], &nextNonParsedIndex, 10);
    if (nextNonParsedIndex == &(argv[2][0])) {
        std::cerr << "Wrong start column index" << std::endl;
        return 1;
    }
    nextNonParsedIndex = &(argv[3][0]);
    unsigned long endColumnIndex = std::strtoul(argv[3], &nextNonParsedIndex, 10);
    if (nextNonParsedIndex == &(argv[3][0])) {
        std::cerr << "Wrong end column index" << std::endl;
        return 1;
    }
    
    if (endColumnIndex < startColumnIndex) {
        std::cerr << "Wrong start and end column index" << std::endl;
        return 1;
    }
    
    const unsigned long dimension = endColumnIndex - startColumnIndex;
    
    std::vector<double *> data{};

    if (readData(data, startColumnIndex-1, endColumnIndex-1, fileName)) {

        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::size_t> pi{};
        pi.resize(data.size());

        std::vector<double> lambda{};
        lambda.resize(data.size());
        
        std::vector<double> m{};
        m.resize(data.size());
        
        
        
        for (ssize_t n = 0; n < data.size(); n++) {
            pi[n] = n;
            lambda[n] = INFINITY;

            for (ssize_t i = 0; i <= n - 1; i++) {
                m[i] = distance(data[i], data[n], dimension);
            }

            for (ssize_t i = 0; i <= n - 1; i++) {
                if (lambda[i] >= m[i]) {
                    m[pi[i]] = std::min(m[pi[i]], lambda[i]);
                    lambda[i] = m[i];
                    pi[i] = n;
                }
                if (lambda[i] < m[i]) {
                    m[pi[i]] = std::min(m[pi[i]], m[i]);
                }
            }

            for (ssize_t i = 0; i <= n - 1; i++) {
                if (lambda[i] >= lambda[pi[i]]) {
                    pi[i] = n;
                }
            }
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = end - start;

        using namespace std::literals::chrono_literals;

        auto nanoseconds = duration % 1us;
        auto microseconds = (duration % 1ms) / 1us;
        auto milliseconds = (duration % 1s) / 1ms;
        auto seconds = (duration % 1min) / 1s;
        auto minute = duration / 1min;

        std::cout << minute << " minutes " << seconds << " seconds " << milliseconds << " ms ("
                  << milliseconds << "." << microseconds << "." << nanoseconds.count() << " ns)"
                  << std::endl;

        std::filesystem::path outputDirectory{".."};
        outputDirectory =
                outputDirectory / ".." / ".." / "visualizer" / "DeB_Cluster_Visualizer" / "out";

        createOutputFile(outputDirectory / "out.txt", data, dimension, pi, lambda);
        createMathematicaOutputFile(outputDirectory / "mat.txt", pi, lambda);
    } else {
        return 1;
    }

    return 0;
}
