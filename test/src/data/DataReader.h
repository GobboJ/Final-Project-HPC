#ifndef FINAL_PROJECT_HPC_DATAREADER_H
#define FINAL_PROJECT_HPC_DATAREADER_H

#include <string>
#include <vector>
#include <filesystem>

/**
 * Reader that reads the data of the points to cluster from a file.
 *
 * @author DeB, Jonathan
 * @version 1.1 2022-07-18
 * @since 1.0
 */
class DataReader {

public:
    /**
     * Reads the data of the points to cluster from a file.
     *
     * @param startColumnIndex Index, starting from <code>0</code>, of the first column in the input
     * file that contains the first coordinate of the points.
     * @param endColumnIndex Index, starting from <code>0</code>, of the last column in the input
     * file that contains the last coordinate of the points.
     * @param fileName Name of the file that contains the data.
     * @return The read data.
     * @throws MalformedFileException If the file does not follow the format.
     */
    static std::vector<double *> readData(std::size_t startColumnIndex,
                                          std::size_t endColumnIndex,
                                          const std::filesystem::path &inputPath);
};

#endif  // FINAL_PROJECT_HPC_DATAREADER_H
