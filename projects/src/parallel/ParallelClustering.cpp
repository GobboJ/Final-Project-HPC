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

/**
 * Computes the distance between two points.
 *
 * @param firstPoint First point.
 * @param secondPoint Second point.
 * @param dimension Dimension of each point.
 * @return The distance between the points.
 */
