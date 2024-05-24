# ifndef KARST_4_0_PORE_H
# define KARST_4_0_PORE_H


#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/GenericElement.h"

namespace karst {

    struct PoreState  {

        Length d{0};        ///< pore diameter
        Length l{0};        ///< pore length
        Flow   q{0};        ///< flow through the pore

    };


    class Pore : public GenericElement <Pore, PoreState> {



    public:

        friend  GenericElement <Pore, PoreState>;

        auto check_if_active() -> bool {

        }



    protected:

        };


} // namespace karst

# endif  //KARST_4_0_PORE_H
