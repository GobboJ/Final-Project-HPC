#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <algorithm>
#include <unordered_map>

bool readData(std::vector<double *> &data, std::size_t &dimension, const std::string &fileName) {
    std::ifstream file{fileName};
    std::string line{};

    if (file) {
        while (file) {
            std::getline(file, line);
            if (file.good() || file.eof()) {
                if (!line.empty()) {
                    dimension = static_cast<size_t>(std::count(line.cbegin(), line.cend(), ','));
                    auto *const element = new double[dimension];

                    std::size_t start = 0;
                    std::size_t length = 0;
                    std::size_t elementIndex = 0;
                    for (char character : line) {
                        if (character == ',') {
                            double value = std::strtod(line.substr(start, length).c_str(), nullptr);
                            start += length + 1;
                            length = 0;
                            element[elementIndex] = value;
                            elementIndex++;
                        } else {
                            length++;
                        }
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

    std::ofstream file{outputPath};
    if (file) {
        /*
         * Cluster[
         *      Cluster[1, 2, 0.71, 1, 1],
         *      Cluster[
         *          Cluster[
         *              Cluster[4, 6, 0.5, 1, 1], 5, 1, 2, 1
         *          ], 3, 1.41, 3, 1
         *      ], 2.5, 2, 4
         * ]
         *
         * ab = Cluster[1, 2, 0.71, 1, 1]
df = Cluster[4, 6, 0.5, 1, 1]
dfe = Cluster[df, 5, 1, 2, 1]
dfec = Cluster[dfe, 3, 1.41, 3, 1]
tot = Cluster[ab, dfec, 2.5, 2, 4]
DendrogramPlot[tot, LeafLabels ->(#&)]
         */
        std::vector<std::tuple<std::size_t, std::size_t, double>> o{};
        for (std::size_t i = 0; i < lambda.size(); i++) {
            o.emplace_back(std::make_tuple(i, pi[i], lambda[i]));
        }
        std::sort(o.begin(),
                  o.end(),
                  [](const std::tuple<std::size_t, std::size_t, double> &a,
                     const std::tuple<std::size_t, std::size_t, double> &b) -> bool {
                      return std::get<2>(a) < std::get<2>(b);
                  });

        std::unordered_map<std::size_t, std::pair<std::size_t, std::size_t>> p{};

        for (std::size_t i = 0; i < o.size() - 1; i++) {
            const auto &[index, pi, lambda] = o[i];

            std::size_t c1;
            std::size_t size1;
            bool found = false;
            for (const auto &[key, value] : p) {
                if (value.first == pi) {
                    found = true;
                    c1 = key;
                    size1 = value.second;
                    break;
                }
            }

            if (found) {
                // Found
                std::size_t c2;
                std::size_t size2;
                bool found = false;
                for (const auto &[key, value] : p) {
                
                }
                if (found) {
                    // Found
                    file << "c" << i << " = Cluster[c" << c1 << ", c" << c2 << ", " << lambda
                         << ", " << size1 << ", " << size2 << "]\n";
                    p[i] = {std::max(p[c1].first, p[c2].first), size1 + size2};
                } else {
                    // Not found
                    file << "c" << i << " = Cluster[c" << c1 << ", " << index + 1 << ", " << lambda
                         << ", " << size1 << ", 1]\n";
                    p[i] = {std::max(p[c1].first, pi), size1 + 1};
                }
            } else {
                // Not found
                file << "c" << i << " = Cluster[" << index + 1 << ", " << pi + 1 << ", " << lambda
                     << ", 1, 1]\n";
                p[i] = {pi, 1};
            }
        }

        file << "DendrogramPlot[c" << o.size() - 2 << ", LeafLabels ->(#&)]\n";
        return true;
    }
    std::cerr << "Unable to open output file." << std::endl;
    return false;
}

bool createMathematicaExpandedOutputFile(const std::filesystem::path &outputPath,
                                         const std::vector<std::size_t> &pi,
                                         const std::vector<double> &lambda) {

    std::ofstream file{outputPath};
    if (file) {
        /*
         * Cluster[
         *      Cluster[1, 2, 0.71, 1, 1],
         *      Cluster[
         *          Cluster[
         *              Cluster[4, 6, 0.5, 1, 1], 5, 1, 2, 1
         *          ], 3, 1.41, 3, 1
         *      ], 2.5, 2, 4
         * ]
         *
         * ab = Cluster[1, 2, 0.71, 1, 1]
df = Cluster[4, 6, 0.5, 1, 1]
dfe = Cluster[df, 5, 1, 2, 1]
dfec = Cluster[dfe, 3, 1.41, 3, 1]
tot = Cluster[ab, dfec, 2.5, 2, 4]
DendrogramPlot[tot, LeafLabels ->(#&)]
         */
        std::vector<std::tuple<std::size_t, std::size_t, double>> o{};
        for (std::size_t i = 0; i < lambda.size(); i++) {
            o.emplace_back(std::make_tuple(i, pi[i], lambda[i]));
        }
        std::sort(o.begin(),
                  o.end(),
                  [](const std::tuple<std::size_t, std::size_t, double> &a,
                     const std::tuple<std::size_t, std::size_t, double> &b) -> bool {
                      return std::get<2>(a) < std::get<2>(b);
                  });

        std::unordered_map<std::size_t, std::pair<std::size_t, std::size_t>> p{};
        std::unordered_map<std::size_t, std::string> clusters{};

        for (std::size_t i = 0; i < o.size() - 1; i++) {
            const auto &[index, pi, lambda] = o[i];

            std::size_t c1;
            std::size_t size1;
            bool found = false;
            for (const auto &[key, value] : p) {
                if (value.first == pi) {
                    found = true;
                    c1 = key;
                    size1 = value.second;
                    break;
                }
            }

            if (found) {
                // Found
                std::size_t c2;
                std::size_t size2;
                bool found = false;
                for (const auto &[key, value] : p) {
                    if (value.first == index) {
                        found = true;
                        c2 = key;
                        size2 = value.second;
                        break;
                    }
                }
                if (found) {
                    // Found
                    std::ostringstream stream{};
                    stream << "Cluster[" << clusters[c1] << ", " << clusters[c2] << ", " << lambda
                           << ", " << size1 << ", " << size2 << "]";
                    clusters[i] = stream.str();
                    p[i] = {std::max(p[c1].first, p[c2].first), size1 + size2};
                } else {
                    // Not found
                    std::ostringstream stream{};
                    stream << "Cluster[" << clusters[c1] << ", " << index + 1 << ", " << lambda << ", "
                           << size1 << ", 1]";
                    clusters[i] = stream.str();
                    p[i] = {std::max(p[c1].first, pi), size1 + 1};
                }
            } else {
                // Not found
                std::ostringstream stream{};
                stream << "Cluster[" << index + 1 << ", " << pi + 1 << ", " << lambda
                     << ", 1, 1]";
                clusters[i] = stream.str();
                p[i] = {pi, 1};
            }
        }
        
        auto a = clusters[20];
        file << clusters[o.size() - 2] << '\n';
        file << "DendrogramPlot[c" << o.size() - 2 << ", LeafLabels ->(#&)]\n";
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
int main() {

    std::vector<double *> data{};
    std::size_t dimension = 0;

    if (readData(data, dimension, "iris.data")) {
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

        std::filesystem::path outputDirectory{".."};
        outputDirectory =
                outputDirectory / ".." / ".." / "visualizer" / "DeB_Cluster_Visualizer" / "out";

        createOutputFile(outputDirectory / "out.txt", data, dimension, pi, lambda);
        createMathematicaOutputFile(outputDirectory / "mat.txt", pi, lambda);
        createMathematicaExpandedOutputFile(outputDirectory / "mat-ext.txt", pi, lambda);
    } else {
        return 1;
    }

    return 0;
}
