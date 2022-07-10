#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <algorithm>
#include <unordered_map>

bool readData(std::vector<std::pair<double, double>> &data, const std::string &fileName) {
    std::ifstream file{fileName};
    std::string line{};

    if (file) {
        while (file) {
            std::getline(file, line);
            if (file.good() || file.eof()) {
                if (!line.empty()) {
                    std::size_t firstCommaIndex = line.find_first_of(',');
                    std::size_t secondCommaIndex = line.find_first_of(',', firstCommaIndex + 1);

                    double x = std::strtod(line.substr(0, firstCommaIndex).c_str(), nullptr);
                    double y = std::strtod(
                            line.substr(firstCommaIndex + 1, secondCommaIndex).c_str(), nullptr);
                    data.emplace_back(x, y);
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

double distance(
        std::vector<std::pair<double, double>> &data, std::size_t first, std::size_t second) {

    const auto [firstValueX, firstValueY] = data[first];
    const auto [secondValueX, secondValueY] = data[second];
    const double xDifference = firstValueX - secondValueX;
    const double yDifference = firstValueY - secondValueY;

    return sqrt(xDifference * xDifference + yDifference * yDifference);
}

bool createOutputFile(const std::filesystem::path &outputPath,
                      const std::vector<std::pair<double, double>> &data,
                      const std::vector<std::size_t> &pi,
                      const std::vector<double> &lambda) {

    std::ofstream file{outputPath};
    if (file) {
        int index = 0;
        char letter = '1';//TODO: 'A';
        std::unordered_map<std::string, std::string> names{};
        // Print the points
        for (const auto &[x1, x2] : data) {
            std::string s{"P"};
            s += std::to_string(index);
            file << s << ": \"" << letter << "\" " << x1 << ' ' << x2 << '\n';
            names[s] = letter;
            letter++;
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
                                 const std::vector<std::pair<double, double>> &data,
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
                    if (value.first == index) {
                        found = true;
                        c2 = key;
                        size2 = value.second;
                        break;
                    }
                }
                if (found) {
                    // Found
                    file << "c" << i << " = Cluster[c" << c1 << ", c" << c2 << ", " << lambda << ", " << size1 << ", " << size2 << "]\n";
                    p[i] = { std::max(p[c1].first, p[c2].first), size1 + size2};
                } else {
                    // Not found
                    file << "c" << i << " = Cluster[c" << c1 << ", " << index << ", " << lambda << ", " << size1 << ", 1]\n";
                    p[i] = { std::max(p[c1].first, pi), size1 + 1};
                }
            } else {
                // Not found
                file << "c" << i << " = Cluster[" << index << ", " << pi << ", " << lambda
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

/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
int main() {

    std::vector<std::pair<double, double>> d = {{1, 1}, {1.5, 1.5}};
    auto a = distance(d, 0, 1);

    std::vector<std::pair<double, double>> data{};

    if (readData(data, "a.data")) {
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
                m[i] = distance(data, i, n);
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
        // TODO: creare output file
        std::filesystem::path outputDirectory{".."};
        outputDirectory =
                outputDirectory / ".." / ".." / "visualizer" / "DeB_Cluster_Visualizer" / "out";

        /*data.resize(10);
        for (std::size_t i = 0; i < 10; i++) {
            data[i] = {i, i};
        }
        pi = {1, 3, 3, 9, 5, 9, 9, 9, 9, 9};
        lambda = {1.2, 3.4, 2.1, 4.1, 1.9, 4.2, 3.6, 2.9, 1.4, INFINITY};
    */
        createOutputFile(outputDirectory / "out.txt", data, pi, lambda);
        createMathematicaOutputFile(outputDirectory / "mat.txt", data, pi, lambda);
    } else {
        return 1;
    }

    return 0;
}
