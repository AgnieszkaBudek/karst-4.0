//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATEPRESSURE_H
#define KARST_4_0_SIMULATEPRESSURE_H

#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>

#include "src/dynamics/SimulationSteps/GenericSimulationStep.h"
#include "external_algorithms/algorithms_cc.h"

namespace karst {


    struct SimulatePressureState {
        MatrixSolver         matrix_solver_type = MatrixSolver::EIGEN;
        Pressure_Algorithm   algorithm          = Pressure_Algorithm::CLASSIC;
        Long N_active;      ///< number of active nodes
        Long N_non_zero;    ///< number of nonzero element of matrix M
    };



    class SimulatePressure : public GenericSimulationStep <SimulatePressure,SimulatePressureState> {


    protected:

        using T = double;
        using SpMat = Eigen::SparseMatrix<T>;
        using TripletT = Eigen::Triplet<T>;
        std::vector<TripletT> triplets{};
        Eigen::VectorXd x;
        Eigen::VectorXd y;
        SpMat M;


        auto do_init() -> void {
            name = "SimulatePressure";
            do_update_state();
        }

        auto do_update_state() -> void{
            state.N_active   = S.get_state().N_active;
            state.N_non_zero = S.get_state().N_active_connections + state.N_active;
            triplets.reserve(state.N_non_zero);
            triplets.clear();
            x.resize(state.N_active);     x.setZero();
            y.resize(state.N_active);     y.setZero();
            M.resize(state.N_active,state.N_active);

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


        auto set_pressures() -> void{
            int i_tmp=0;
            for(auto n : S.get_nodes()){
                assert(n.config.a_name == i_tmp);
                n.set_u(Pressure(x[i_tmp++]));
            }
        }


        auto run_EIGEN()->void{

            M.resize(state.N_active,state.N_active);
            M.setFromTriplets(triplets.begin(), triplets.end());

            // Solver: Conjugate Gradient + Incomplete Cholesky
            Eigen::ConjugateGradient<SpMat, Eigen::Lower|Eigen::Upper, Eigen::IncompleteCholesky<T>> solver;
            solver.compute(M);

            if (solver.info() != Eigen::Success) {
                std::cerr << "Eigen:: Decomposition failed.\n";
            }
            x = solver.solveWithGuess(y, x);
            if (solver.info() != Eigen::Success) {
                std::cerr << "Eigen:: Solver failed.\n";
            }
        }

        auto prepare_matrix_for_EIGEN() -> void{

            triplets.clear();

            for(auto n : S.get_nodes()){

                int name_tmp = n.config.a_name;
                if (n.get_type()==NodeType::INPUT)  y[name_tmp]  = double(S.get_state().P_in);
                if (n.get_type()==NodeType::OUTPUT) y[name_tmp]  = double(S.get_state().P_out);
                if (n.get_type()!=NodeType::NORMAL) triplets.emplace_back(name_tmp, name_tmp, 1.0);

                for(auto& [nn,pp] : n.get_nodes_pores()) {
                    double perm_v = double(pp->get_perm());
                    triplets.emplace_back(nn->config.a_name, name_tmp, perm_v);
                    triplets.emplace_back(name_tmp, name_tmp, -perm_v);
                }
            }
        }


    };
} // namespace karst



#endif //KARST_4_0_SIMULATEPRESSURE_H
