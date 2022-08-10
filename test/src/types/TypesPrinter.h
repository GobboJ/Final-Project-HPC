#ifndef FINAL_PROJECT_HPC_TYPESPRINTER_H
#define FINAL_PROJECT_HPC_TYPESPRINTER_H

#include <string>
#include <array>
#include <vector>
#include <list>
#include <deque>

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

        if constexpr (std::is_same_v<T, double>) {
            typeName = "double";
        } else if constexpr (std::is_same_v<T, const double>) {
            typeName = "const double";
        } else if constexpr (std::is_same_v<T, std::size_t>) {
            typeName = "std::size_t";
        } else if constexpr (std::is_same_v<T, const std::size_t>) {
            typeName = "const std::size_t";
        } else if constexpr (std::is_pointer_v<T>) {
            typeName = getTypeName<std::remove_pointer_t<T>>() + " *";
        } else if constexpr (std::is_same_v<T, std::vector<typename T::value_type>>) {
            typeName = "std::vector<" + getTypeName<typename T::value_type>() + ">";
        } else if constexpr (std::is_same_v<T, std::list<typename T::value_type>>) {
            typeName = "std::list<" + getTypeName<typename T::value_type>() + ">";
        } else if constexpr (std::is_same_v<T, std::deque<typename T::value_type>>) {
            typeName = "std::deque<" + getTypeName<typename T::value_type>() + ">";
        } else if constexpr (IsStdArray<T>::value) {
            typeName = "std::array<" + getTypeName<typename T::value_type>() + ", N>";
        } else if constexpr (
                std::is_same_v<T, typename std::vector<typename T::value_type>::iterator>) {
            typeName = "std::vector<" + getTypeName<typename T::value_type>() + ">::iterator";
        } else if constexpr (
                std::is_same_v<T, typename std::list<typename T::value_type>::iterator>) {
            typeName = "std::list<" + getTypeName<typename T::value_type>() + ">::iterator";
        } else if constexpr (
                std::is_same_v<T, typename std::deque<typename T::value_type>::iterator>) {
            typeName = "std::deque<" + getTypeName<typename T::value_type>() + ">::iterator";
        } else if constexpr (
                std::is_same_v<T, typename std::vector<typename T::value_type>::const_iterator>) {
            typeName = "std::vector<" + getTypeName<typename T::value_type>() + ">::const_iterator";
        } else if constexpr (
                std::is_same_v<T, typename std::list<typename T::value_type>::const_iterator>) {
            typeName = "std::list<" + getTypeName<typename T::value_type>() + ">::const_iterator";
        } else if constexpr (
                std::is_same_v<T, typename std::deque<typename T::value_type>::const_iterator>) {
            typeName = "std::deque<" + getTypeName<typename T::value_type>() + ">::const_iterator";
        } else if constexpr (IsAlignedArray<T>::value) {
            typeName = "AlignedArray<" + getTypeName<typename T::value_type>() + ", N, " +
                       ((IsAlignedTo<T, 32>::value) ? "AVX_ALIGNMENT" : "SSE_ALIGNMENT") + ">";
        } else {
            typeName = "UNKNOWN TYPE";
        }

        return typeName;
    }
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_TYPESPRINTER_H
