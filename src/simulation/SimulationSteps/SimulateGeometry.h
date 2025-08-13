//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATEGEOMETRY_H
#define KARST_4_0_SIMULATEGEOMETRY_H

#include "src/simulation//SimulationSteps/GenericSimulationStep.h"

namespace karst {


    struct SimulateGeometryState {
        Volume eps_V = 1.e-5_V;
        EnumArray<SOLUBLES,CFlux,enum_size_v<SOLUBLES>> CF_in {0._X};
        EnumArray<SOLUBLES,CFlux,enum_size_v<SOLUBLES>> CF_out{0._X};
        EnumArray<SOLUBLES,Volume,enum_size_v<SOLUBLES>> Delta_V {0._V};
    };

    class SimulateGeometry : public GenericSimulationStep <SimulateGeometry,SimulateGeometryState> {

    public:
        friend class GenericSimulationStep<SimulateGeometry, SimulateGeometryState>;
        using GenericSimulationStep::GenericSimulationStep;

    protected:

        auto do_init() -> void {name = "SimulateGeometry";}

        auto do_run()  -> void {

            // 1. distribute volume to gains
            for(auto& p : S.get_pores())
                for(auto& sp : S.config.solidS){
                    Volume V_tmp = R.get_delta_volume_map(sp)(p);                       //total change of volume of species sp
                    auto count = std::ranges::count_if(p.get_grains(), [&](auto g) {    //number of gains that take part in reaction
                        return (g->if_species_available(sp) or V_tmp > 0._V);});
                    for(auto g : p.get_grains()) if(g->if_species_available(sp) or V_tmp > 0._V){            //distributing volume into those grains
                        g->add_v(sp, 1./double(count) * V_tmp);
                        if(g->get_v(sp) < state.eps_V) g->set_v(sp, 0._V);
                    }
                }
            for(auto& g : S.get_grains()) g.update_v_tot();

            // 2. update diameters
            for(auto& p : S.get_pores()){
                Volume V_tot = 0._V;
                for(auto& sp : S.config.solidS)
                    V_tot += R.get_delta_volume_map(sp)(p);
                p.add_d(-2*V_tot/p.get_surface_tot());
            }

            log.log(S.get_pores().begin()->get_state_info()+" Before updating length");

            //3. update lengths
            for (auto& p: S.get_pores())
                p.update_length();

        }

        auto do_check()-> bool {

            return true;
        }

        auto do_go_back(){

            for(auto& p : S.get_pores()){
                p.set_d(p.get_old_state().d);
                p.set_l(p.get_old_state().l);
            }

            for(auto& g : S.get_grains()) g.revert_states();

        }

        auto do_mix_states() -> void {           //more complicated that other optwions

            log.log_with_context<LogLevel::WARNING>(*this, "This may not be implemented correctly.");

            for (auto &p: S.get_pores()) {
                p.set_d(p.get_old_state().d);
                p.set_l(p.get_old_state().l);
            }

            // 2. Set new volume based on both new and old states;
            for (auto &g: S.get_grains()) {
                const auto s_new = g.get_state();
                const auto s_old = g.get_old_state();
                for (auto sp: S.config.solidS)
                    g.set_v(sp, (s_old.v[sp] + s_new.v[sp]) / 2.);           //immune to going back
                g.set_tot_v((s_new.tot_volume + s_old.tot_volume) / 2.);
            }

            //3. update diameters
            for (auto &p: S.get_pores()) {
                Volume V_tot = 0._V;
                for (auto &sp: S.config.solidS)
                    V_tot += R.get_delta_volume_map(sp)(p);
                p.add_d(2 * V_tot /
                        p.get_surface_tot());       //TODO: this wont work both state and state_old are in future now...
            }

            //4. update lengths
            for (auto &p: S.get_pores()) p.update_length();

        }


        std::string do_get_state_info() const{
            std:: string str ="\n\t\teps_V = "+state.eps_V;
            for(auto sp : S.config.solubleS)
                str  += "\n\t\tCF["+sp+"] = "+state.CF_in[sp]+" -> "+state.CF_out[sp] + "\tdelta_V["+sp+"] = "+state.Delta_V[sp];
            return str;
        }

        void do_ps_for_debug(std::string& str) const{

            S.io_mod.print_net_ps_with_values(
                    get_context_info() + " " + str,
                    S.get_nodes(),
                    S.get_pores(),
                    S.get_grains(),
                    [](auto& el){return std::format("{:3.2f}",double(el.get_c(SOLUBLES::B)));},
                    [](auto& el){return std::format("({:2.1f}, {:2.2f})",double(el.get_d()),double(el.get_l()));},
                    [](auto& el){return std::format("{:3.2f}",double(el.get_v(SOLIDS::A)));}
                    );
        }


    };
} // namespace karst



#endif //KARST_4_0_SIMULATEGEOMETRY_H
