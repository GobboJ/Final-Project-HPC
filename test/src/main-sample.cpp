/*
 * Interactive application that illustrates how to use the library.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-09-14
 * @since 1.0
 */
#include "ParallelClustering.h"
#include "SequentialClustering.h"
#include <chrono>
#include <cstddef>
#include <random>

// Imports
using cluster::parallel::DistanceComputers;
using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;

/**
 * Small number used in the comparison between two double numbers.
 */
const constexpr double EPSILON = 0.00001;

/**
 * Main entrypoint of the example application.
 *
 * @return The exit code.
 */
int main() {

    // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
    std::size_t samplesCount;
    // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
    std::size_t attributesCount;
    // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
    std::size_t threadsCount;

    /*
     * Here you can put the code that retrieves the data samples to cluster.
     * In this example, the data samples are generated randomly.
     */

    // Read the number of data samples to generate
    std::cout << "Number of data samples:" << ' ';
    std::cin >> samplesCount;

    // Read the number of attributes of each data sample
    std::cout << "Number of attributes of each sample:" << ' ';
    std::cin >> attributesCount;

    // Read the number of threads to use in the parallel implementation
    std::cout << "Number of threads to use in the parallel implementation:" << ' ';
    std::cin >> threadsCount;
    if (threadsCount == 0) {
        std::cerr << "" << std::endl;
        return 1;
    }

    // Print a summary
    std::cout << "Running the example with:" << std::endl;
    std::cout << "    "
              << "- a randomly generated dataset of" << ' ' << samplesCount
              << " samples, each of which with" << ' ' << attributesCount << " attributes;"
              << std::endl;
    std::cout << "    "
              << "- " << threadsCount << " thread" << ((threadsCount == 1) ? " " : "s ")
              << "to use in the parallel implementation." << std::endl;

    // Initialize the random number generator
    auto seed = static_cast<std::uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::mt19937_64 randomNumberGenerator{seed};

    // Compute the size of the 32-aligned arrays that will hold the data samples to cluster
    std::size_t avxSamplesCount = ParallelClustering<>::computeAvxDimension(samplesCount);

    // Vector that will contain the data samples to cluster for the sequential implementation
    std::vector<double *> dataSamples{};
    // Vector that will contain the data samples to cluster for the parallel implementation
    std::vector<double *> alignedDataSamples{};

    // Generate the requested number of random samples
    for (std::size_t sampleIndex = 0; sampleIndex < samplesCount; sampleIndex++) {
        // Allocate the array  that holds the data sample
        auto *dataSample = new double[attributesCount];
        // Allocate the 32-bytes aligned array that holds the data sample
        auto *alignedDataSample = static_cast<double *>(
                _mm_malloc(avxSamplesCount * sizeof(double),
                           ParallelClustering<>::AVX_PACK_SIZE * sizeof(double)));
        memset(alignedDataSample, 0, avxSamplesCount * sizeof(double));

        // Generate the requested number of attributes
        for (std::size_t attributeIndex = 0; attributeIndex < attributesCount; attributeIndex++) {
            // Generate a random attribute fro 0 to 999
            auto randomAttribute = static_cast<double>(randomNumberGenerator() % 1000);
            // Store the attribute
            dataSample[attributeIndex] = randomAttribute;
            alignedDataSample[attributeIndex] = randomAttribute;
        }
        // Store the data sample
        dataSamples.push_back(dataSample);
        alignedDataSamples.push_back(alignedDataSample);
    }

    /*
     * Now, the data samples to cluster have been loaded, hence we can run the implementations of
     * the clustering algorithm.
     * In this example we run just two implementation, the sequential and one of the parallel.
     */

    // Create pi and lambda to use in the parallel implementation
    std::vector<std::size_t> parallelPiVector{};
    parallelPiVector.resize(samplesCount);
    std::vector<double> parallelLambdaVector;
    parallelLambdaVector.resize(samplesCount);

    auto parallelPiBegin = parallelPiVector.begin();
    auto parallelLambdaBegin = parallelLambdaVector.begin();

    /*
     * Execute the parallel implementation with:
     *     - Distance computation algorithm C: DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT;
     *     - Parallelization using threads of the computation of the distances PD: true;
     *     - Parallelization using threads of the rearrangement of the dendrogram PF: true;
     *     - Parallelization using thread of the computation of the square roots PS: true;
     *     - Check of the alignment of the data samples A: false. This is useful for debug purposes.
     */
    ParallelClustering<true, true, true, false>::cluster<
            DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(alignedDataSamples,
                                                             samplesCount,
                                                             attributesCount,
                                                             parallelPiBegin,
                                                             parallelLambdaBegin,
                                                             threadsCount,
                                                             threadsCount,
                                                             threadsCount);

    // Create pi and lambda to use in the sequential implementation
    std::vector<std::size_t> sequentialPiVector{};
    sequentialPiVector.resize(samplesCount);
    std::vector<double> sequentialLambdaVector;
    sequentialLambdaVector.resize(samplesCount);

    auto sequentialPiBegin = sequentialPiVector.begin();
    auto sequentialLambdaBegin = sequentialLambdaVector.begin();

    // Execute the sequential implementation
    SequentialClustering::cluster(
            dataSamples, samplesCount, attributesCount, sequentialPiBegin, sequentialLambdaBegin);

    /*
     * Now the parallelPiVector, parallelLambdaVector, sequentialPiVector and sequentialLambdaVector
     * vectors contain the pi and lambda values computed by the two implementations.
     * Therefore, we can use these results.
     * In this example we just compare the results, so to check that they are equal.
     */

    // Compare the values of pi
    bool piEqual = true;
    for (std::size_t i = 0; i < samplesCount; i++) {
        if (sequentialPiVector[i] != parallelPiVector[i]) {
            std::cerr << "Pi mismatch at index" << ' ' << i << std::endl;
            piEqual = false;
        }
    }
    if (piEqual) {
        std::cout << "All the values of pi are equal" << std::endl;
    }
    // Compare the values of lambda
    bool lambdaEqual = true;
    for (std::size_t i = 0; i < samplesCount; i++) {
        if (fabs(sequentialLambdaVector[i] - parallelLambdaVector[i]) > EPSILON) {
            std::cerr << "Lambda mismatch at index" << ' ' << i << std::endl;
            lambdaEqual = false;
        }
    }
    if (lambdaEqual) {
        std::cout << "All the values of lambda are equal" << std::endl;
    }

    return 0;
}
