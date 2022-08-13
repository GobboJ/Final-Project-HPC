#ifndef FINAL_PROJECT_HPC_TYPESPRINTER_H
#define FINAL_PROJECT_HPC_TYPESPRINTER_H

#include <array>
#include <deque>
#include <list>
#include <string>
#include <vector>
#include "collections/OnlyIterableVector.h"
#include "collections/OnlyConstIterableVector.h"

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-10
 * @since version date
 */
class TypesPrinter {
    using Alignments = collections::Alignments;

private:
    template <typename T>
    struct IsStdArray : std::false_type {};

    template <typename T, std::size_t N>
    struct IsStdArray<std::array<T, N>> : std::true_type {};

    template <typename T>
    struct IsAlignedArray : std::false_type {};

    template <typename T, std::size_t A, std::size_t N>
    struct IsAlignedArray<collections::AlignedArray<T, N, A>> : std::true_type {};

    template <typename T, std::size_t A>
    struct IsAlignedTo : std::false_type {};

    template <typename T, std::size_t A, std::size_t N>
    struct IsAlignedTo<collections::AlignedArray<T, N, A>, A> : std::true_type {};

    template <typename T>
    struct IsPair : std::false_type {};

    template <typename F, typename S>
    struct IsPair<std::pair<F, S>> : std::true_type {};

public:
    template <typename T>
    static std::string getTypeName() {

        return composeTypeName<T>(true);
    }

private:
    template <typename T>
    static std::string composeTypeName(bool isTopType = false) {
        std::string typeName{};

        using NonReferenceType = std::remove_reference_t<T>;
        using NonCVRefType = std::remove_cvref_t<T>;

        if constexpr (std::is_same_v<NonReferenceType, double>) {
            typeName = "double";
        } else if constexpr (std::is_same_v<NonReferenceType, const double>) {
            typeName = "const double";
        } else if constexpr (std::is_same_v<NonReferenceType, std::size_t>) {
            typeName = "std::size_t";
        } else if constexpr (std::is_same_v<NonReferenceType, const std::size_t>) {
            typeName = "const std::size_t";
        } else if constexpr (std::is_same_v<NonReferenceType, int>) {
            typeName = "int";
        } else if constexpr (std::is_same_v<NonReferenceType, const int>) {
            typeName = "const int";
        } else if constexpr (std::is_same_v<NonCVRefType, std::filesystem::path>) {
            typeName = "std::filesystem::path";
        } else if constexpr (std::is_same_v<NonCVRefType, std::string>) {
            typeName = "std::string";
        } else if constexpr (std::is_pointer_v<NonReferenceType>) {
            typeName = composeTypeName<std::remove_pointer_t<NonReferenceType>>() + " *";
        } else if constexpr (IsPair<NonCVRefType>::value) {
            typeName = "std::pair<" + composeTypeName<typename NonCVRefType::first_type>() + ", " +
                       composeTypeName<typename NonCVRefType::second_type>() + ">";
        } else if constexpr (std::is_same_v<NonCVRefType, std::filesystem::path::const_iterator>) {
            typeName = "std::filesystem::path::const_iterator";
        } else if constexpr (std::is_same_v<NonCVRefType, std::string::iterator>) {
            typeName = "std::string::iterator";
        } else if constexpr (std::is_same_v<NonCVRefType, std::string::const_iterator>) {
            typeName = "std::string::const_iterator";
        } else if constexpr (
                std::is_same_v<NonCVRefType, std::vector<typename NonCVRefType::value_type>>) {
            typeName = "std::vector<" + composeTypeName<typename NonCVRefType::value_type>() + ">";
        } else if constexpr (
                std::is_same_v<NonCVRefType, std::list<typename NonCVRefType::value_type>>) {
            typeName = "std::list<" + composeTypeName<typename NonCVRefType::value_type>() + ">";
        } else if constexpr (
                std::is_same_v<NonCVRefType, std::deque<typename NonCVRefType::value_type>>) {
            typeName = "std::deque<" + composeTypeName<typename NonCVRefType::value_type>() + ">";
        } else if constexpr (IsStdArray<NonCVRefType>::value) {
            typeName =
                    "std::array<" + composeTypeName<typename NonCVRefType::value_type>() + ", N>";
        } else if constexpr (
                std::is_same_v<NonCVRefType,
                               typename std::vector<typename NonCVRefType::value_type>::iterator>) {
            typeName = "std::vector<" + composeTypeName<typename NonCVRefType::value_type>() +
                       ">::iterator";
        } else if constexpr (
                std::is_same_v<NonCVRefType,
                               typename std::list<typename NonCVRefType::value_type>::iterator>) {
            typeName = "std::list<" + composeTypeName<typename NonCVRefType::value_type>() +
                       ">::iterator";
        } else if constexpr (
                std::is_same_v<NonCVRefType,
                               typename std::deque<typename NonCVRefType::value_type>::iterator>) {
            typeName = "std::deque<" + composeTypeName<typename NonCVRefType::value_type>() +
                       ">::iterator";
        } else if constexpr (
                std::is_same_v<
                        NonCVRefType,
                        typename std::vector<typename NonCVRefType::value_type>::const_iterator>) {
            typeName = "std::vector<" + composeTypeName<typename NonCVRefType::value_type>() +
                       ">::const_iterator";
        } else if constexpr (
                std::is_same_v<
                        NonCVRefType,
                        typename std::list<typename NonCVRefType::value_type>::const_iterator>) {
            typeName = "std::list<" + composeTypeName<typename NonCVRefType::value_type>() +
                       ">::const_iterator";
        } else if constexpr (
                std::is_same_v<
                        NonCVRefType,
                        typename std::deque<typename NonCVRefType::value_type>::const_iterator>) {
            typeName = "std::deque<" + composeTypeName<typename NonCVRefType::value_type>() +
                       ">::const_iterator";
        } else if constexpr (IsAlignedArray<NonCVRefType>::value) {
            typeName = "AlignedArray<" + composeTypeName<typename NonCVRefType::value_type>() +
                       ", N, " +
                       ((IsAlignedTo<NonCVRefType, Alignments::AVX_ALIGNMENT>::value)
                                ? "AVX_ALIGNMENT"
                                : "SSE_ALIGNMENT") +
                       ">";
        } else if constexpr (
                std::is_same_v<
                        NonCVRefType,
                        collections::OnlyIterableVector<typename NonCVRefType::value_type>>) {
            typeName = "OnlyIterableVector<" +
                       composeTypeName<typename NonCVRefType::value_type>() + ">";
        } else if constexpr (
                std::is_same_v<
                        NonCVRefType,
                        collections::OnlyConstIterableVector<typename NonCVRefType::value_type>>) {
            typeName = "OnlyConstIterableVector<" +
                       composeTypeName<typename NonCVRefType::value_type>() + ">";
        } else if constexpr (std::is_same_v<NonCVRefType,
                                            typename collections::OnlyIterableVector<
                                                    typename NonCVRefType::value_type>::iterator>) {
            typeName = "OnlyIterableVector<" +
                       composeTypeName<typename NonCVRefType::value_type>() + ">::iterator";
        } else if constexpr (
                std::is_same_v<NonCVRefType,
                               typename collections::OnlyConstIterableVector<
                                       typename NonCVRefType::value_type>::const_iterator>) {
            typeName = "OnlyConstIterableVector<" +
                       composeTypeName<typename NonCVRefType::value_type>() + ">::const_iterator";
        } else {
            typeName = "UNKNOWN TYPE";
        }

        if (!isTopType && std::is_const_v<NonReferenceType>) {
            if constexpr (std::is_pointer_v<NonReferenceType>) {
                typeName = typeName + " const";
            } else {
                typeName = "const " + typeName;
            }
        }

        return typeName;
    }
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_TYPESPRINTER_H
