//
// Created by Agnieszka on 06/09/2025.
//

#ifndef KARST_4_0_CREATE_2D_TRIANGULATION_H
#define KARST_4_0_CREATE_2D_TRIANGULATION_H

#include <iostream>
#include <vector>
#include <random>
#include <utility>
#include <tuple>
#include <algorithm>
#include <algorithm>
#include <set>

//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Delaunay_triangulation_2.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"

namespace karst {


// Type definitions for better readability
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Point_2 = Kernel::Point_2;
    using Triangulation_2 = CGAL::Delaunay_triangulation_2<Kernel>;

// A struct to store the triangulation results
    struct TriangulationResult2D {
        std::vector<std::pair<int, int>> edges;
        std::vector<std::array<int, 3>> faces;
        std::vector<Point_2> points;
    };



    TriangulationResult2D generate_and_triangulate_periodic(int Nx, int Ny, TriangulationResult2D& result, Network& S) {
        S.log.log("Performing Delaney triangulation with periodic bc.");

        if (Nx <= 0 || Ny <= 0) {
            S.log.log<LogLevel::ERROR>("Dimensions Nx and Ny must be positive.");
            return result; // Return an empty result
        }

        // 0. Clear result (just in case)
        result.points.clear();
        result.edges.clear();
        result.faces.clear();

        // 1. Generate random points
        std::mt19937 gen(S.t_config.random_seed);
        std::uniform_real_distribution<> distribX(0.0, static_cast<double>(Nx));
        std::uniform_real_distribution<> distribY(0.0, static_cast<double>(Ny));

        int total_points = Nx * Ny;
        result.points.reserve(total_points);
        while (result.points.size()<total_points) {
            bool accept = true;
            double x = distribX(gen);
            double y = distribY(gen);
            for(auto& p_tmp : result.points){
                auto dist = std::sqrt(pow(p_tmp.x() - x,2) + pow(p_tmp.y() - y,2));
                if(dist<double(S.t_config.min_distance_between_nodes)) accept = false;
            }
            if(accept) result.points.emplace_back(x, y);
        }

        // 1.2 Sorting
        S.log.log("Sorting nodes.");
        std::sort(result.points.begin(), result.points.end(),
                  [](const Point_2& a, const Point_2& b) {
                      return std::tuple{int(a.y()), a.x()} < std::tuple{int(b.y()), b.x()};
                  });

        // 2. Create the triangulation
        Triangulation_2 dt;
        // 2.1: Map: point -> index in result.points
        struct PointLess {
            bool operator()(const Point_2& a, const Point_2& b) const {
                if (a.y() < b.y()) return true;
                if (a.y() > b.y()) return false;
                return a.x() < b.x();
            }
        };
        std::map<Point_2, int, PointLess> idx_by_point;


        // Periodic: we construct 9 cells around
        std::vector<Point_2> tiled_points;
        tiled_points.reserve(result.points.size() * 9);
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                for (int i = 0; i <static_cast<int>(result.points.size()); ++i) {
                    const auto& p = result.points[i];
                    Point_2 q(p.x() + dx * double(Nx), p.y() + dy * double(Ny));
                    tiled_points.emplace_back(q);
                    idx_by_point.emplace(q, i); // each cell points to the index in the original result.points
                }
            }
        }

        dt.insert(tiled_points.begin(), tiled_points.end());


        // 3. Extract edges and faces
        std::set<std::pair<int,int>> edge_set;
        std::set<std::array<int,3>> face_set;
        for (Triangulation_2::Finite_faces_iterator fit = dt.finite_faces_begin(); fit != dt.finite_faces_end(); ++fit) {
            Point_2 p1 = fit->vertex(0)->point();
            Point_2 p2 = fit->vertex(1)->point();
            Point_2 p3 = fit->vertex(2)->point();
            Point_2 p_tmp =  CGAL::circumcenter(p1, p2, p3);
            if (    p_tmp.x()>=Nx or
                    p_tmp.x()<0 or
                    p_tmp.y()>=Ny or
                    p_tmp.y()<0
                    ) continue; //do not take into account triangles close to the corners
            if (CGAL::collinear(p1, p2, p3))  continue; // skip the collinear triangles
            int i0 = idx_by_point.at(p1);
            int i1 = idx_by_point.at(p2);
            int i2 = idx_by_point.at(p3);
            // skip the degenerated triangles
            if (i0 == i1 || i1 == i2 || i0 == i2) continue;
            std::array<int,3> tri = {i0, i1, i2};
            std::sort(tri.begin(), tri.end());
            face_set.emplace(tri);
            edge_set.emplace(tri[0],tri[1]);
            edge_set.emplace(tri[1],tri[2]);
            edge_set.emplace(tri[0],tri[2]);
        }
        result.faces.reserve(face_set.size());
        result.edges.reserve(edge_set.size());
        for (const auto& f : face_set) result.faces.push_back(f);
        for (const auto& e : edge_set) result.edges.emplace_back(e);


        return result;
    }




    inline void create_2D_triangulation(Network& S, Int N_x, Int N_y) {

        S.log.log(std::format("Creating network form triangulation 2D {} x {}",N_x,N_y));

        // 1. Triangulation and point generator
        TriangulationResult2D result;
        TriangulationResult2D my_triangulation = generate_and_triangulate_periodic(N_x, N_y, result,S);

        S.log.log(std::format("Generated {} points.",result.points.size()));
        S.log.log(std::format("Found {} triangles (faces).",my_triangulation.faces.size()));
        S.log.log(std::format("Found {} pores (points).",result.edges.size()));

        // 2. Network elements memory reservation
        S.nodes.reserve(result.points.size());
        S.pores.reserve(result.edges.size());
        S.grains.reserve(result.faces.size());


        //3. Nodes generation
        int name_tmp = 0;
        for (int i = 0; i < (int)result.points.size(); ++i){
            auto p_tmp = &result.points[i];
            S.nodes.emplace_back(S.config,
                                 S.t_config,
                                 ElementConfig{.type=ElementType::NODE,.name=name_tmp,.log = S.log});
            S.nodes.back().set_pos({p_tmp->x()*1._L,p_tmp->y()*1._L,0._L});
            name_tmp++;
        }

        //4. Pores generation
        name_tmp=0;
        for(auto& p : result.edges){
            S.pores.emplace_back(S.config,
                                 S.t_config,
                                 ElementConfig{.type=ElementType::PORE,.name=name_tmp,.log = S.log});
            S.pores.back().nodes = {&(S.nodes.at(p.first)),&(S.nodes.at(p.second))};
            if(p.first>= S.nodes.size() or p.second >= S.nodes.size())
                S.log.log<LogLevel::ERROR>(std::format("Problem in setting nodes  {} and {}.",p.second,p.first));
            name_tmp++;

        }

        //5. Grain generation
        name_tmp=0;
        for(auto& g : result.faces){
            S.grains.emplace_back(S.config,
                                  S.t_config,
                                  ElementConfig{.type=ElementType::GRAIN,.name=name_tmp,.log = S.log});
            S.grains.back().nodes={&S.nodes.at(std::get<0>(g)),
                                   &S.nodes.at(std::get<1>(g)),
                                   &S.nodes.at(std::get<2>(g))};
            if(std::get<0>(g)>= S.nodes.size() or std::get<1>(g) >= S.nodes.size() or std::get<2>(g) >= S.nodes.size())
                S.log.log<LogLevel::ERROR>(std::format("Problem with setting nodes."));
            name_tmp++;

        }

        //6. adding rest of the topology info
        //adding node -> pore info and node -> node
        for (auto& p : S.pores) {
            p.nodes[0]->pores.push_back(&p);
            p.nodes[1]->pores.push_back(&p);
            p.nodes[0]->nodes.push_back(p.nodes[1]);
            p.nodes[1]->nodes.push_back(p.nodes[0]);
        }

        //adding node -> grain info
        for (auto& g_tmp : S.grains)
            for (auto n_tmp : g_tmp.nodes)
                n_tmp->grains.push_back(&g_tmp);



        //adding pore -> pore info
        for (auto& p_tmp : S.pores)
            for (auto n_tmp : p_tmp.nodes)
                for (auto pp : n_tmp->pores)
                    if(pp!=&p_tmp)
                        p_tmp.pores.push_back(pp);

        //adding grain -> grain info
        for (auto& g_tmp : S.grains)
            for (auto n_tmp : g_tmp.nodes)
                for (auto gg : n_tmp->grains)
                    if(gg!=&g_tmp)
                        g_tmp.grains.push_back(gg);

        // 7.1 Setting node pores for each node
        for(auto& n : S.nodes)
            n.set_node_pores();

        // 7.2 adding grain -> pore info
        for (auto& g : S.grains) {
            size_t size = g.nodes.size();
            for ( size_t i = 0; i < g.nodes.size(); i++){
                auto p = Network::find_pore(g.nodes[(i+1) % size],g.nodes[i]);
                if(p!= nullptr) g.pores.push_back(p);
            }
        }

        //adding pore -> grain info
        for (auto& g_tmp : S.grains)
            for (auto p_tmp : g_tmp.pores)
                p_tmp->grains.push_back(&g_tmp);


        // 8. inlet/outlet information:
        S.log.log("Setting inlets and outlets...");
        if (S.t_config.do_radial_geometry){
            auto n_in = S.find_the_closest_node({Length(N_x/2.),Length(N_y/2.),0._L});
            S.n_inlet.push_back(n_in);

            auto R_min = S.config.l0*(std::min(N_x,N_y)/2.) -3._L;
            S.n_outlet.reserve(N_x*N_y/2);
            for(auto& n : S.nodes)
                if(n.get_pos() - n_in->get_pos() > R_min)
                    S.n_outlet.push_back(&n);
        }
        else{
            if(S.t_config.point_inlet){
                auto n_in = S.find_the_closest_node({Length(N_x/2.),Length(0.),0._L});
                S.n_inlet.push_back(n_in);
            }
            else{
                int inlet_size=0;
                for (auto &n : S.nodes)
                    if(int(double(n.pos.y))<1) inlet_size++;
                    else break;
                S.n_inlet.reserve(inlet_size);
                for (auto &n : S.nodes)
                    if(int(double(n.pos.y))<1) S.n_inlet.push_back(&n);
                    else break;
            }
            if(S.t_config.point_outlet){
                auto n_out = S.find_the_closest_node({Length(0.),Length(N_y/2.),0._L});
                S.n_outlet.push_back(n_out);
            }

            else{
                int outlet_size=0;
                for (auto &n : S.nodes  | std::views::reverse )
                    if(int(double(n.pos.y))>=N_y-1) outlet_size++;
                    else break;
                S.n_outlet.reserve(outlet_size);
                for (auto &n : S.nodes | std::views::reverse )
                    if(int(double(n.pos.y))>=N_y-1) S.n_outlet.push_back(&n);
                    else break;
            }
        }
        for (auto n : S.n_inlet)   n->set_type(NodeType::INPUT);
        for (auto n : S.n_outlet)  n->set_type(NodeType::OUTPUT);
    }

}



#endif //KARST_4_0_CREATE_2D_TRIANGULATION_H
