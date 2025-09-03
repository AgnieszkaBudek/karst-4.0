//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATEPRESSURE_H
#define KARST_4_0_SIMULATEPRESSURE_H

#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCholesky>

#include "src/simulation//SimulationSteps/GenericSimulationStep.h"
//#include "external_algorithms/algorithms_cc.h"




namespace karst {


    struct SimulatePressureState {
        MatrixSolver         matrix_solver_type = MatrixSolver::EIGEN;
        Int how_often_direct {1};       ///< how often use a direct solver instead of iterative
        Long N_active;                   ///< number of active nodes
        Long N_non_zero;                 ///< number of nonzero element of matrix M

        Long step {0};                   ///< step from the last update of matrix size
        Double cg_solver_tolerance {1e-8};
        Int cg_solver_iterations   {100};
    };



    class SimulatePressure : public GenericSimulationStep <SimulatePressure,SimulatePressureState> {

    public:
        friend class GenericSimulationStep<SimulatePressure, SimulatePressureState>;
        using GenericSimulationStep::GenericSimulationStep;

    protected:

        using T = double;
        using SpMat = Eigen::SparseMatrix<T>;
        using TripletT = Eigen::Triplet<T>;
        std::vector<TripletT> triplets{};
        Eigen::VectorXd x{};
        Eigen::VectorXd y{};
        SpMat M;

        Eigen::ConjugateGradient<SpMat, Eigen::Lower|Eigen::Upper, Eigen::IncompleteCholesky<T>> cg_solver;
        Eigen::SimplicialLLT<SpMat> direct_solver;


        auto do_init() -> void {
            name = "SimulatePressure";
            do_update_state();
        }

        auto do_update_state() -> void{
            state.N_active   = S.get_state().N_active - Long(S.get_inlets().size() + S.get_outlets().size()) ;
            state.N_non_zero = S.get_state().N_active_connections + state.N_active;
            triplets.reserve(state.N_non_zero);
            triplets.clear();
            x.resize(state.N_active);     x.setZero();
            y.resize(state.N_active);     y.setZero();
            M.resize(state.N_active,state.N_active);

            state.step = 0;

            ASSERT_MSG(state.N_active > 0, "state.N_active = "+std::to_string(state.N_active));

        }

        auto do_run()  -> void {

            if (S.get_state().new_Topo)
                do_update_state();

            if(state.matrix_solver_type == MatrixSolver::EIGEN) {
                prepare_matrix_for_EIGEN();
                run_EIGEN();
            }

            set_pressures();
        }

        auto do_check() -> bool{return true;};   //TODO: implement later

        auto do_mix_states() -> void{

            for(auto& n :S.get_nodes()){
                const auto s_new = n.get_state();
                const auto s_old = n.get_old_state();
                n.set_u((s_old.u+s_new.u)/2.);
            }
        }

        auto do_go_back(){
            log.log_with_context<LogLevel::ERROR>(*this,"Not implemented. Shouldn't be necessary.");
        }

        auto set_pressures() -> void{
            int i_tmp=0;
            for(auto& n : S.get_nodes()){
                if(n.get_type()==NodeType::INPUT)  {n.set_u(S.config.P_in);  continue;}
                if(n.get_type()==NodeType::OUTPUT) {n.set_u(S.config.P_out); continue;}
                ASSERT_MSG(n.m_name == i_tmp,"n.m_name = "+std::to_string(n.m_name)+"\ti_tmp = "+std::to_string(i_tmp));
                ASSERT_MSG(x[i_tmp] >0,"x[i_tmp] = "+std::to_string(x[i_tmp]));
                n.set_u(Pressure(x[i_tmp]));
                i_tmp++;
            }
            for(auto& n : S.get_nodes())
                log.log_with_context<LogLevel::DEBUG_FULL>(n," .u = "+n.get_u());;
        }


        auto run_EIGEN()->void{

            M.setFromTriplets(triplets.begin(), triplets.end());
            log.log<LogLevel::DEBUG_FULL>(print_full_matrix());

            if(state.step % state.how_often_direct == 0){       // Solver: Conjugate Gradient + Incomplete Cholesky

                log.log_with_context<LogLevel::DEBUG>(*this,"Running direct solver.");
                direct_solver.compute(M);
                if (direct_solver.info() != Eigen::Success)
                    log.log_with_context<LogLevel::ERROR>(*this,"Eigen::SimplicialLLT Decomposition failed.");
                x = direct_solver.solve(y);
                if (direct_solver.info() != Eigen::Success)
                    log.log_with_context<LogLevel::ERROR>(*this,"Eigen::SimplicialLLT Solver failed.");

            }

            else{           // Solver: Conjugate Gradient + Incomplete Cholesky

                log.log_with_context<LogLevel::DEBUG>(*this,"Running Solver: Conjugate Gradient + Incomplete Cholesky.");

                cg_solver.setTolerance(state.cg_solver_tolerance);
                cg_solver.setMaxIterations(state.cg_solver_iterations);
                cg_solver.compute(M);
                if (cg_solver.info() != Eigen::Success)
                    log.log_with_context<LogLevel::ERROR>(*this,"Eigen::ConjugateGradient Decomposition failed.");
                x = cg_solver.solveWithGuess(y, x);
                if (cg_solver.info() != Eigen::Success)
                    log.log_with_context<LogLevel::ERROR>(*this,"Eigen::ConjugateGradient Solver failed.");
            }
            state.step++;
        }

        auto prepare_matrix_for_EIGEN() -> void {

            triplets.clear();
            y.setZero();
            for(auto& n : S.get_nodes()) {
                int name_tmp = n.m_name;
                if (n.get_type() == NodeType::NORMAL){
                    double sum_tmp = 0.0;
                    for (auto &[nn, pp]: n.get_nodes_pores()) {
                        double perm_v = double(pp->get_perm());
                        sum_tmp+=perm_v;
                        switch (nn->get_type()){
                            case NodeType::NORMAL:
                                triplets.emplace_back(name_tmp, nn->m_name, -perm_v); break;
                            case NodeType::INPUT:
                                y[name_tmp] += double(perm_v*S.config.P_in);  break;
                            case NodeType::OUTPUT:
                                y[name_tmp] += double(perm_v*S.config.P_out); break;
                            case NodeType::SIZE: break;
                        }
                    }
                    triplets.emplace_back(name_tmp, name_tmp, sum_tmp);
                }
            }
            ASSERT_MSG(triplets.size()==state.N_non_zero,"triplets.size = "+std::to_string(triplets.size()) +
                                                        " state.N_non_zero = "+std::to_string(state.N_non_zero));
        }

        std::string do_get_state_info() const{
            return  "\n\t\tN_active             = " + std::to_string(state.N_active)+
                    "\n\t\tN_non_zero           = " + std::to_string(state.N_non_zero)+
                    "\n\t\tcg_solver_iterations = " + std::to_string(state.cg_solver_iterations)+
                    "\n\t\tcg_solver_tolerance  = " + std::to_string(state.cg_solver_tolerance)+
                    "\n\t\thow_often_direct     = " + std::to_string(state.how_often_direct)+
                    "\n\t\tmatrix_solver_type   = " + state.matrix_solver_type;
        }

        std::string print_full_matrix() const{
            std::ostringstream out;
            out <<"\n";

            int rows = int(M.rows());
            int cols = int(M.cols());

            int max_rows = std::min(16, rows);
            int max_cols = std::min(16, cols);

            Eigen::MatrixXd denseM = Eigen::MatrixXd(M);  // konwersja na macierz gęstą

            for (int i = 0; i < max_rows; ++i) {
                for (int j = 0; j < max_cols; ++j) {
                    out
                            << std::fixed
                            << std::setprecision(1)
                            << std::setw(6)    // szerokość pola, dopasuj według potrzeb
                            << std::right
                            << denseM(i, j)
                            << " ";
                }
                out << std::endl;
            }
            return out.str();
        }

        void do_ps_for_debug(std::string& str) const{

            S.io_mod.print_net_ps_with_values(
                    get_context_info() + " " + str,
                    S.get_nodes(),
                    S.get_pores(),
                    std::vector<Grain>{},
                    [](auto& el){return std::format("{:3.2f}",double(el.get_u()));},
                    [](auto& el){return "";},
                    [](auto& el){return "";}
            );
        }

    };
} // namespace karst



#endif //KARST_4_0_SIMULATEPRESSURE_H
