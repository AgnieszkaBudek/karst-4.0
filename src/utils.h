# ifndef KARST_4_0_UTILS_H
# define KARST_4_0_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <queue>
#include <map>
#include <numeric>
#include <cassert>

#include "src/units.h"
#include "src/chemistry/reaction_config.h"

namespace karst {

    class Network;
    class Pore;
    class Node;
    class Grain;

    const double NaN = std::numeric_limits<double>::quiet_NaN();


    class ofstream_ps         : public std::ofstream  {};
    class ofstream_ps_pores   : public ofstream_ps    {};
    class ofstream_ps_grains  : public ofstream_ps    {};   ///< output PostScript file
    class ofstream_txt        : public std::ofstream  {};   ///< output text file
    class log_stream          : public std::ostream   {};   ///< log files



}

#endif       //KARST_4_0_UTILS_H