# ifndef UNITS_H
# define UNITS_H

#include <iostream>
#include <ratio>
#include "src/utils.h"


namespace karst {

// Default data type
    using DefaultDataType = double;     ///< default data type for Unit
    using Int  = int;                   ///< default integer type
    using Long = long;                  ///< default long integer type
    using Double = DefaultDataType;

//// Base units



// Template for units representation
    template<typename M, typename L, typename T, typename C, typename Tp = DefaultDataType>
    struct Unit {
        Tp value;

        explicit Unit(double v=NaN) : value(v) {}

        // casting to double
        explicit operator double() const {
            return static_cast<double>(value);
        }

    };

    template<typename M, typename L, typename T, typename C, typename Tp>
    std::ostream& operator<<(std::ostream& os, const Unit<M, L, T, C, Tp>& unit) {
        os << unit.value;
        return os;
    }

// Aliases for useful units
    using Mass   = Unit<std::ratio<1>, std::ratio<0>, std::ratio<0>, std::ratio<0>>;
    using Length = Unit<std::ratio<0>, std::ratio<1>, std::ratio<0>, std::ratio<0>>;
    using Time   = Unit<std::ratio<0>, std::ratio<0>, std::ratio<1>, std::ratio<0>>;
    using Mole   = Unit<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;

    using Pressure   = Unit<std::ratio<1>, std::ratio<-1>, std::ratio<-2>, std::ratio<0>>;
    using Force      = Unit<std::ratio<1>, std::ratio<1>,  std::ratio<-2>, std::ratio<0>>;
    using Flow       = Unit<std::ratio<0>, std::ratio<3>,  std::ratio<-1>, std::ratio<0>>;
    using Velocity   = Unit<std::ratio<0>, std::ratio<1>,  std::ratio<-1>, std::ratio<0>>;
    using Diffusion  = Unit<std::ratio<0>, std::ratio<2>,  std::ratio<-1>, std::ratio<0>>;
    using Area       = Unit<std::ratio<0>, std::ratio<2>,  std::ratio<0>,  std::ratio<0>>;
    using Volume     = Unit<std::ratio<0>, std::ratio<3>,  std::ratio<0>,  std::ratio<0>>;
    using Viscosity  = Unit<std::ratio<1>, std::ratio<-1>, std::ratio<-1>, std::ratio<0>>;
    using Concentration = Unit<std::ratio<0>, std::ratio<-3>, std::ratio<0>, std::ratio<1>>;
    using Unitless      = Unit<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>>;



    // User-defined literals
    inline Time       operator"" _T(long double v) { return Time       (static_cast<double>(v)); }
    inline Mass       operator"" _M(long double v) { return Mass       (static_cast<double>(v)); }
    inline Length     operator"" _L(long double v) { return Length     (static_cast<double>(v)); }
    inline Mole       operator"" _O(long double v) { return Mole       (static_cast<double>(v)); }

    inline Pressure   operator"" _P(long double v) { return Pressure   (static_cast<double>(v)); }
    inline Force      operator"" _N(long double v) { return Force      (static_cast<double>(v)); }
    inline Flow       operator"" _F(long double v) { return Flow       (static_cast<double>(v)); }
    inline Velocity   operator"" _v(long double v) { return Velocity   (static_cast<double>(v)); }
    inline Diffusion  operator"" _D(long double v) { return Diffusion  (static_cast<double>(v)); }
    inline Area       operator"" _A(long double v) { return Area       (static_cast<double>(v)); }
    inline Volume     operator"" _V(long double v) { return Volume     (static_cast<double>(v)); }
    inline Viscosity  operator"" _S(long double v) { return Viscosity  (static_cast<double>(v)); }
    inline Concentration operator"" _C(long double v) { return Concentration (static_cast<double>(v)); }
    inline Unitless      operator"" _U(long double v) { return Unitless      (static_cast<double>(v)); }



    // Units multiplication
    template<typename M1, typename L1, typename T1, typename C1, typename T, typename M2, typename L2, typename T2, typename C2>
    auto operator*(const Unit<M1, L1, T1, C1, T>& lhs, const Unit<M2, L2, T2, C2, T>& rhs) {
        return Unit<
                std::ratio_add<M1, M2>,
                std::ratio_add<L1, L2>,
                std::ratio_add<T1, T2>,
                std::ratio_add<C1, C2>,
                T
        >(lhs.value * rhs.value);
    }

// Units division
    template<typename M1, typename L1, typename T1, typename C1, typename T, typename M2, typename L2, typename T2, typename C2>
    auto operator/(const Unit<M1, L1, T1, C1, T>& lhs, const Unit<M2, L2, T2, C2, T>& rhs) {
        return Unit<
                std::ratio_subtract<M1, M2>,
                std::ratio_subtract<L1, L2>,
                std::ratio_subtract<T1, T2>,
                std::ratio_subtract<C1, C2>,
                T
        >(lhs.value / rhs.value);
    }

// Units addition
    template<typename M1, typename L1, typename T1, typename C1, typename T>
    auto operator+(const Unit<M1, L1, T1, C1, T>& lhs, const Unit<M1, L1, T1, C1, T>& rhs) {
        return Unit<M1, L1, T1, C1, T>(lhs.value + rhs.value);
    }

// Units subtraction
    template<typename M1, typename L1, typename T1, typename C1, typename T>
    auto operator-(const Unit<M1, L1, T1, C1, T>& lhs, const Unit<M1, L1, T1, C1, T>& rhs) {
        return Unit<M1, L1, T1, C1, T>(lhs.value - rhs.value);
    }

    // Units times -1
    template<typename M1, typename L1, typename T1, typename C1, typename T>
    auto operator-(const Unit<M1, L1, T1, C1, T>& u) {
        return Unit<M1, L1, T1, C1, T>(-u.value);
    }

// Units comparison
    template<typename M1, typename L1, typename T1, typename C1, typename T>
    auto operator < (const Unit<M1, L1, T1, C1, T>& lhs, const Unit<M1, L1, T1, C1, T>& rhs) ->bool {
        return lhs.value < rhs.value;
    }

    template<typename M1, typename L1, typename T1, typename C1, typename T>
    auto operator <= (const Unit<M1, L1, T1, C1, T>& lhs, const Unit<M1, L1, T1, C1, T>& rhs) ->bool {
        return lhs.value <= rhs.value;
    }

    template<typename M1, typename L1, typename T1, typename C1, typename T>
    auto operator > (const Unit<M1, L1, T1, C1, T>& lhs, const Unit<M1, L1, T1, C1, T>& rhs) ->bool {
        return lhs.value > rhs.value;
    }

    template<typename M1, typename L1, typename T1, typename C1, typename T>
    auto operator >= (const Unit<M1, L1, T1, C1, T>& lhs, const Unit<M1, L1, T1, C1, T>& rhs) ->bool {
        return lhs.value >= rhs.value;
    }

    template<typename M1, typename L1, typename T1, typename C1, typename T>
    auto operator == (const Unit<M1, L1, T1, C1, T>& lhs, const Unit<M1, L1, T1, C1, T>& rhs) ->bool {
        return lhs.value == rhs.value;
    }

//    template<typename M1, typename L1, typename T1, typename C1, typename T>  //TODO: lepiej korzystac z istniejącego std::istream& operator >> double, to wykasować
//    auto operator << (Unit<M1, L1, T1, C1, T>& lhs, const std::string& str)  {
//        try {
//            lhs.value = std::stod(str);
//        }
//        catch (const std::invalid_argument &ia) {
//            std::cerr << "Invalid string -> double conversion: " << str << std::endl;
//        }
//        return lhs;
//    }

    template<typename M1, typename L1, typename T1, typename C1, typename T>
    std::istream& operator >> (std::istream& is,Unit<M1, L1, T1, C1, T>& lhs)  {
        is >> lhs.value;
        return lhs;
    }


} // namespace karst

#endif //def UNITS_H