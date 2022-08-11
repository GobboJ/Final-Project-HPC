#ifndef FINAL_PROJECT_HPC_TYPESPRINTER_H
#define FINAL_PROJECT_HPC_TYPESPRINTER_H

#include <array>
#include <deque>
#include <list>
#include <string>
#include <vector>

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-10
 * @since version date
 */
class TypesPrinter {

private:
    template <typename T>
    struct IsStdArray : std::false_type {};

    template <typename T, std::size_t N>
    struct IsStdArray<std::array<T, N>> : std::true_type {};

    template <typename T>
    struct IsAlignedArray : std::false_type {};

    template <typename T, std::size_t A, std::size_t N>
    struct IsAlignedArray<AlignedArray<T, N, A>> : std::true_type {};

    template <typename T, std::size_t A>
    struct IsAlignedTo : std::false_type {};

    template <typename T, std::size_t A, std::size_t N>
    struct IsAlignedTo<AlignedArray<T, N, A>, A> : std::true_type {};

public:
    template <typename T>
    static std::string getTypeName() {

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
        } else if constexpr (std::is_pointer_v<NonReferenceType>) {
            typeName = getTypeName<std::remove_pointer_t<NonReferenceType>>() + " *";
        } else if constexpr (
                std::is_same_v<NonCVRefType, std::vector<typename NonCVRefType::value_type>>) {
            typeName = "std::vector<" + getTypeName<typename NonCVRefType::value_type>() + ">";
        } else if constexpr (
                std::is_same_v<NonCVRefType, std::list<typename NonCVRefType::value_type>>) {
            typeName = "std::list<" + getTypeName<typename NonCVRefType::value_type>() + ">";
        } else if constexpr (
                std::is_same_v<NonCVRefType, std::deque<typename NonCVRefType::value_type>>) {
            typeName = "std::deque<" + getTypeName<typename NonCVRefType::value_type>() + ">";
        } else if constexpr (IsStdArray<NonCVRefType>::value) {
            typeName = "std::array<" + getTypeName<typename NonCVRefType::value_type>() + ", N>";
        } else if constexpr (
                std::is_same_v<NonCVRefType,
                               typename std::vector<typename NonCVRefType::value_type>::iterator>) {
            typeName = "std::vector<" + getTypeName<typename NonCVRefType::value_type>() +
                       ">::iterator";
        } else if constexpr (
                std::is_same_v<NonCVRefType,
                               typename std::list<typename NonCVRefType::value_type>::iterator>) {
            typeName =
                    "std::list<" + getTypeName<typename NonCVRefType::value_type>() + ">::iterator";
        } else if constexpr (
                std::is_same_v<NonCVRefType,
                               typename std::deque<typename NonCVRefType::value_type>::iterator>) {
            typeName = "std::deque<" + getTypeName<typename NonCVRefType::value_type>() +
                       ">::iterator";
        } else if constexpr (
                std::is_same_v<
                        NonCVRefType,
                        typename std::vector<typename NonCVRefType::value_type>::const_iterator>) {
            typeName = "std::vector<" + getTypeName<typename NonCVRefType::value_type>() +
                       ">::const_iterator";
        } else if constexpr (
                std::is_same_v<
                        NonCVRefType,
                        typename std::list<typename NonCVRefType::value_type>::const_iterator>) {
            typeName = "std::list<" + getTypeName<typename NonCVRefType::value_type>() +
                       ">::const_iterator";
        } else if constexpr (
                std::is_same_v<
                        NonCVRefType,
                        typename std::deque<typename NonCVRefType::value_type>::const_iterator>) {
            typeName = "std::deque<" + getTypeName<typename NonCVRefType::value_type>() +
                       ">::const_iterator";
        } else if constexpr (IsAlignedArray<NonCVRefType>::value) {
            typeName = "AlignedArray<" + getTypeName<typename NonCVRefType::value_type>() +
                       ", N, " +
                       ((IsAlignedTo<NonCVRefType, Alignments::AVX_ALIGNMENT>::value)
                                ? "AVX_ALIGNMENT"
                                : "SSE_ALIGNMENT") +
                       ">";
        } else {
            typeName = "UNKNOWN TYPE";
        }

        return typeName;
    }
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_TYPESPRINTER_H
