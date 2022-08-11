/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-11
 * @since version date
 */
#ifndef FINAL_PROJECT_HPC_ALIGNMENTS_H
#define FINAL_PROJECT_HPC_ALIGNMENTS_H

#include <cstddef>

namespace cluster::test::types {
class Alignments {

public:
    static const constexpr std::size_t SSE_ALIGNMENT = 16;
    static const constexpr std::size_t AVX_ALIGNMENT = 32;
};
}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_ALIGNMENTS_H
