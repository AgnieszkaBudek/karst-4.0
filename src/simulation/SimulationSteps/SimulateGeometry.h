//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATEGEOMETRY_H
#define KARST_4_0_SIMULATEGEOMETRY_H

#include "src/simulation//SimulationSteps/GenericSimulationStep.h"

namespace karst {


    struct SimulateGeometryState {
        Volume epsilon_V = 1.e-5_V;
    };

    class SimulateGeometry : public GenericSimulationStep <SimulateGeometry,SimulateGeometryState> {

    public:
        friend class GenericSimulationStep<SimulateGeometry, SimulateGeometryState>;
        using GenericSimulationStep::GenericSimulationStep;

    protected:

        auto do_init() -> void {name = "SimulateGeometry";}

        auto do_run()  -> void {

            //distribute volume to gains
            for(auto& p : S.get_pores())
                for(auto& sp : S.config.solidS){
                    Volume V_tmp = R.get_delta_volume_map(sp)(p);                       //total change of volume of species sp
                    auto count = std::ranges::count_if(p.get_grains(), [&](auto g) {    //number of gains that take part in reaction
                        return (g->if_species_left(sp) or V_tmp>0._V);});
                    for(auto g : p.get_grains()) if(g->if_species_left(sp) or V_tmp>0._V){            //distributing volume into those grains
                        g->add_v(sp, 1./double(count) * V_tmp);
                        if(g->get_v(sp) < state.epsilon_V) g->set_v(sp,0._V);
                    }
                }


            //update diameters
            for(auto& p : S.get_pores()){
                Volume V_tot = 0._V;
                for(auto& sp : S.config.solidS)
                    V_tot += R.get_delta_volume_map(sp)(p);
                p.add_d(2*V_tot/p.get_surface_tot());
            }


            //update lengths
            for(auto& p : S.get_pores()){
                Unitless l_tot_f = 0._U;
                for(auto& g : p.get_grains())
                    l_tot_f +=  1._U*pow(double(g->get_tot_v()/g->get_max_volume()),S.config.l_V_scaling_f);
                p.set_l(1./double(p.get_grains().size())*p.get_l_max()*l_tot_f);
                if(p.get_l()<S.config.l_min) p.set_l(S.config.l_min);       //pore length cannot be too small
            }
        }

        auto do_check()-> bool {

            EnumArray<SOLUBLES,CFlux,enum_size_v<SOLUBLES>> CF_in {0._X};
            EnumArray<SOLUBLES,CFlux,enum_size_v<SOLUBLES>> CF_out{0._X};

            return true;

        }



    };
} // namespace karst



#endif //KARST_4_0_SIMULATEGEOMETRY_H
