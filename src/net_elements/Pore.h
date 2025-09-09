# ifndef KARST_4_0_PORE_H
# define KARST_4_0_PORE_H


#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/GenericElement.h"
#include "src/import_export/printing_primitives.h"

namespace karst {

    struct PoreState  {

        Length d{0._L};        ///< pore diameter
        Length l{0._L};        ///< pore length
        Flow   q{0._F};        ///< flow through the pore

        PoreGeometry geometry {PoreGeometry::CYLINDER};
        PoreType     type     {PoreType::MATRIX};


    };


    class Pore : public GenericElement <Pore, PoreState> {

    public:

        friend  GenericElement <Pore, PoreState>;

        explicit Pore  (const NetworkConfig &net_conf,
                        const NetworkTopologyConfig &topo_conf0,
                        const ElementConfig config0)
                        : GenericElement<Pore, PoreState>(net_conf,topo_conf0,config0){}


        auto set_d (Length d0)  -> void  {state.d = d0; }
        //auto add_d (const Length& dd)  -> void  {state.d +=  dd; }
        auto add_d (Length dd)  -> void  {state.d = state.d + dd; }
        auto set_l (Length l0)  -> void  {state.l = l0; }
        auto set_q (Flow q0  )  -> void  {state.q = q0; }
        auto set_type     (PoreType t)     -> void  {state.type = t;};
        auto set_geometry (PoreGeometry g) -> void  {state.geometry = g;};

        auto get_d               () const -> Length       { ASSERT_MSG(state.d>=0._L,      get_context_info()); return state.d; }
        auto get_l               () const -> Length       { ASSERT_MSG(state.l>=0._L,      get_context_info()); return state.l; }
        auto get_q               () const -> Flow         { ASSERT_MSG(abs(state.q)>=0._F, get_context_info()); return abs(state.q); }
        auto get_l_max           () const -> Length       { return *nodes[1] - *nodes[0];} //TODO: can be a field in state changing after merging!
        auto check_if_space_left () const -> bool         { return state.d > 0._L; }
        auto get_type            () const -> PoreType     { return state.type;};
        auto get_geometry        () const -> PoreGeometry { return state.geometry;};


        auto get_perm            () const    {      //we can do some mixing of different regimes
            switch (state.geometry) {
                case PoreGeometry::CYLINDER: return power<4>(state.d)/state.l/net_config.mu_0;
                case PoreGeometry::THICK_A:  return power<3>(net_config.h_tot)*state.d/state.l/net_config.mu_0;
                case PoreGeometry::THIN_A:   return power<3>(state.d)*(net_config.h_tot)/state.l/net_config.mu_0;
                case PoreGeometry::U_SHAPE:  return power<4>(state.d)/state.l/net_config.mu_0;
                case PoreGeometry::THROAT:   return power<4>(state.d)/state.l/net_config.mu_0;
                case PoreGeometry::SIZE:     return Permeability {NaN};

            }
            return Permeability {NaN};
        }

        //auto get_SH_eff    (SOLIDS sp, Reaction R) const  -> Unitless ;         //Rethink if needed - NIE CHCE ICH
        //auto get_DA_eff    (SOLIDS sp, Reaction R) const  -> Unitless ;         //Rethink if needed

        //This one will give the information about passivation (would it handel a hysteresis?)
        auto get_surface   (SOLIDS sp) const  -> Area;

        auto get_surface_tot         () const -> Area {           //return total surface
            switch (state.geometry) {
                case PoreGeometry::CYLINDER: return std::numbers::pi * state.d * state.l;
                case PoreGeometry::THICK_A:  return net_config.h_tot * state.l;
                case PoreGeometry::U_SHAPE:  return 3 * state.d * state.l;
                case PoreGeometry::THIN_A:   return net_config.h_tot * state.l;
                case PoreGeometry::THROAT:   return std::numbers::pi * state.d * state.l;
                case PoreGeometry::SIZE:     return Area{NaN};
            }
            return Area{NaN};
        }



        auto get_new_d_l(Area a, Permeability k) -> std::pair<Length,Length>{
            switch (state.geometry) {
                case PoreGeometry::CYLINDER:  return { power<std::ratio< 1,5>> (a*k*net_config.mu_0/std::numbers::pi),
                                                       power<std::ratio<-1,5>> (k*net_config.mu_0)*power<std::ratio<4,5>>(a/std::numbers::pi)};
                case PoreGeometry::THICK_A:   return { a*k*net_config.mu_0/(2*power<4>(net_config.h_tot)),
                                                       a/(2*net_config.h_tot)};
                case PoreGeometry::U_SHAPE:   return { power<std::ratio< 1,5>> (a*k*net_config.mu_0/3),
                                                       power<std::ratio<-1,5>> (k*net_config.mu_0)*power<std::ratio<4,5>>(a/3)};
                case PoreGeometry::THIN_A:    return { power<std::ratio< 1,3>> (a*k*net_config.mu_0/(2*net_config.h_tot*net_config.h_tot)),
                                                       a/(2*net_config.h_tot)};
                case PoreGeometry::THROAT:    return { power<std::ratio< 1,5>> (a*k*net_config.mu_0/std::numbers::pi),
                                                       power<std::ratio<-1,5>> (k*net_config.mu_0)*power<std::ratio<4,5>>(a/std::numbers::pi)};
                default:      return { Length{NaN}, Length{NaN}};
            }
        };

        auto update_length() -> void;
        auto get_available_volume(SOLIDS species) -> Volume;
        //friend ofstream_ps_pores  &operator <<(ofstream_ps_pores &stream, const Pore &p){}     //TODO: implement it
        //friend ofstream_ps_grains &operator<<(ofstream_ps_grains &stream, const Pore &p){}     //TODO: implement it


    protected:

        void do_disconnect_from_network();

        //init function
        auto do_init() -> void
        {
            if (state.d > 0._L and state.l > 0._L) return;        // do nth, is state is basically set

            state = (PoreState{
                    .d = net_config.d0,
                    .l = *nodes[0] - *nodes[1],
                    .q = Flow(NaN)});

            update_geometry();

            //add randomness to diamaeters //TODO: add randomness to init diameters

        }


        auto do_is_state_set() -> bool{
            return (
                    !std::isnan(double(state.d)) and
                    !std::isnan(double(state.l)) and
                    !std::isnan(double(state.q))
            );
        }

        auto update_geometry() -> void{

            if(state.geometry == PoreGeometry::THROAT) return;  //in throat approach we do not change the geometry

            if(state.type == PoreType::MATRIX){
                if(state.d >= net_config.h_tot)
                    state.geometry = PoreGeometry::THICK_A;
                else
                    state.geometry = PoreGeometry::CYLINDER;
            }
            if(state.type == PoreType::FRACTURE){
                if(state.d >= net_config.h_tot)
                    state.geometry = PoreGeometry::THICK_A;
                else
                    state.geometry = PoreGeometry::THIN_A;
            }
        }

        std::string do_get_state_info() const{
            return  "\n.d   = " + state.d +
                    "\n.l   = " + state.l +
                    "\n.q   = " + state.q +
                    "\n.geo = " + state.geometry;
        }

        };



} // namespace karst



# endif  //KARST_4_0_PORE_H
