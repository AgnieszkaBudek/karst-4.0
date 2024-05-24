//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_H
#define KARST_4_0_NETWORK_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {


    class Network {

    public:

        explicit Network() = default;

        ~Network() {

            std::cerr << "Dodac do destruktowa upewnienie sie, ze node nie pojawia sie gdzie indziej" << std::endl;
        }



    protected:


    };

}

#endif //KARST_4_0_NETWORK_H
