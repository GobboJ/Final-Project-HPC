/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-01
 * @since version date
 */
#ifndef FINAL_PROJECT_HPC_SIMDUTILS_H
#define FINAL_PROJECT_HPC_SIMDUTILS_H
#include <cstddef>

class [[deprecated]] SimdUtils {

public:
    static const constexpr std::size_t SSE_PACK_SIZE = 2;
    static const constexpr std::size_t AVX_PACK_SIZE = 4;

    static inline std::size_t computeSseBlocksCount(std::size_t dimension) {

        return 1 + ((dimension - 1) / SSE_PACK_SIZE);
    }

    static inline std::size_t computeAvxBlocksCount(std::size_t dimension) {

        return 1 + ((dimension - 1) / AVX_PACK_SIZE);
    }
};

#endif  // FINAL_PROJECT_HPC_SIMDUTILS_H
