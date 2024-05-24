# ifndef UNITS_H
# define UNITS_H

#include <iostream>
#include <ratio>


namespace karst {

// Default data type
    using DefaultDataType = double;     ///< default data type for Unit
    using Int  = int;                   ///< default integer type
    using Long = long;                  ///< default long integer type

//// Base units



// Template for units representation
    template<typename M, typename L, typename T, typename S, typename Tp = DefaultDataType>
    struct Unit {
        Tp value;

        explicit Unit(double v) : value(v) {}
    };

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




// Units multiplication
    template<typename M1, typename L1, typename T1, typename S1, typename T, typename M2, typename L2, typename T2, typename S2>
    auto operator*(const Unit<M1, L1, T1, S1, T>& lhs, const Unit<M2, L2, T2, S2, T>& rhs) {
        return Unit<
                std::ratio_add<M1, M2>,
                std::ratio_add<L1, L2>,
                std::ratio_add<T1, T2>,
                std::ratio_add<S1, S2>,
                T
        >(lhs.value * rhs.value);
    }

// Units division
    template<typename M1, typename L1, typename T1, typename S1, typename T, typename M2, typename L2, typename T2, typename S2>
    auto operator/(const Unit<M1, L1, T1, S1, T>& lhs, const Unit<M2, L2, T2, S2, T>& rhs) {
        return Unit<
                std::ratio_subtract<M1, M2>,
                std::ratio_subtract<L1, L2>,
                std::ratio_subtract<T1, T2>,
                std::ratio_subtract<S1, S2>,
                T
        >(lhs.value / rhs.value);
    }

// Units addition
    template<typename M1, typename L1, typename T1, typename S1, typename T>
    auto operator+(const Unit<M1, L1, T1, S1, T>& lhs, const Unit<M1, L1, T1, S1, T>& rhs) {
        return Unit<M1, L1, T1, S1, T>(lhs.value + rhs.value);
    }

// Units subtraction
    template<typename M1, typename L1, typename T1, typename S1, typename T>
    auto operator-(const Unit<M1, L1, T1, S1, T>& lhs, const Unit<M1, L1, T1, S1, T>& rhs) {
        return Unit<M1, L1, T1, S1, T>(lhs.value - rhs.value);
    }

// Units comparison
    template<typename M1, typename L1, typename T1, typename S1, typename T>
    auto operator < (const Unit<M1, L1, T1, S1, T>& lhs, const Unit<M1, L1, T1, S1, T>& rhs) ->bool {
        return lhs.value < rhs.value;
    }

    template<typename M1, typename L1, typename T1, typename S1, typename T>
    auto operator <= (const Unit<M1, L1, T1, S1, T>& lhs, const Unit<M1, L1, T1, S1, T>& rhs) ->bool {
        return lhs.value <= rhs.value;
    }

    template<typename M1, typename L1, typename T1, typename S1, typename T>
    auto operator > (const Unit<M1, L1, T1, S1, T>& lhs, const Unit<M1, L1, T1, S1, T>& rhs) ->bool {
        return lhs.value > rhs.value;
    }

    template<typename M1, typename L1, typename T1, typename S1, typename T>
    auto operator >= (const Unit<M1, L1, T1, S1, T>& lhs, const Unit<M1, L1, T1, S1, T>& rhs) ->bool {
        return lhs.value >= rhs.value;
    }


} // namespace karst

#endif //def UNITS_H