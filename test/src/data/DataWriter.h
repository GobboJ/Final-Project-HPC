#ifndef FINAL_PROJECT_HPC_DATAWRITER_H
#define FINAL_PROJECT_HPC_DATAWRITER_H

#include <vector>
#include <filesystem>

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-18
 * @since version date
 */
class DataWriter {

public:
    static bool createOutputFile(const std::filesystem::path &outputPath,
                                 const std::vector<double *> &data,
                                 const std::size_t &dimension,
                                 const std::vector<std::size_t> &pi,
                                 const std::vector<double> &lambda);

    static bool createMathematicaOutputFile(const std::filesystem::path &outputPath,
                                            const std::vector<std::size_t> &pi,
                                            const std::vector<double> &lambda);
};

#endif  // FINAL_PROJECT_HPC_DATAWRITER_H
