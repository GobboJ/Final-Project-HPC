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
     * be 16 bytes-aligned (i.e., the memory address of the first attribute of each data sample must
     * be a multiple of 16).<br>
     * Moreover, the number of attributes of the data samples must be a multiple of
     * <code>ParallelClustering::SSE_PACK_SIZE</code>. If it is not the case, then the data samples
     * must be padded with <code>0</code> at the end until the number of attributes becomes a
     * multiple of <code>ParallelClustering::SSE_PACK_SIZE</code>.
     */
    SSE,

    /**
     * Computes the Euclidean distance between two data samples using AVX instructions.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 32 bytes-aligned (i.e., the memory address of the first attribute of each data sample must
     * be a multiple of 32).<br>
     * Moreover, the number of attributes of the data samples must be a multiple of
     * <code>ParallelClustering::AVX_PACK_SIZE</code>. If it is not the case, then the data samples
     * must be padded with <code>0</code> at the end until the number of attributes becomes a
     * multiple of <code>ParallelClustering::AVX_PACK_SIZE</code>.
     */
    AVX,

    /**
     * Computes the Euclidean distance between two data samples using SSE instructions.<br>
     * With respect to the <code>DistanceComputers::SSE</code> algorithm, this one does not
     * store any partial sum into memory, but keeps them in the registers.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 16 bytes-aligned (i.e., the memory address of the first attribute of each data sample must
     * be a multiple of 16).<br>
     * Moreover, the number of attributes of the data samples must be a multiple of
     * <code>ParallelClustering::SSE_PACK_SIZE</code>. If it is not the case, then the data samples
     * must be padded with <code>0</code> at the end until the number of attributes becomes a
     * multiple of <code>ParallelClustering::SSE_PACK_SIZE</code>.
     */
    SSE_OPTIMIZED,

    /**
     * Computes the Euclidean distance between two data samples using AVX instructions.<br>
     * With respect to the <code>DistanceComputers::AVX</code> algorithm, this one does not
     * store any partial sum into memory, but keeps them in the registers.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 32 bytes-aligned (i.e., the memory address of the first attribute of each data sample must
     * be a multiple of 32).<br>
     * Moreover, the number of attributes of the data samples must be a multiple of
     * <code>ParallelClustering::AVX_PACK_SIZE</code>. If it is not the case, then the data samples
     * must be padded with <code>0</code> at the end until the number of attributes becomes a
     * multiple of <code>ParallelClustering::AVX_PACK_SIZE</code>.
     */
    AVX_OPTIMIZED,

    /**
     * Computes the Euclidean distance between two data samples using SSE instructions.<br>
     * Like the <code>DistanceComputers::SSE_OPTIMIZED</code> algorithm, this one does not
     * store any partial sum into memory, but keeps them in the registers.<br>
     * Moreover, this algorithm computes the squares of the distances, avoiding the computation
     * of the square roots.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 16 bytes-aligned (i.e., the memory address of the first attribute of each data sample must
     * be a multiple of 16).<br>
     * Moreover, the number of attributes of the data samples must be a multiple of
     * <code>ParallelClustering::SSE_PACK_SIZE</code>. If it is not the case, then the data samples
     * must be padded with <code>0</code> at the end until the number of attributes becomes a
     * multiple of <code>ParallelClustering::SSE_PACK_SIZE</code>.
     */
    SSE_OPTIMIZED_NO_SQUARE_ROOT,

    /**
     * Computes the Euclidean distance between two data samples using AVX instructions.<br>
     * Like the <code>DistanceComputers::AVX_OPTIMIZED</code> algorithm, this one does not
     * store any partial sum into memory, but keeps them in the registers.<br>
     * Moreover, this algorithm computes the squares of the distances, avoiding the computation
     * of the square roots.<br>
     * The attributes of each data sample <b>MUST BE</b> stored sequentially in memory, and must
     * be 32 bytes-aligned (i.e., the memory address of the first attribute of each data sample must
     * be a multiple of 32).<br>
     * Moreover, the number of attributes of the data samples must be a multiple of
     * <code>ParallelClustering::AVX_PACK_SIZE</code>. If it is not the case, then the data samples
     * must be padded with <code>0</code> at the end until the number of attributes becomes a
     * multiple of <code>ParallelClustering::AVX_PACK_SIZE</code>.
     */
    AVX_OPTIMIZED_NO_SQUARE_ROOT
};
}  // namespace cluster::parallel
#endif  // FINAL_PROJECT_HPC_DISTANCECOMPUTERS_H
