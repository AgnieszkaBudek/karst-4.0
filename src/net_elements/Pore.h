# ifndef KARST_4_0_PORE_H
# define KARST_4_0_PORE_H


#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/Node.h"
#include "src/network/Network.h"

#include "src/net_elements/GenericElement.h"

namespace karst {

    struct PoreState  {

        Length d{NaN};        ///< pore diameter
        Length l{NaN};        ///< pore length
        Flow   q{NaN};        ///< flow through the pore

    };


    class Pore : public GenericElement <Pore, PoreState> {

    public:

        explicit Pore  (const Network &S0, const ElementConfig config0) : GenericElement<Pore, PoreState>(S0,config0){}

        friend  GenericElement <Pore, PoreState>;


        inline auto check_if_active() const  -> bool  {}   //TODO: implement it

        inline auto check_if_space_left() const-> bool    { return s.d > 0._L; }

        inline auto get_d() const -> Length  { return s.d; }

        inline auto get_l() const -> Length  { return s.l; }

        inline auto get_q() const -> Flow    { return s.q; }



        //init function
        auto init() -> void
        {
            s = (PoreState{
                    .d = S.config.d0,
                    .l = n[0]->get_pos() - n[1]->get_pos(),
                    .q = Flow(NaN)});

            //add randomness to diamaeters //TODO: add randomness to init diameters

            //add barrier between inlet and outlet (cutting vertical boundary conditions)
            if( std::abs(n[0]->get_pos().y - n[1]->get_pos().y) > S.t_config.N_y/2.)
                s.d = Length(0);
            if( n[0]->get_type() != Node::NodeType::NORMAL &&  n[0]->get_type() != Node::NodeType::NORMAL)
                s.d = Length(0);

        }


        //export functions:
        friend log_stream& operator<<(log_stream& os, const Pore& obj) {
            os <<  obj.config.type << ": "<< obj.config.name << std::endl;
            os <<"\td = "<<obj.s.d<<"\tl = "<<obj.s.l<<"\tq = "<<obj.s.q;
            os << std::endl << std::endl;
            return os;
        }

        friend ofstream_ps_pores &operator <<(ofstream_ps_pores &stream, const Pore &p){}     //TODO: implement it
        friend ofstream_ps_grains &operator<<(ofstream_ps_grains &stream, const Pore &p){}    //TODO: implement it

    protected:

        };





} // namespace karst



# endif  //KARST_4_0_PORE_H
