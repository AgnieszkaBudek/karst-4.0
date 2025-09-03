//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_PRINTING_PS_H
#define KARST_4_0_PRINTING_PS_H
#include "src/import_export/PrintingModule.h"


namespace karst{


    void PrintingModule::print_net_ps(const std::string title,
                         const std::vector<Node>  &nodes,
                         const std::vector<Pore>  &pores,
                         const std::vector<Grain> &grains)  {

        log.log("Printing postscript...");


        OutStream& stream = net_pores_ps;
        stream.add_page();
        print_ps_headlines(stream,stream.get_page_count(),title);

//            for (const auto& g_tmp : grains)
//                if(g_tmp.check_if_active())
//                    stream << Polygon3D{.n = g_tmp.get_nodes(),.name = std::to_string(g_tmp.config.name)};

        for (const auto& p_tmp : pores)
            if(p_tmp.check_if_active())
                //stream << Pore3D{.p = p_tmp,.name = std::to_string(p_tmp.config.name)};
                stream << Pore3D{.p = p_tmp,.name = std::string("")};

//            for (const auto& n_tmp : nodes)
//                if(n_tmp.check_if_active())
//                    stream << Node3D{.n = n_tmp,.name = std::to_string(n_tmp.config.name)};

        stream << "\nshowpage\n"<<fflush;

    }

    auto PrintingModule::print_ps_headlines (OutStream& stream, int page_nr, const std::string& description_note) -> void{

        int N = (net_topo_config.N_x);
        int M = (net_topo_config.N_y);
        double scale_tmp = 400. / (double(config.L_out) * std::max(N, M));
        double x_zero = 100. / scale_tmp;
        double y_zero = 750. / scale_tmp;

        if(page_nr==1){
            stream << "%!PS-Adobe-3.0\n";
            stream << "%%Pages:100\n";
        }


        stream << "%%Page: " << page_nr << " " << page_nr <<"\n\n";
        stream << "1 setlinejoin 1 setlinecap 0.02 setlinewidth\n";

        stream << scale_tmp << "\t" << scale_tmp << " scale\n";
        stream << x_zero << "\t" << y_zero << " translate\n";
        stream << "1 setlinejoin\n";
        stream << "1 setlinecap\n";

        //title
        stream << Color(0,0,0);
        stream << "/Times-Bold findfont " << 20. / scale_tmp <<" scalefont setfont\n";
        stream << "0 " << -450. / scale_tmp << " moveto\n";
        stream << "0 0 ("<<description_note<<") ashow\n";
        stream << " stroke\n\n";
    }



    template<typename Nodes, typename Pores, typename Grains,typename Fn, typename Fp, typename Fg>
    void PrintingModule::print_net_ps_with_values(   std::string title,
                                                     Nodes &&nodes,
                                                     Pores &&pores,
                                                     Grains &&grains,
                                                     Fn &&f_n,
                                                     Fp &&f_p,
                                                     Fg &&f_g)
    {
        log.log("Printing postscript: debug for " + title + "...");
        OutStream& stream =net_debug_ps;
        stream.add_page();
        print_ps_headlines(stream,stream.get_page_count(),title);


        for (const auto &g_tmp : grains)
            if (g_tmp.check_if_active())
                stream << Polygon3D{.n = g_tmp.get_nodes(),. name = std::forward<Fg>(f_g)(g_tmp)};

        for (const auto &p_tmp : pores)
            if (p_tmp.check_if_active())
                stream << Pore3D{.p = p_tmp, .name = std::forward<Fp>(f_p)(p_tmp)};

        for (const auto &n_tmp : nodes)
            if (n_tmp.check_if_active())
                stream << Node3D{.n = n_tmp, .name = std::forward<Fn>(f_n)(n_tmp)};

        stream << "\nshowpage\n"<<fflush;

    }



}

#endif //KARST_4_0_PRINTING_PS_H
