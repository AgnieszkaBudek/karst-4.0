#ifndef DRUKOWANIE_NET_H
#define DRUKOWANIE_NET_H

#include <iomanip>
#include "src/utils.h"
#include "src/units.h"
#include "src/import_export/printing_primitives.h"
#include "src/net_elements/Node.h"
#include "src/net_elements/Pore.h"
#include "src/net_elements/Grain.h"


namespace karst {


    // 1. Pore3D
    struct Pore3D {

        const Pore&  p;
        std::string name;
        Color  k    {0.5,0.5,0.5};
    };

    inline std::ostream & operator<< (std::ostream &os, const Pore3D& l) {
        Point3D  pt0 = {l.p.get_nodes()[0]->get_pos()};
        Point3D  pt1 = {l.p.get_nodes()[1]->get_pos()};

        //ASSERT_MSG(! l.p.check_if_active(),l.p.get_context()+"Non active is printed.")
        // do not draw pores with d==0
        if (! l.p.check_if_active())
            return os;

        //do not draw pores at periodic boundary conditions
        if (pt0 - pt1 > Unitless (l.p.topo_config.N_x) * 0.5_U * l.p.net_config.l0){
            return os;
        }

        os << std::setprecision(4) << l.k;
        os << l.p.get_d() << " setlinewidth" << std::endl;
        //os << "newpath " << pt0 << " moveto " << pt1 << "lineto stroke" << std::endl;
        os << pt0 << " moveto " << pt1 << "lineto stroke" << std::endl;

        // printing label
        if (!l.name.empty()){
            Length u_tmp = (pt0 - pt1) / 10.;
            os << "/Times-Bold findfont " << u_tmp << " scalefont setfont "
               << Color(0, 0, 1) << std::endl;
            os << pt0 * pt1 + Point3D{.x=-u_tmp} << "moveto" << std::endl;
            os << "0 0 (" << std::setprecision(3) << l.name << ") ashow stroke" << std::endl;
        }

        return os;}



    // 2. Node3D
    struct Node3D {

        const Node & n;
        Color k{NaN};
        std::string name;
        Length r{0.1};
    };

    inline std::ostream & operator<< (std::ostream &os, const Node3D& n3D) {

        auto k_tmp = n3D.k;
        auto pos_tmp = n3D.n.get_pos();

        if (std::isnan(k_tmp.r) or std::isnan(k_tmp.g) or std::isnan(k_tmp.b)) {
            if      (n3D.n.get_type() == NodeType::NORMAL) k_tmp = Color{0.7, 0.7, 0.7};
            else if (n3D.n.get_type() == NodeType::INPUT ) k_tmp = Color{0.8, 0.8, 0.0};
            else if (n3D.n.get_type() == NodeType::OUTPUT) k_tmp = Color{0.4, 0.4, 0.8};
        }

        os << k_tmp;
        os << pos_tmp << n3D.r << " 0 360 arc fill closepath" << std::endl;

        // printing label
        if (!n3D.name.empty()){
            os << Color(0.9,0.2,0.2);
            os << "/Times-Bold findfont " << n3D.r << " scalefont setfont" << std::endl;
            os << Point3D(pos_tmp.x - n3D.r/1.3 , pos_tmp.y + n3D.r/5.) << "moveto" << std::endl;
            os << "0 0 (" << std::setprecision(5) << n3D.name << ") ashow stroke" << std::endl;
        }

        return os;
    }

    // 3. Triangle 3D
    struct Triangle3D {

        const Node &n1, &n2, &n3;
        std::string name;
        Color k{};
    };


    inline std::ostream & operator<< (std::ostream &os, const Triangle3D& tr) {

        Color k_tmp = tr.k;
        if (isnan(k_tmp))
            k_tmp = Color{0.6,0.8,0.6};


        os << k_tmp;
        os << tr.n1.get_pos() << "moveto " << tr.n2.get_pos() << "lineto " << tr.n3.get_pos()
           << "lineto closepath fill stroke" << std::endl;

        //printing label
        if (!tr.name.empty()) {
            os << Color(0, 0, 0);
            os << "/Times-Bold findfont " << 0.1 << " scalefont setfont" << std::endl;
            os << Point3D(
                    (tr.n1.get_pos().x + tr.n2.get_pos().x + tr.n3.get_pos().x) / 3._U,
                    (tr.n1.get_pos().y + tr.n2.get_pos().y + tr.n3.get_pos().y) / 3._U
                    ) << "moveto" << std::endl;
            os << "0 0 (" << std::setprecision(3) << tr.name << ") ashow stroke" << std::endl;
        }

        return os;
    }

    // 4. Polygon
    struct Polygon3D {

        const std::vector<Node *> n;
        std::string name {};
        Color k{NaN};

    };

    inline std::ostream & operator<< (std::ostream &os, const Polygon3D& w) {       //TODO: dodać skalowanie ziarna i kolorowanie ładne



        //don't draw polygons crossing periodic boundary
        auto max_l = 0._L;          //maximal distance between consecutive nodes;
        for(auto it = w.n.begin(); it<w.n.end()-1; ++it)
            if((*it)->get_pos() -(*(it+1))->get_pos() > max_l )
                max_l = (*it)->get_pos() -(*(it+1))->get_pos();
        if(max_l > Unitless (w.n[0]->topo_config.N_x) * 0.5_U * w.n[0]->net_config.l0)
            return os;

        // set default color
        Color k_tmp = w.k;
        if (isnan(k_tmp))
            k_tmp = Color{0.6,0.8,0.6};

        os << k_tmp;
        os << w.n[0]->get_pos() << "moveto ";
        for (auto n_tmp : std::ranges::subrange(std::next(w.n.begin()), w.n.end()))
            os << n_tmp->get_pos() << "lineto ";
        os << "closepath fill stroke" << std::endl;

        //printing label
        if (!w.name.empty()) {
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

    //ofstream_ps &print_grain_with_scaling(ofstream_ps &stream, Grain &g, Network &S); //TODO implement it


}  //namespace karst
#endif  //


