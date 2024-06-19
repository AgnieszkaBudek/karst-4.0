#ifndef DRUKOWANIE_H
#define DRUKOWANIE_H 

#include <iomanip>
#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/Node.h"
#include "src/net_elements/Pore.h"
#include "src/net_elements/Grain.h"

namespace karst {


    // 1. Color:
    struct Color {

        double r {0}, g {0}, b{0};
    };

    std::ostream &operator <<(std::ostream &os, const Color &k) {
        os<<"("<<k.r<<","<<k.g<<","<<k.b<<")";
        return os;}

    ofstream_ps &operator <<(ofstream_ps &os, const Color &k){
        os<<k.r<<" "<<k.g<<" "<<k.b<<" setrgbcolor"<<std::endl;
        return os;}

    // 2. Point3D
    struct Point3D {

        Length x{0}, y{0}, z{0};
    };

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


    ofstream_ps & operator<< (ofstream_ps &os, const Point3D& p) {
        if(p.z==0._L) os<<std::setprecision(4)<<std::setw(12)<<p.x<<std::setw(12)<<-p.y<<" ";
        else       os<<std::setw(12)<<p.x<<std::setw(12)<<p.y<<std::setw(12)<<p.z<<" ";
        return os;}

    std::ostream & operator<< (std::ostream &os, const Point3D& p) {
        os<<"("<<std::setw(12)<<std::setprecision(4)<<p.x<<std::setw(12)<<p.y<<std::setw(12)<<p.z<<")";
        return os;}


    // 3. Line3D
    struct Line3D {

        const Point3D a{}, b{};
        Area w {0.1};         // cross-section
    };

    std::ostream & operator<< (std::ostream &os, const Line3D& l){
        os<<l.w<<" setlinewidth"<<std::endl;
        os<<l.a<<"moveto "<<l.b<<"lineto stroke"<<std::endl;
        return os;}

    // 4. Pore3D
    struct Pore3D {

        const Pore&  p;
        double name {NaN};
        Color  k    {};
    };

    std::ostream & operator<< (std::ostream &os, const Pore3D& l) {

        os << std::setprecision(4) << l.k;
        os << l.p.get_d() << " setlinewidth" << std::endl;
        os << l.p.n[0] << " moveto " << l.p.n[1] << "lineto stroke" << std::endl;

        // printing label
        if (l.name != NaN){
            os << "/Times-Bold findfont " << (l.p.n[0]->get_pos() - l.p.n[1]->get_pos()) / 7._U << " scalefont setfont "
               << Color(0, 0, 1) << std::endl;
            os << l.p.n[0]->get_pos() * l.p.n[1]->get_pos() << "moveto" << std::endl;
            os << "0 0 (" << std::setprecision(3) << l.name << ") ashow stroke" << std::endl;
        }

        return os;}

    // 5. Dot3D
    struct Dot3D {

        Point3D a;
        Color   k    {};
        double  name {NaN};
        Length  r    {0.1};
    };

    std::ostream & operator<< (std::ostream &os, const Dot3D& d) {

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

    // 6. Node3D
    struct Node3D {

        const Node & n;
        Color k{};
        double name{NaN};
        Length r{0.1};
    };

    std::ostream & operator<< (std::ostream &os, const Node3D& n3D) {

        auto k_tmp = n3D.k;
        auto pos_tmp = n3D.n.get_pos();

        if (k_tmp.r==NaN or k_tmp.g==NaN || k_tmp.b==NaN) {
            if      (n3D.n.get_type() == NodeType::NORMAL) k_tmp = Color{0.7, 0.7, 0.7};
            else if (n3D.n.get_type() == NodeType::INPUT ) k_tmp = Color{1, 0, 0};
            else if (n3D.n.get_type() == NodeType::OUTPUT) k_tmp = Color{0, 0, 1};
        }

        os << k_tmp;
        os << pos_tmp << n3D.r << " 0 360 arc fill closepath" << std::endl;

        // printing label
        if (n3D.name != NaN){
            os << Color(0.8,0,0);
            os << "/Times-Bold findfont " << n3D.r << " scalefont setfont" << std::endl;
            os << Point(pos_tmp.x - n3D.r/3._U , pos_tmp.y - n3D.r/3._U) << "moveto" << std::endl;
            os << "0 0 (" << std::setprecision(5) << n3D.name << ") ashow stroke" << std::endl;
        }
        //os << "stroke" << std::endl;

        return os;
    }

    // 7. Triangle 3D
    struct Triangle3D {

        const Node &n1, &n2, &n3;
        double name{NaN};
        Color k{};
    };


    std::ostream & operator<< (std::ostream &os, const Triangle3D& tr) {


        os << tr.k;
        os << tr.n1.get_pos() << "moveto " << tr.n2.get_pos() << "lineto " << tr.n3.get_pos()
           << "lineto closepath fill stroke" << std::endl;

        //printing label
        if (tr.name != NaN) {
            os << Color(0, 0, 0);
            os << "/Times-Bold findfont " << 0.1 << " scalefont setfont" << std::endl;
            os << Point3D(
                    (tr.n1.get_pos().x + tr.n2.get_pos().x + tr.n3.get_pos().x) / 3,
                    (tr.n1.get_pos().y + tr.n2.get_pos().y + tr.n3.get_pos().y) / 3
                    ) << "moveto" << std::endl;
            os << "0 0 (" << std::setprecision(3) << tr.name << ") ashow stroke" << std::endl;
        }

        return os;
    }


    struct Polygon3D {

        std::deque<Node const *> n;
        double name {NaN};
        Color k{};

    };

    std::ostream & operator<< (std::ostream &os, const Polygon3D& w) {

        os << w.k;
        os << w.n[0]->get_pos() << "moveto ";
        for (auto n_tmp : std::ranges::subrange(std::next(w.n.begin()), w.n.end()))
            os << n_tmp->get_pos() << "lineto ";
        os << "closepath fill stroke" << std::endl;

        //printing label
        if (w.name != NaN) {
            os << Color(0, 0, 0);
            os << "/Times-Bold findfont " << 0.1 << " scalefont setfont" << std::endl;
            auto sr = Point3D{0._L, 0._L, 0._L};
            for (auto x : w.n)
                sr = sr + x->get_pos();
            auto sr_mean = Unitless{1./double(w.n.size())} * sr;
            os << sr_mean << "moveto" << std::endl;
            os << "0 0 (" << std::setprecision(3) << w.name << ") ashow stroke" << std::endl;
        }

        return os;
    }



    ofstream_ps &print_grain_with_scaling(ofstream_ps &stream, Grain &g, Network &S); //TODO implement it


}  //namespace karst
#endif  //def PRINTING_PRIMITIVES_H


