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

    };


    class Pore : public GenericElement <Pore, PoreState> {

    public:

        friend  GenericElement <Pore, PoreState>;

        explicit Pore  (const NetworkConfig &net_conf,
                        const NetworkTopologyConfig &topo_conf0,
                        const ElementConfig config0)
                        : GenericElement<Pore, PoreState>(net_conf,topo_conf0,config0){}



        [[nodiscard]] auto check_if_space_left () const-> bool     { return state.d > 0._L; }
        [[nodiscard]] auto get_d               () const -> Length  { return state.d; }
        [[nodiscard]] auto get_l               () const -> Length  { return state.l; }
        [[nodiscard]] auto get_q               () const -> Flow    { return state.q; }




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


        };



} // namespace karst



# endif  //KARST_4_0_PORE_H
