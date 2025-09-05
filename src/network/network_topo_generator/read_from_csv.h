//
// Created by Agnieszka on 02/09/2025.
//

#ifndef KARST_4_0_READ_FROM_CSV_H
#define KARST_4_0_READ_FROM_CSV_H

#include "src/external_algorithms/CsvParser/csv.h"
#include "src/network/Network.h"

//using namespace scv;
namespace karst {

    inline void read_csv_H_data(Network& S) {

        S.log.log("Reading data from csv (H style)...");


        // Read files
        csv::CSVReader csv_n(S.t_config.nodes_csv);
        csv::CSVReader csv_p(S.t_config.pores_csv);
        auto info_nodes = csv::get_file_info(S.t_config.nodes_csv);
        auto info_pores = csv::get_file_info(S.t_config.pores_csv);
        auto N_nodes = info_nodes.n_rows;
        auto N_pores = info_pores.n_rows;

        S.log.log(std::format("Total number of nodes read: {}",N_nodes));
        S.log.log(std::format("Total number of pores read: {}",N_pores));


        S.nodes .reserve(N_nodes);
        S.pores .reserve(N_pores);
        S.grains.reserve(N_pores); //each pore has its own "grain"


        //for setting system size
        Length x_min{Length(std::numeric_limits<double>::max())}, x_max{Length(std::numeric_limits<double>::min())};
        Length y_min{Length(std::numeric_limits<double>::max())}, y_max{Length(std::numeric_limits<double>::min())};
        Length z_min{Length(std::numeric_limits<double>::max())}, z_max{Length(std::numeric_limits<double>::min())};


        std::vector<Area> n_area; n_area.reserve(N_nodes);
        Volume V_all_pores  = 0._V;      //total volume of pores
        Volume V_grains_tmp = 0._V;      //temporal grain volume

        //reading info about nodes
        for (auto& row : csv_n) {
            S.nodes.emplace_back(S.config, S.t_config,
                                 ElementConfig{.type=ElementType::NODE, .name=row["ID"].get<int>(), .log = S.log});
            S.nodes.back().set_pos(Point3D{
                                            .x = row["X"].get<double>()*1._L,
                                            .y = row["Y"].get<double>()*1._L,
                                            .z = row["Z"].get<double>()*1._L});
            n_area.push_back(row["Area"].get<double>()*1._A);       //saving info about areas
            V_all_pores+=row["Volume"].get<double>()*1._V;

            //for assessing system size
            if( row["X"].get<double>()*1._L  - row["EqRadius"].get<double>()*1._L < x_min )
                x_min =  row["X"].get<double>()*1._L  - row["EqRadius"].get<double>()*1._L;
            if( row["X"].get<double>()*1._L  + row["EqRadius"].get<double>()*1._L > x_max )
                x_max =  row["X"].get<double>()*1._L  + row["EqRadius"].get<double>()*1._L;
            if( row["Y"].get<double>()*1._L  - row["EqRadius"].get<double>()*1._L < y_min )
                y_min =  row["Y"].get<double>()*1._L  - row["EqRadius"].get<double>()*1._L;
            if( row["Y"].get<double>()*1._L  + row["EqRadius"].get<double>()*1._L > y_max )
                y_max =  row["Y"].get<double>()*1._L  + row["EqRadius"].get<double>()*1._L;
            if( row["Z"].get<double>()*1._L  - row["EqRadius"].get<double>()*1._L < z_min )
                z_min =  row["Z"].get<double>()*1._L  - row["EqRadius"].get<double>()*1._L;
            if( row["Z"].get<double>()*1._L  + row["EqRadius"].get<double>()*1._L > z_max )
                z_max =  row["Z"].get<double>()*1._L  + row["EqRadius"].get<double>()*1._L;
        }

        auto L_x = x_max - x_min;
        auto L_y = y_max - y_min;
        auto L_z = z_max - z_min;

        for(auto& n : S.nodes){
            n.set_pos({n.pos.x-x_min,n.pos.y-y_min,n.pos.z-z_min});
        }

        //reading info about pores
        for (auto& row : csv_p) {
            S.pores.emplace_back(S.config, S.t_config,
                                 ElementConfig{.type=ElementType::PORE, .name=row["ID"].get<int>(), .log = S.log});
            auto& pore_tmp = S.pores.back();
            pore_tmp.set_nodes ({& S.nodes[row["N1"].get<int>()],& S.nodes[row["N2"].get<int>()]});
            pore_tmp.set_geometry(PoreGeometry::THROAT);

            //setting info about effective l and d
            auto eq_r = row["EqRadius"].get<double>()*0.5_L;
            auto eq_l = row["Length"].get<double>()*1._L;
            auto area = 0.5*(n_area[row["N1"].get<int>()] + n_area[row["N2"].get<int>()]);
            auto d_l  = pore_tmp.get_new_d_l(area, power<4>(2*eq_r)/(eq_l*S.config.mu_0));
            pore_tmp.set_d(d_l.first);
            pore_tmp.set_l(d_l.second);

            S.grains.emplace_back(S.config, S.t_config,
                                 ElementConfig{.type=ElementType::GRAIN, .name=row["ID"].get<int>(), .log = S.log});
            S.grains.back().set_pores({&pore_tmp});
            S.grains.back().set_nodes({pore_tmp.get_nodes()[0],pore_tmp.get_nodes()[1]});
            S.grains.back().set_v_max(pore_tmp.get_l_max()*area);
            V_grains_tmp+=pore_tmp.get_l_max()*area;
        }

        //rescaling grain volumes
        auto porosity_0 = V_all_pores/(L_x*L_y*L_z);
        auto volume_factor = V_grains_tmp/(L_x*L_y*L_z);
        S.log.log(std::format("Initial porosity is set to {}", double(porosity_0)));

        for (auto& grain : S.grains){
            grain.set_tot_v(grain.get_max_volume()*volume_factor*porosity_0);
            grain.set_v_max(grain.get_tot_v()*power<3>(grain.get_pores()[0]->get_l_max()/grain.get_pores()[0]->get_l()));  //to be coherent with pore length calcualtions
            grain.set_v(SOLIDS::X, grain.get_tot_v() * S.config.X_f);
            grain.set_v(SOLIDS::A, grain.get_tot_v() - grain.get_v(SOLIDS::X));
            for (auto sp : S.config.solidS){
                if (sp != SOLIDS::A and sp!= SOLIDS::X)
                    grain.set_v(sp,0.0_V);
            }
        }


        //adding rest of the topology info

        //adding node -> pore info and node->node
        for (auto& p_tmp : S.pores)
            for (auto n_tmp : p_tmp.nodes){
                n_tmp->pores.push_back(&p_tmp);
                if(p_tmp.nodes[0]==n_tmp) n_tmp->nodes.push_back(p_tmp.nodes[1]);
                else                      n_tmp->nodes.push_back(p_tmp.nodes[0]);
            }
        for (auto& n_tmp : S.nodes)
            n_tmp.set_node_pores();

        //adding node -> grain info
        for (auto& g_tmp : S.grains)
            for (auto n_tmp : g_tmp.nodes)
                n_tmp->grains.push_back(&g_tmp);

        //adding pore -> grain info
        for (auto& g_tmp : S.grains)
            for (auto p_tmp : g_tmp.pores)
                p_tmp->grains.push_back(&g_tmp);

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


        //Adding inlet/outlet information
        if(S.t_config.point_inlet)
            S.n_inlet.push_back(S.find_the_closest_node({L_x/2,L_y/2,0._L}));       //chack the orientation
        else{
            for (auto& n : S.nodes)
                if(n.get_pos().z < L_z/pow(int(S.nodes.size()),0.3))
                S.n_inlet.push_back(&n);
        }
        if(S.t_config.point_outlet)
            S.n_outlet.push_back(S.find_the_closest_node({L_x/2,L_y/2,L_z}));       //chack the orientation
        else{
            for (auto& n : S.nodes)
                if(n.get_pos().z >  L_z*(1-1./pow(int(S.nodes.size()),0.3)))
                    S.n_outlet.push_back(&n);
        }

        S.log.log(std::format("Number of inlets  is equal to {}", S.n_inlet. size()));
        S.log.log(std::format("Number of outlets is equal to {}", S.n_outlet.size()));

        //temporary setting zero state
        for(auto& n : S.nodes)
            n.set_u(0._P);
        for(auto& p : S.pores)
            p.set_q(0._F);

        S.io_mod.save_VTU(S.nodes,S.pores,S.grains,false);


    }
}
#endif //KARST_4_0_READ_FROM_CSV_H
