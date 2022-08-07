/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-07
 * @since version date
 */
#ifndef FINAL_PROJECT_HPC_DISTANCECOMPUTERS_H
#define FINAL_PROJECT_HPC_DISTANCECOMPUTERS_H

namespace cluster::parallel {
/**
 * Enumeration of all the algorithms that can be used to compute the distance between two
 * samples of the dataset.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2 2022-07-30
 * @since 1.0
 */
enum class DistanceComputers {

    /**
     * Computes the Euclidean distance between two data samples without using any SIMD
     * instructions.
     */
    CLASSICAL,

    /**
     * Computes the Euclidean distance between two data samples using SSE instructions.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 16 bytes-aligned.<br>
     * If the number of attributes <i>d</i> of each sample is not multiple of <code>16 /
     * sizeof(double)</code>, then the <i>d</i> attributes <b>MUST BE</b> followed by as
     * many <code>0</code> as needed to reach the closes multiple of <code>16 /
     * sizeof(double)</code> number of elements.
     */
    SSE,

    /**
     * Computes the Euclidean distance between two data samples using AVX instructions.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 32 bytes-aligned.<br>
     * If the number of attributes <i>d</i> of each sample is not multiple of <code>32 /
     * sizeof(double)</code>, then the <i>d</i> attributes <b>MUST BE</b> followed by as
     * many <code>0</code> as needed to reach the closes multiple of <code>32 /
     * sizeof(double)</code> number of elements.
     */
    AVX,

    /**
     * Computes the Euclidean distance between two data samples using SSE instructions.<br>
     * With respect to the <code>DistanceComputers::SSE</code> algorithm, this one does not
     * store any partial sum into memory, but keeps them in the registers.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 16 bytes-aligned.<br>
     * If the number of attributes <i>d</i> of each sample is not multiple of <code>16 /
     * sizeof(double)</code>, then the <i>d</i> attributes <b>MUST BE</b> followed by as
     * many <code>0</code> as needed to reach the closes multiple of <code>16 /
     * sizeof(double)</code> number of elements.
     */
    SSE_OPTIMIZED,

    /**
     * Computes the Euclidean distance between two data samples using AVX instructions.<br>
     * With respect to the <code>DistanceComputers::AVX</code> algorithm, this one does not
     * store any partial sum into memory, but keeps them in the registers.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 32 bytes-aligned.<br>
     * If the number of attributes <i>d</i> of each sample is not multiple of <code>32 /
     * sizeof(double)</code>, then the <i>d</i> attributes <b>MUST BE</b> followed by as
     * many <code>0</code> as needed to reach the closes multiple of <code>32 /
     * sizeof(double)</code> number of elements.
     */
    AVX_OPTIMIZED,

    /**
     * Computes the Euclidean distance between two data samples using SSE instructions.<br>
     * Like the <code>DistanceComputers::SSE_OPTIMIZED</code> algorithm, this one does not
     * store any partial sum into memory, but keeps them in the registers.<br>
     * Moreover, this algorithm computes the squares of the distances, avoiding the computation
     * of the square roots.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 16 bytes-aligned.<br>
     * If the number of attributes <i>d</i> of each sample is not multiple of <code>16 /
     * sizeof(double)</code>, then the <i>d</i> attributes <b>MUST BE</b> followed by as
     * many <code>0</code> as needed to reach the closes multiple of <code>16 /
     * sizeof(double)</code> number of elements.
     */
    SSE_OPTIMIZED_NO_SQUARE_ROOT,

    /**
     * Computes the Euclidean distance between two data samples using AVX instructions.<br>
     * Like the <code>DistanceComputers::AVX_OPTIMIZED</code> algorithm, this one does not
     * store any partial sum into memory, but keeps them in the registers.<br>
     * Moreover, this algorithm computes the squares of the distances, avoiding the computation
     * of the square roots.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 32 bytes-aligned.<br>
     * If the number of attributes <i>d</i> of each sample is not multiple of <code>32 /
     * sizeof(double)</code>, then the <i>d</i> attributes <b>MUST BE</b> followed by as
     * many <code>0</code> as needed to reach the closes multiple of <code>32 /
     * sizeof(double)</code> number of elements.
     */
    AVX_OPTIMIZED_NO_SQUARE_ROOT
};
}  // namespace cluster::parallel
#endif  // FINAL_PROJECT_HPC_DISTANCECOMPUTERS_H
