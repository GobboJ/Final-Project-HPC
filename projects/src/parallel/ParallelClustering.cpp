/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-18
 * @since version date
 */
#include "../../include/parallel/ParallelClustering.h"
#include "../utils/Timer.h"
#include <cmath>
#include <atomic>

//#include <pmmintrin.h>
//#include <xmmintrin.h>
#include <immintrin.h>

void ParallelClustering::clusterV1(const std::vector<double *> &data,
                                   std::size_t dimension,
                                   std::vector<std::size_t> &pi,
                                   std::vector<double> &lambda) noexcept {
    
    std::size_t dataSize = data.size();
    
    // Initializes pi and lambda vectors
    pi.resize(dataSize);
    lambda.resize(dataSize);

    // Initializes the distance matrix
    std::vector<double> partRow{};
    partRow.resize(dataSize);

    // 1. Set pi(0) to 0, lambda(0) to infinity
    pi[0] = 0;
    lambda[0] = std::numeric_limits<double>::infinity();
    
#ifdef TIMERS
    Timer firstTimer{};
    Timer secondTimer{};
    Timer thirdTimer{};
    Timer fourthTimer{};
#endif

    for (std::size_t n = 1; n < dataSize; n++) {
    
#ifdef TIMERS
        firstTimer.start();
#endif

        // 1. Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
        pi[n] = n;
        lambda[n] = std::numeric_limits<double>::infinity();
        
#ifdef TIMERS
        firstTimer.stop();
        
        secondTimer.start();
#endif

        // 2. Set M(i) to d(i, n + 1) for i = 1,..,n

#pragma omp parallel for default(none) shared(n, data, dimension, partRow) \
        num_threads(ParallelClustering::DISTANCE_PARALLEL_THREADS_COUNT)
        for (std::size_t i = 0; i <= n - 1; i++) {
            partRow[i] = ParallelClustering::distance(data[i], data[n], dimension);
        }
        
#ifdef TIMERS
        secondTimer.stop();
        
        thirdTimer.start();
#endif

        // 3. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= M(i)
            if (lambda[i] >= partRow[i]) {
                // set M(pi(i)) to min { M(pi(i)), lambda(i) }
                partRow[pi[i]] = std::min(partRow[pi[i]], lambda[i]);
                // set lambda(i) to M(i)
                lambda[i] = partRow[i];
                // set pi(i) to n + 1
                pi[i] = n;
            }
            // if lambda(i) < M(i)
            if (lambda[i] < partRow[i]) {
                // set M(pi(i)) to min { M(pi(i)), M(i) }
                partRow[pi[i]] = std::min(partRow[pi[i]], partRow[i]);
            }
        }
        
#ifdef TIMERS
        thirdTimer.stop();
        
        fourthTimer.start();
#endif

        // 4. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= lambda(pi(i))
            if (lambda[i] >= lambda[pi[i]]) {
                // set pi(i) to n + 1
                pi[i] = n;
            }
        }
        
#ifdef TIMERS
        fourthTimer.stop();
#endif
#ifdef PRINT_ITERATIONS
        if (n % 1000 == 0) {
            std::cout << n << std::endl;
        }
#endif
    }
    
#ifdef TIMERS
    std::cout << "Stage 1: ";
    firstTimer.print();
    std::cout << "Stage 2: ";
    secondTimer.print();
    std::cout << "Stage 3: ";
    thirdTimer.print();
    std::cout << "Stage 4: ";
    fourthTimer.print();
    std::cout << "Total  : ";
    (firstTimer + secondTimer + thirdTimer + fourthTimer).print();
#endif
}

void ParallelClustering::clusterV2(const std::vector<double *> &data,
                                   std::size_t dimension,
                                   std::vector<std::size_t> &pi,
                                   std::vector<double> &lambda) noexcept {
    
    std::size_t dataSize = data.size();
    
    // Initializes pi and lambda vectors
    pi.resize(dataSize);
    lambda.resize(dataSize);

    // Initializes the distance matrix
    std::vector<double> m{};
    m.resize(dataSize);

    // 1. Set pi(0) to 0, lambda(0) to infinity
    pi[0] = 0;
    lambda[0] = std::numeric_limits<double>::infinity();
    
#ifdef TIMERS
    Timer firstTimer{};
    Timer secondTimer{};
    Timer thirdTimer{};
    Timer fourthTimer{};
#endif

    for (std::size_t n = 1; n < dataSize; n++) {
    
#ifdef TIMERS
        firstTimer.start();
#endif

        // 1. Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
        pi[n] = n;
        lambda[n] = std::numeric_limits<double>::infinity();
        
#ifdef TIMERS
        firstTimer.stop();
        
        secondTimer.start();
#endif

        // 2. Set M(i) to d(i, n + 1) for i = 1,..,n

        std::size_t quotient = 1 + ((dimension - 1) / 4);
        
        

#pragma omp parallel for default(none) shared(n, data, dimension, m, quotient) \
        num_threads(ParallelClustering::DISTANCE_PARALLEL_THREADS_COUNT)
        for (std::size_t i = 0; i <= n - 1; i++) {
            double sum = 0;
            for (std::size_t j = 0; j < quotient; j++) {
                __m128d dataI = _mm_load_pd(data[i]);  // data[i]{0, 1}
                __m128d dataN = _mm_load_pd(data[n]);  // data[n]{0, 1}
                
                __m128d sub = _mm_sub_pd(dataI, dataN);
                __m128d square = _mm_mul_pd(sub, sub);
                
                sum += _mm_hadd_pd(square, square)[0];
            }
            m[i] = sqrt(sum);
        }
        
#ifdef TIMERS
        secondTimer.stop();
        
        thirdTimer.start();
#endif

        // 3. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= M(i)
            if (lambda[i] >= m[i]) {
                // set M(pi(i)) to min { M(pi(i)), lambda(i) }
                m[pi[i]] = std::min(m[pi[i]], lambda[i]);
                // set lambda(i) to M(i)
                lambda[i] = m[i];
                // set pi(i) to n + 1
                pi[i] = n;
            }
            // if lambda(i) < M(i)
            if (lambda[i] < m[i]) {
                // set M(pi(i)) to min { M(pi(i)), M(i) }
                m[pi[i]] = std::min(m[pi[i]], m[i]);
            }
        }
        
#ifdef TIMERS
        thirdTimer.stop();
        
        fourthTimer.start();
#endif

        // 4. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= lambda(pi(i))
            if (lambda[i] >= lambda[pi[i]]) {
                // set pi(i) to n + 1
                pi[i] = n;
            }
        }
        
#ifdef TIMERS
        fourthTimer.stop();
#endif
#ifdef PRINT_ITERATIONS
        if (n % 1000 == 0) {
            std::cout << n << std::endl;
        }
#endif
    }
    
#ifdef TIMERS
    std::cout << "Stage 1: ";
    firstTimer.print();
    std::cout << "Stage 2: ";
    secondTimer.print();
    std::cout << "Stage 3: ";
    thirdTimer.print();
    std::cout << "Stage 4: ";
    fourthTimer.print();
    std::cout << "Total  : ";
    (firstTimer + secondTimer + thirdTimer + fourthTimer).print();
#endif
}

void ParallelClustering::clusterV3(const std::vector<double *> &data,
                                   std::size_t dimension,
                                   std::vector<std::size_t> &pi,
                                   std::vector<double> &lambda) noexcept {
    
    std::size_t dataSize = data.size();
    
    // Initializes pi and lambda vectors
    pi.resize(dataSize);
    lambda.resize(dataSize);

    // Initializes the distance matrix
    std::vector<double> m{};
    m.resize(dataSize);

    // 1. Set pi(0) to 0, lambda(0) to infinity
    pi[0] = 0;
    lambda[0] = std::numeric_limits<double>::infinity();
    
#ifdef TIMERS
    Timer firstTimer{};
    Timer secondTimer{};
    Timer thirdTimer{};
    Timer fourthTimer{};
#endif

    for (std::size_t n = 1; n < dataSize; n++) {
    
#ifdef TIMERS
        firstTimer.start();
#endif

        // 1. Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
        pi[n] = n;
        lambda[n] = std::numeric_limits<double>::infinity();
        
#ifdef TIMERS
        firstTimer.stop();
        
        secondTimer.start();
#endif

        // 2. Set M(i) to d(i, n + 1) for i = 1,..,n

        std::size_t quotient = 1 + ((dimension - 1) / 4);

#pragma omp parallel for default(none) shared(n, data, dimension, m, quotient) \
        num_threads(ParallelClustering::DISTANCE_PARALLEL_THREADS_COUNT)
        for (std::size_t i = 0; i <= n - 1; i++) {
            double sum = 0;
            for (std::size_t j = 0; j < quotient; j++) {
                __m256d dataI = _mm256_load_pd(&(data[i][j * 4]));
                __m256d dataN = _mm256_load_pd(&(data[n][j * 4]));

                __m256d sub = _mm256_sub_pd(dataI, dataN);
                __m256d square = _mm256_mul_pd(sub, sub);  // x^2, y^2, z^2, a^2
                sum += _mm256_hadd_pd(square, square)[0];  // (x^2 + y^2 + z^2 + a^2) ...
            }
            // {3, 6, 8, 10, 15, 18, 0, 0}
            /*
            __m128d dataI = _mm_load_pd(data[i]);  // data[i]{0, 1}
            __m128d dataN = _mm_load_pd(data[n]);  // data[n]{0, 1}

            __m128d sub = _mm_sub_pd(dataI, dataN);
            __m128d square = _mm_mul_pd(sub, sub);
            __m128d result = _mm_hadd_pd(square, square);


            _mm_store_sd(&sum, result);
            */
            m[i] = sqrt(sum);

            // partRow[i] = ParallelClustering::distance(data[i], data[n], dimension);
        }
        
#ifdef TIMERS
        secondTimer.stop();
        
        thirdTimer.start();
#endif

        // 3. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= M(i)
            if (lambda[i] >= m[i]) {
                // set M(pi(i)) to min { M(pi(i)), lambda(i) }
                m[pi[i]] = std::min(m[pi[i]], lambda[i]);
                // set lambda(i) to M(i)
                lambda[i] = m[i];
                // set pi(i) to n + 1
                pi[i] = n;
            }
            // if lambda(i) < M(i)
            if (lambda[i] < m[i]) {
                // set M(pi(i)) to min { M(pi(i)), M(i) }
                m[pi[i]] = std::min(m[pi[i]], m[i]);
            }
        }

#ifdef TIMERS
        thirdTimer.stop();
        
        fourthTimer.start();
#endif
        
        // 4. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= lambda(pi(i))
            if (lambda[i] >= lambda[pi[i]]) {
                // set pi(i) to n + 1
                pi[i] = n;
            }
        }
        
#ifdef TIMERS
        fourthTimer.stop();
#endif
#ifdef PRINT_ITERATIONS
        if (n % 1000 == 0) {
            std::cout << n << std::endl;
        }
#endif
    }
    
#ifdef TIMERS
    std::cout << "Stage 1: ";
    firstTimer.print();
    std::cout << "Stage 2: ";
    secondTimer.print();
    std::cout << "Stage 3: ";
    thirdTimer.print();
    std::cout << "Stage 4: ";
    fourthTimer.print();
    std::cout << "Total  : ";
    (firstTimer + secondTimer + thirdTimer + fourthTimer).print();
#endif
}

void ParallelClustering::clusterV4(double *data,
                                   const std::size_t dimension,
                                   const std::size_t dataSize,
                                   std::vector<std::size_t> &pi,
                                   std::vector<double> &lambda) noexcept {

    // Initializes pi and lambda vectors
    pi.resize(dataSize);
    lambda.resize(dataSize);

    // Initializes the distance matrix
    std::vector<double> m{};
    m.resize(dataSize);

    // 1. Set pi(0) to 0, lambda(0) to infinity
    pi[0] = 0;
    lambda[0] = std::numeric_limits<double>::infinity();

#ifdef TIMERS
    Timer firstTimer{};
    Timer secondTimer{};
    Timer thirdTimer{};
    Timer fourthTimer{};
#endif
    for (std::size_t n = 1; n < dataSize; n++) {
#ifdef TIMERS
        firstTimer.start();
#endif
        // 1. Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
        pi[n] = n;
        lambda[n] = std::numeric_limits<double>::infinity();
#ifdef TIMERS
        firstTimer.stop();

        secondTimer.start();
#endif
        // 2. Set M(i) to d(i, n + 1) for i = 1,..,n

        std::size_t quotient = 1 + ((dimension - 1) / 4);
        std::size_t singleDataSize = 4 * quotient;

#pragma omp parallel for default(none) shared(n, data, dimension, m, quotient, singleDataSize) \
        num_threads(ParallelClustering::DISTANCE_PARALLEL_THREADS_COUNT)
        for (std::size_t i = 0; i <= n - 1; i++) {
            double sum = 0;
            for (std::size_t j = 0; j < quotient; j++) {
                // dimension = 6
                // data.size = 8
                // {c11, c12, c13, c14, c21, c22, c23, c23}
                // i = 0 --> 0 - 7
                // i = 1 --> 8 - 15
                // i = 2 --> 16 - 23
                // i = 3 --> 24 - 31
                __m256d dataI = _mm256_load_pd(&(data[(i * singleDataSize) + (j * 4)]));
                __m256d dataN = _mm256_load_pd(&(data[(n * singleDataSize) + (j * 4)]));

                __m256d sub = _mm256_sub_pd(dataI, dataN);
                __m256d square = _mm256_mul_pd(sub, sub);  // x^2, y^2, z^2, a^2
                sum += _mm256_hadd_pd(square, square)[0];  // (x^2 + y^2 + z^2 + a^2) ...
            }
            // {3, 6, 8, 10, 15, 18, 0, 0}
            /*
            __m128d dataI = _mm_load_pd(data[i]);  // data[i]{0, 1}
            __m128d dataN = _mm_load_pd(data[n]);  // data[n]{0, 1}

            __m128d sub = _mm_sub_pd(dataI, dataN);
            __m128d square = _mm_mul_pd(sub, sub);
            __m128d result = _mm_hadd_pd(square, square);


            _mm_store_sd(&sum, result);
            */
            m[i] = sqrt(sum);

            // partRow[i] = ParallelClustering::distance(data[i], data[n], dimension);
        }
#ifdef TIMERS
        secondTimer.stop();

        thirdTimer.start();
#endif

        // 3. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= M(i)
            if (lambda[i] >= m[i]) {
                // set M(pi(i)) to min { M(pi(i)), lambda(i) }
                m[pi[i]] = std::min(m[pi[i]], lambda[i]);
                // set lambda(i) to M(i)
                lambda[i] = m[i];
                // set pi(i) to n + 1
                pi[i] = n;
            }
            // if lambda(i) < M(i)
            if (lambda[i] < m[i]) {
                // set M(pi(i)) to min { M(pi(i)), M(i) }
                m[pi[i]] = std::min(m[pi[i]], m[i]);
            }
        }
#ifdef TIMERS
        thirdTimer.stop();

        fourthTimer.start();
#endif
        // 4. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= lambda(pi(i))
            if (lambda[i] >= lambda[pi[i]]) {
                // set pi(i) to n + 1
                pi[i] = n;
            }
        }
#ifdef TIMERS
        fourthTimer.stop();
#endif
#ifdef PRINT_ITERATIONS
        if (n % 1000 == 0) {
            std::cout << n << std::endl;
        }
#endif
    }

#ifdef TIMERS
    std::cout << "Stage 1: ";
    firstTimer.print();
    std::cout << "Stage 2: ";
    secondTimer.print();
    std::cout << "Stage 3: ";
    thirdTimer.print();
    std::cout << "Stage 4: ";
    fourthTimer.print();
    std::cout << "Total  : ";
    (firstTimer + secondTimer + thirdTimer + fourthTimer).print();
#endif
}

void ParallelClustering::clusterV5(const std::vector<double *> &data,
                                   std::size_t dimension,
                                   std::vector<std::size_t> &pi,
                                   std::vector<double> &lambda) noexcept {

    std::size_t dataSize = data.size();

    // Initializes pi and lambda vectors
    pi.resize(dataSize);
    lambda.resize(dataSize);

    // Initializes the distance matrix
    std::vector<double> m{};
    m.resize(dataSize);

    // 1. Set pi(0) to 0, lambda(0) to infinity
    pi[0] = 0;
    lambda[0] = std::numeric_limits<double>::infinity();

#ifdef TIMERS
    Timer firstTimer{};
    Timer secondTimer{};
    Timer thirdTimer{};
    Timer fourthTimer{};
#endif

    for (std::size_t n = 1; n < dataSize; n++) {

#ifdef TIMERS
        firstTimer.start();
#endif

        // 1. Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
        pi[n] = n;
        lambda[n] = std::numeric_limits<double>::infinity();

#ifdef TIMERS
        firstTimer.stop();

        secondTimer.start();
#endif

        // 2. Set M(i) to d(i, n + 1) for i = 1,..,n

        std::size_t quotient = 1 + ((dimension - 1) / 4);

#pragma omp parallel for default(none) shared(n, data, dimension, m, quotient) \
        num_threads(ParallelClustering::DISTANCE_PARALLEL_THREADS_COUNT)
        for (std::size_t i = 0; i <= n - 1; i++) {
            double sum = 0;
            for (std::size_t j = 0; j < quotient; j++) {
                __m128d dataI = _mm_load_pd(data[i]);  // data[i]{0, 1}
                __m128d dataN = _mm_load_pd(data[n]);  // data[n]{0, 1}

                __m128d sub = _mm_sub_pd(dataI, dataN);
                __m128d square = _mm_mul_pd(sub, sub);

                sum += _mm_hadd_pd(square, square)[0];
            }
            m[i] = sqrt(sum);
        }

#ifdef TIMERS
        secondTimer.stop();

        thirdTimer.start();
#endif

        // 3. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= M(i)
            if (lambda[i] >= m[i]) {
                // set M(pi(i)) to min { M(pi(i)), lambda(i) }
                m[pi[i]] = std::min(m[pi[i]], lambda[i]);
                // set lambda(i) to M(i)
                lambda[i] = m[i];
                // set pi(i) to n + 1
                pi[i] = n;
            }
            // if lambda(i) < M(i)
            if (lambda[i] < m[i]) {
                // set M(pi(i)) to min { M(pi(i)), M(i) }
                m[pi[i]] = std::min(m[pi[i]], m[i]);
            }
        }

#ifdef TIMERS
        thirdTimer.stop();

        fourthTimer.start();
#endif

        // 4. For i from 1 to n
#pragma omp parallel for default(none) shared(n, lambda, pi) \
        num_threads(ParallelClustering::STAGE_4_PARALLEL_THREADS_COUNT)
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= lambda(pi(i))
            if (lambda[i] >= lambda[pi[i]]) {
                // set pi(i) to n + 1
                pi[i] = n;
            }
        }

#ifdef TIMERS
        fourthTimer.stop();
#endif
#ifdef PRINT_ITERATIONS
        if (n % 1000 == 0) {
            std::cout << n << std::endl;
        }
#endif
    }

#ifdef TIMERS
    std::cout << "Stage 1: ";
    firstTimer.print();
    std::cout << "Stage 2: ";
    secondTimer.print();
    std::cout << "Stage 3: ";
    thirdTimer.print();
    std::cout << "Stage 4: ";
    fourthTimer.print();
    std::cout << "Total  : ";
    (firstTimer + secondTimer + thirdTimer + fourthTimer).print();
#endif
}


/**
 * Computes the distance between two points.
 *
 * @param firstPoint First point.
 * @param secondPoint Second point.
 * @param dimension Dimension of each point.
 * @return The distance between the points.
 */
double ParallelClustering::distance(const double *__restrict__ const firstPoint,
                                    const double *__restrict__ const secondPoint,
                                    const std::size_t dimension) noexcept {

    double sum = 0;
    for (std::size_t i = 0; i < dimension; i++) {
        sum += pow(firstPoint[i] - secondPoint[i], 2.0);
    }

    return sqrt(sum);
}
