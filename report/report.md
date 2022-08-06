---
title: Clustering Parallelization Project
author:

- Alessio de Biasi (870288)
- Jonathan Gobbo (870506)
  date: August 2022
  geometry: "left=3cm,right=3cm,top=2cm,bottom=2cm"

---
<!--
TODO: Aggiungere capitolo considerazioni finali
- Numero ottimale di thread
- Speedup massimo raggiunto
- Ottimizzazioni inutili

TODO: Decidere quali e quanti pc usare per i test
-->

- Coordinates of points are stored sequentially in memory.

# Versions

<!--
Per ognuno
- organizzazione dei dati (vector di array, array unico, allineati?)
- spiegazione semplice
- motivazione
- tempi per un tot di dataset
-->

## Sequential

The first version of the algorithm we implemented is the sequential one. Data is supplied as
a `std::vector<double *>`, i.e., a vector of pointers to arrays stored in heap memory. It is a
simple implementation of the pseudo-code of the *SLINK* algorithm as seen in the slides, and will be
used as reference to measure the speed-up obtained in the other versions.

<!--
TODO: test
-->

| ss  | ss    |
|-----|-------|
| dd  | ff b  |

## Distanze calcolate in parallelo

This is the first and simplest parallelized version of the algorithm. Data is supplied in the same
way as the sequential one. The computation of the distances is parallelized by using the *OpenMP*
library. The decision to begin with this is given by the fact that we observed that stage of the
algorithm to be the most expensive in terms of time, and also the lack of data dependencies.

## SSE & AVX

In the second parallel version we implemented the distance computation code using *SSE*
instructions. We deemed using SIMD instructions useful since the attributes of each point are
contiguous in memory and we have to apply the same sequence of instructions on each attribute.

In the third revision we replaced the SSE instructions with *AVX* ones, to see if performance would
improve. In both cases we had to deal with memory alignment by adding padding at the end of each
sample with zeroes, but otherwise the data is supplied in the same way.

## Distanze multithread + SSE & AVX

In the fourth and fifth solutions we combined respectively the multithreaded distance calculation
with SSE first and AVX second, with the purpose of seeing if these techniques would work together
well.

## Linearizing data (arrayone)

In this sixth try we modified the previous multithreaded with AVX solution to work on data which has
been linearized, i.e. a single array of double values aligned to work with AVX instructions. The
purpose of this test is to see if there are any cache awareness improvements given by the linearized
memory.

## Stage 4 parallel

This seventh version is based on the fourth one, with the addition of the multithreaded
parallelization of the stage 4 of the algorithm. Once again we noticed that at this stage there are
no data dependencies, making it trivial to parallelize.

## Ottimizzazione calcolo distanze mantenendo somme parziali all'interno dei registri

The eighth and ninth solutions aim to optimize the SSE and AVX code of the distance calculations by
avoiding to write partial sums in memory and keeping them in the registers. It also uses
multithreading in both the distance calculator and the stage 4.

## Linearized + calcolo distanze ottimizzato

The tenth and eleventh algorithms modify the previous couple of solutions by passing the linearized
data instead.

## Utilizzo dei quadrati delle distanze + linearized

This approach, based on the previous solutions, is an approximation algorithm in the sense that it
computes the square root of the distances after the stage 4, causing the previous code to perform
inequality checks with the distances squared. This, despite the rounding errors, should not cause
any issue since if $\sqrt{n^2} \geq \sqrt{m^2}$, then $n^2 \geq m^2$.

## Sequential and Parallel with iterators

To generalize our code and hide the data structures used we decided to use iterators. This also
improved the performance of both sequential and parallel algorithms.

# How to use the library

The Sequential and Parallel algorithms are provided respectively by the `SequentialClustering.h`
and `ParallelClustering.h` header files.

## Sequential Algorithm

The sequential `cluster` function has the following signature:

```cpp
template <DataIterator D, PiIterator P, LambdaIterator L>
static void cluster(D dataIterator, std::size_t dataSamplesCount, 
    std::size_t dimension, P piIterator, L lambdaIterator) noexcept
```

To invoke the sequential clustering algorithm, the following parameters must be passed to
the `cluster` function:

- `dataIterator`: a dataset iterator of generic type `D` that must satisfy the
  concept `DataIterator`, defined in the header file `Types.h`;
- `dataSamplesCount`: the number of rows in the dataset;
- `dimension`: the number of columns in the dataset;
- `piIterator`: an iterator on the *pi* data structure, of generic type `P` satisfying
  the `PiIterator` concept;
- `lambdaIterator`: an iterator on the *lambda* data structure, of generic type `L` satisfying
  the `LambdaIterator` concept;

The `cluster` function assumes that the data structures underlying the `piIterator`
and `lambdaIterator` iterators are big enough to hold all the values.

## Parallel Algorithm

The parallel `cluster` function has the following signature:

```cpp
template <DistanceComputers C, ParallelDataIterator D, PiIterator P,
    LambdaIterator L, bool S2 = true, bool S4 = false, bool S5 = false>
static void cluster(const D dataIterator, std::size_t dataSamplesCount,
    std::size_t dimension, P piIterator,L lambdaIterator,
    std::size_t distanceComputationThreadsCount = 0,
    std::size_t stage4ThreadsCount = 0,
    std::size_t squareRootThreadsCount = 0)
```

To invoke the parallel clustering algorithm, the following parameters must be passed to
the `cluster` function:

- `dataIterator`: a dataset iterator of generic type `D` that must satisfy the
  concept `ParallelDataIterator`, defined in the header file `Types.h`. In contrary to the
  sequential version, this concept requires data to be addressable through the `[]` operator to
  allow concurrent data access;
- `distanceComputationThreadsCount`: defines the number of threads used to compute the distances
  between the points. By default it is $0$, meaning that OpenMP will use all the available CPU
  threads to perform the computation. If `S2` is false, this value is ignored;
- `stage4ThreadsCount`: defines the number of threads used to perform the stage 4 of the clustering
  algorithm. If `S4` is false, this value is ignored;
- `squareRootThreadsCount`: defines the number of threads used to compute the square roots of the
  distances. If `S5` is false, or `C` is not `SSE_OPTIMIZED_NO_SQUARE_ROOT`
  nor `AVX_OPTIMIZED_NO_SQUARE_ROOT`, then this value is ignored;

The parameters `dataSamplesCount`, `dimension`, `piIterator` and `lambdaIterator` are equivalent to
the sequential version.

Moreover, the following template parameters must be specified:

- `DistanceComputers`: enumeration that defines which variant of the parallel cluster algorithm to
  be used to compute the distance between the points. Its possible values are `CLASSICAL`, `SSE`
  , `AVX`, `SSE_OPTIMIZED`, `AVX_OPTIMIZED`, `SSE_OPTIMIZED_NO_SQUARE_ROOT`
  , `AVX_OPTIMIZED_NO_SQUARE_ROOT`. Their meaning are described in the previous chapter;
- `S2`: boolean value defaulted to `true`, indicates whether the distance computation stage of the
  algorithm should be parallelized using threads;
- `S4`: boolean value defaulted to `false`, indicates whether the stage 4 of the algorithm should be
  parallelized using threads;
- `S5`: boolean value defaulted to `false`, indicates whether the computation of the square roots of
  the distances should be parallelized using threads. Effective only if `C` is
  either `SSE_OPTIMIZED_NO_SQUARE_ROOT` or `AVX_OPTIMIZED_NO_SQUARE_ROOT`.
