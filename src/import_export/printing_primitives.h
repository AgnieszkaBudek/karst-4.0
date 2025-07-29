#ifndef PRINTING_PRIMITIVES_H
#define PRINTING_PRIMITIVES_H

#include <iomanip>
#include "src/utils.h"
#include "src/units.h"


namespace karst {


    // 1. Color:
    struct Color {

        double r {0.5}, g {0.5}, b{0.5};
    };

    inline std::ostream &operator <<(std::ostream &os, const Color &k) {
        //os<<"("<<k.r<<","<<k.g<<","<<k.b<<")";                                //TODO: poprawić rozpoznawanie rodziajó∑ strumieni wyjściowych
        os<<k.r<<" "<<k.g<<" "<<k.b<<" setrgbcolor"<<std::endl;
        return os;}

//    inline ofstream_ps &operator <<(ofstream_ps &os, const Color &k){
//        os<<k.r<<" "<<k.g<<" "<<k.b<<" setrgbcolor"<<std::endl;
//        return os;}


    inline auto isnan(Color k) -> bool{
        return (std::isnan(k.r) or std::isnan(k.g) or std::isnan(k.b));
    }

    // 2. Point3D
    struct Point3D {

        Length x{0.}, y{0.}, z{0.};
    };

    inline auto isnan(Point3D point) -> bool{
        return(
            std::isnan(double (point.x)) or
            std::isnan(double (point.y)) or
            std::isnan(double (point.z))
        );
    }

    inline auto operator - (const Point3D&  p1, const Point3D&  p2) -> Length {  //return distance between two points
        return Length(
                sqrt(pow(double(p1.x - p2.x), 2) +
                     pow(double(p1.y - p2.y), 2) +
                     pow(double(p1.z - p2.z), 2))
                     );
    }

    inline auto  operator * (const Point3D& p1,  const Point3D& p2)  -> Point3D {  //return middle of two points
        return Point3D(
                (p1.x+p2.x)/2.0_U,
                (p1.y+p2.y)/2.0_U,
                (p1.z+p2.z)/2.0_U
                );
                }

    inline auto  operator + (const Point3D&  p1, const Point3D&  p2) -> Point3D {  //return sum of two points
        return Point3D ((p1.x+p2.x), (p1.y+p2.y), (p1.z+p2.z));
    }

    inline auto  operator * (Unitless a,  const Point3D &p ) -> Point3D {
        return Point3D(a*p.x, a*p.y, a*p.z);}

    inline auto  operator * (double a,  const Point3D &p ) -> Point3D {
        return Point3D(a*p.x, a*p.y, a*p.z);}


//    inline ofstream_ps & operator<< (ofstream_ps &os, const Point3D& p) {
//        if(p.z==0._L) os<<std::setprecision(4)<<std::setw(12)<<p.x<<std::setw(12)<<-p.y<<" ";
//        else       os<<std::setw(12)<<p.x<<std::setw(12)<<p.y<<std::setw(12)<<p.z<<" ";
//        return os;}

    inline std::ostream & operator<< (std::ostream &os, const Point3D& p) {
        //os<<"("<<std::setw(12)<<std::setprecision(4)<<p.x<<std::setw(12)<<p.y<<std::setw(12)<<p.z<<")";       //TODO: poprawić obsługę różnych strumieni wyjśćiowych
        if(p.z==0._L) os<<std::setprecision(4)<<std::setw(12)<<p.x<<std::setw(12)<<-p.y<<" ";
        else       os<<std::setw(12)<<p.x<<std::setw(12)<<p.y<<std::setw(12)<<p.z<<" ";
        return os;}


    // 3. Line3D
    struct Line3D {

        const Point3D a{}, b{};
        Area w {0.1};         // cross-section
    };

    inline std::ostream & operator<< (std::ostream &os, const Line3D& l){
        os<<l.w<<" setlinewidth"<<std::endl;
        os<<l.a<<"moveto "<<l.b<<"lineto stroke"<<std::endl;
        return os;}



    // 4. Dot3D
    struct Dot3D {

        Point3D a;
        Color   k    {};
        double  name {NaN};
        Length  r    {0.1};
    };

    inline std::ostream & operator<< (std::ostream &os, const Dot3D& d) {

        os << d.k;
        os << d.a << d.r << " 0 360 arc fill closepath" << std::endl;

        // printing label
        if (d.name != NaN) {
            os << Color(0.8, 0, 0);
            os << "/Times-Bold findfont " << d.r << " scalefont setfont" << std::endl;
            os << Point3D(d.a.x - d.r / 3._U, d.a.y - d.r / 3._U) << "moveto" << std::endl;
            os << "0 0 (" << std::setprecision(5) << d.name << ") ashow stroke" << std::endl;
        }

        //os << "stroke"<<std::endl;

        return os;
    }


}  //namespace karst
#endif  //def PRINTING_PRIMITIVES_H


