/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-18
 * @since version date
 */
#include "DataWriter.h"
#include <fstream>
#include <unordered_map>
#include <iostream>

bool DataWriter::createOutputFile(const std::filesystem::path &outputPath,
                                  const std::vector<double *> &data,
                                  const std::size_t &dimension,
                                  const std::vector<size_t> &pi,
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
        std::vector<std::size_t> result{};
        result.resize(pi.size());
        for (std::size_t i = 0; i < pi.size(); i++) {
            result[i] = i;
        }
        // Order by lambda
        std::sort(result.begin(),
                  result.end(),
                  [&lambda](const std::size_t &first, const std::size_t &second) -> bool {
                      return lambda[first] < lambda[second];
                  });

        std::unordered_map<std::size_t, std::pair<std::string, std::string>> clusters{};
        for (std::size_t i = 0; i < pi.size(); i++) {
            std::string s{"P"};
            s += std::to_string(i);
            clusters[i] = {s, names[s]};
        }

        std::size_t clusterIndex = 0;
        for (const auto index : result) {
            if (index != pi[index]) {
                std::string clusterName{"C"};
                clusterName += std::to_string(clusterIndex);
                const auto &[firstClusterIndex, firstName] = clusters[index];
                const auto &[secondClusterIndex, secondName] = clusters[pi[index]];

                std::string fullClusterName = firstName;
                fullClusterName += secondName;
                if (fullClusterName.length() > 10) {
                    fullClusterName = fullClusterName.substr(0, 7) + "...";
                }

                file << clusterName << ": \"" << fullClusterName << "\" " << firstClusterIndex
                     << ' ' << secondClusterIndex << ' ' << lambda[index] << '\n';
                clusters[index] = {clusterName, fullClusterName};
                clusters[pi[index]] = {clusterName, fullClusterName};

                clusterIndex++;
            }
        }
        file.close();

        return true;
    }

    std::cerr << "Unable to open output file." << std::endl;
    return false;
}

bool DataWriter::createMathematicaOutputFile(const std::filesystem::path &outputPath,
                                             const std::vector<size_t> &pi,
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

        file << "Needs[\"HierarchicalClustering`\"]\n";
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
