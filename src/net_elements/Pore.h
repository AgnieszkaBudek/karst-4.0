# ifndef KARST_4_0_PORE_H
# define KARST_4_0_PORE_H


#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/GenericElement.h"
#include "src/import_export/printing_primitives.h"

namespace karst {

    struct PoreState  {

        Length d{NaN};        ///< pore diameter
        Length l{NaN};        ///< pore length
        Flow   q{NaN};        ///< flow through the pore

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




        auto set_d (Length d0) -> void  {state.d = d0; }
        auto add_d (Length d0) -> void  {state.d += d0; }
        auto set_l (Length l0) -> void  {state.l = l0; }
        auto set_q (Flow q0  ) -> void  {state.q = q0; }

        auto get_d               () const -> Length  { return state.d; }
        auto get_l               () const -> Length  { return state.l; }
        auto get_q               () const -> Flow    { return state.q; }
        auto get_l_max           () const -> Length  { return nodes[1]->get_pos()-nodes[0]->get_pos();}
        auto check_if_space_left () const-> bool     { return state.d > 0._L; }

        auto get_perm            () const    {
            switch (state.geometry) {
                case PoreGeometry::CYLINDER: return power<4>(state.d)/state.l/net_config.mu_0;
                case PoreGeometry::THICK_A:  return power<3>(net_config.h_tot)*state.d/state.l/net_config.mu_0;
                case PoreGeometry::THIN_A:   return power<3>(state.d)*(net_config.h_tot)/state.l/net_config.mu_0;
                case PoreGeometry::U_SHAPE:  return power<4>(state.d)/state.l/net_config.mu_0;
                case PoreGeometry::SIZE:     return Permeability {NaN};

            }
        }

        auto get_surface   (SOLIDS sp) const  -> Area;
        auto get_surface_tot         () const -> Area {           //return total surface
            switch (state.geometry) {
                case PoreGeometry::CYLINDER: return std::numbers::pi * state.d * state.l;
                case PoreGeometry::THICK_A:  return net_config.h_tot * state.l;
                case PoreGeometry::U_SHAPE:  return 3 * state.d * state.l;
                case PoreGeometry::THIN_A:   return net_config.h_tot * state.l;
                case PoreGeometry::SIZE:     return Area{NaN};
            }
        }


        auto get_volume         () const -> Volume {
            switch (state.geometry) {
                case PoreGeometry::CYLINDER: return 1/4.*std::numbers::pi * state.d * state.d * state.l;
                case PoreGeometry::THICK_A:  return net_config.h_tot * state.l * state.l;
                case PoreGeometry::U_SHAPE:  return state.d * state.d * state.l;
                case PoreGeometry::THIN_A:   return net_config.h_tot * state.l * state.l;
                case PoreGeometry::SIZE:     return Volume {NaN};
            }
        }

        auto get_available_volume(SOLIDS species) -> Volume;


        //export functions:
        inline friend std::ostream& operator<<(std::ostream& os, const Pore& obj) {
            os <<  obj.config.type << ": "<< obj.config.name << std::endl;
            os << "\td = " << obj.state.d << "\tl = " << obj.state.l << "\tq = " << obj.state.q;
            os << std::endl << std::endl;
            return os;
        }

        //friend ofstream_ps_pores  &operator <<(ofstream_ps_pores &stream, const Pore &p){}     //TODO: implement it
        //friend ofstream_ps_grains &operator<<(ofstream_ps_grains &stream, const Pore &p){}     //TODO: implement it


    protected:

        void do_disconnect_from_network();

        //init function
        auto do_init() -> void
        {
            state = (PoreState{
                    .d = net_config.d0,
                    .l = nodes[0]->get_pos() - nodes[1]->get_pos(),
                    .q = Flow(NaN)});

            update_geometry();

            //add randomness to diamaeters //TODO: add randomness to init diameters

            //std::cerr << "Initializing Pore: " << *this;

        }


        auto do_is_state_set() -> bool{
            return (
                    !std::isnan(double(state.d)) and
                    !std::isnan(double(state.l)) and
                    !std::isnan(double(state.q))
            );
        }

        auto update_geometry() -> void{
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

        };



} // namespace karst



# endif  //KARST_4_0_PORE_H
