# ifndef KARST_4_0_UTILS_H
# define KARST_4_0_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <queue>
#include <map>
#include <numeric>
#include <cassert>
#include <functional>
#include <ranges>
#include <array>
#include <cstddef>
#include <algorithm>
#include <numbers>
#include <format>
#include <type_traits>
#include <iterator>
#include <utility>
#include <string>
#include <type_traits>

#include "src/all_enums.h"
#include "src/import_export/Logger.h"
#include "src/import_export/OutStream.h"




#ifdef NDEBUG
    #define ASSERT_MSG(cond, msg) ((void)0)
#else
#define ASSERT_MSG(cond, msg) \
        do { \
            if (!(cond)) { \
                std::cerr << std::format("\n[ASSERTION] Assertion failed: {}\n[ASSERTION] In file {} at line {}\n", msg, __FILE__, __LINE__); \
                assert(cond); \
            } \
        } while (0)
#endif


namespace karst {


    inline const double NaN = std::numeric_limits<double>::quiet_NaN();

    inline std::string bool_to_string(bool value) {
        return value ? "true" : "false";
    }





    template <class R, class T>
    concept RangeOf =
    std::ranges::input_range<R> &&
    std::same_as<
            std::remove_cvref_t<
                    std::remove_reference_t<std::ranges::range_reference_t<R>>
            >,
            T
    >;



    // Tuple to Struct template...
    template <typename T, typename Tuple>
    T tupleToStruct(Tuple&& tuple) {
        return std::apply([](auto&&... args) {
            return T{std::forward<decltype(args)>(args)...}; }, std::forward<Tuple>(tuple));
    }

///< fast as array but takes particular enum as a key!
    template<typename Enum, typename T, std::size_t N>
    class EnumArray {
        static_assert(std::is_enum_v<Enum>, "EnumArray: Enum must be an enum type.");


        public:
            using value_type = T;
            using reference = T&;
            using const_reference = const T&;
            using iterator = typename std::array<T, N>::iterator;
            using const_iterator = typename std::array<T, N>::const_iterator;


        // Default constructor - tylko jeśli T jest default-constructible
        EnumArray() requires std::is_default_constructible_v<T> {
            for (auto& e : data_) e = T{};
        }

        // Constructor z pojedynczą wartością - tylko dla copyable
        explicit EnumArray(const T& value) requires std::is_copy_constructible_v<T> {
            for (auto& e : data_) e = value;
        }

        // Constructor z lambda / callable
        template<typename F>
        explicit EnumArray(F&& f)
        requires (!std::is_same_v<std::decay_t<F>, T>)
        {
            for (std::size_t i = 0; i < N; ++i) {
                data_[i] = f(static_cast<Enum>(i));
            }
        }

        // Constructor z initializer_list
        EnumArray(std::initializer_list<std::pair<Enum, T>> init_list)
        requires std::is_copy_constructible_v<T>
        {
            for (const auto& [key, value] : init_list) {
                data_[static_cast<std::size_t>(key)] = value;
            }
        }

//        EnumArray() requires std::is_default_constructible_v<T> {
//            data_.fill(T{});
//        }
//
//        explicit EnumArray(const T& value) requires std::is_copy_constructible_v<T> {
//            data_.fill(value);
//        }
//
//            template<typename F>
//            explicit EnumArray(F&& f)
//            requires (!std::is_same_v<std::decay_t<F>, T>){
//                for (std::size_t i = 0; i < N; ++i) {
//                    data_[i] = std::move(f(static_cast<Enum>(i)));
//                }
//            }
//
//
//            EnumArray(std::initializer_list<std::pair<Enum, T>> init_list)
//                requires std::is_default_constructible_v<T> && std::is_copy_constructible_v<T>
//                : EnumArray()
//                {
//                for (const auto& [key, value] : init_list) {
//                    data_[static_cast<std::size_t>(key)] = value;
//                }
//            }

        std::vector<double> to_double_vector() const {
            std::vector<double> result;
            result.reserve(N);
            for (const auto& v : data_) {
                result.push_back(double(v));
            }
            return result;
        }

        private:
            std::array<T, N> data_;

        public:
            //full access to data
            auto get_data() const -> const std::array<T, N>& {return  data_;}
            auto get_data() -> std::array<T, N>& { return data_; }               // modyfikowalna wersja
            void fill(const T& val) { data_.fill(val); }

            // Access by enum key
            reference operator[](Enum e) { return data_[static_cast<std::size_t>(e)];}
            const_reference operator[](Enum e) const {return data_[static_cast<std::size_t>(e)];}

            // Access to raw data
            reference at(std::size_t i) { return data_.at(i); }
            const_reference at(std::size_t i) const { return data_.at(i); }

            // Raw iterators
            iterator        begin()         { return data_.begin(); }
            iterator        end  ()         { return data_.end();   }
            const_iterator  begin() const   { return data_.begin(); }
            const_iterator  end  () const   { return data_.end();   }

            // For structured binding iteration
            struct entry {
                Enum key;
                reference value;
            };

            struct const_entry {
                Enum key;
                const_reference value;
            };

            class iter {
                EnumArray& parent;
                std::size_t index;

            public:
                using difference_type = std::ptrdiff_t;
                using value_type = entry;
                using reference = entry;
                using pointer = void;
                using iterator_category = std::forward_iterator_tag;

                iter(EnumArray& parent, std::size_t index) : parent(parent), index(index) {}

                reference operator*() {
                    return {static_cast<Enum>(index), parent.data_[index]};
                }

                iter& operator++() {
                    ++index;
                    return *this;
                }

                bool operator!=(const iter& other) const {
                    return index != other.index;
                }
            };

            class const_iter {
                const EnumArray& parent;
                std::size_t index;

            public:
                using difference_type = std::ptrdiff_t;
                using value_type = const_entry;
                using reference = const_entry;
                using pointer = void;
                using iterator_category = std::forward_iterator_tag;

                const_iter(const EnumArray& parent, std::size_t index) : parent(parent), index(index) {}

                reference operator*() const {
                    return {static_cast<Enum>(index), parent.data_[index]};
                }

                const_iter& operator++() {
                    ++index;
                    return *this;
                }

                bool operator!=(const const_iter& other) const {
                    return index != other.index;
                }
        };

        iter entries_begin() { return iter(*this, 0); }
        iter entries_end  () { return iter(*this, N); }

        const_iter entries_begin() const { return const_iter(*this, 0); }
        const_iter entries_end  () const { return const_iter(*this, N); }


        // Structured binding range
        auto entries() {
            struct wrapper {
                EnumArray& parent;
                auto begin() { return parent.entries_begin(); }
                auto end  () { return parent.entries_end();   }
            };
            return wrapper{*this};
        }

        auto entries() const {
            struct wrapper {
                const EnumArray& parent;
                auto begin() const { return parent.entries_begin(); }
                auto end  () const { return parent.entries_end();   }
            };
            return wrapper{*this};
        }


    };


}

#endif       //KARST_4_0_UTILS_H