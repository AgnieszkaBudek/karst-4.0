//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_VTK_WRITE_H
#define KARST_4_0_VTK_WRITE_H

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkStringArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkType.h>
#include <vtkPolygon.h>
#include <vtkTriangle.h>
#include <vtkQuad.h>
#include <vtkPolygon.h>
#include <vtkIdTypeArray.h>
#include <vtkCellType.h>
#include <map>
#include <vector>
#include <string>
#include "src/import_export/PrintingModule.h"

namespace karst {

//    inline void test_vtk_debug() {
//
//            std::cout << "Testing VTK in debug mode..." << std::endl;
//
//            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
//
//            // NIE używaj: std::cout << points << std::endl; // ❌ To nie działa!
//
//            // Zamiast tego użyj:
//            std::cout << "Points object address: " << points.GetPointer() << std::endl;
//            std::cout << "Points reference count: " << points->GetReferenceCount() << std::endl;
//
//            if (points->GetData()) {
//                std::cout << "Points data is valid" << std::endl;
//            } else {
//                std::cout << "Points data is null" << std::endl;
//            }
//
//            // Test InsertNextPoint
//            vtkIdType id = points->InsertNextPoint(1.0, 2.0, 3.0);
//            std::cout << "Inserted point ID: " << id << std::endl;
//            std::cout << "Number of points: " << points->GetNumberOfPoints() << std::endl;
//        }

    template <RangeOf<Node> NodeRange, RangeOf<Pore> PoreRange, RangeOf<Grain> GrainRange>
    void PrintingModule::save_VTU(
                  NodeRange&& nodes,
                  PoreRange&& pores,
                  GrainRange&& grains,
                  const bool add_grain_info)
    {         //TODO: Add option add_grain_info, now its always true

        OutStream& file = net_out;

        vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();
        std::map<const Node*, vtkIdType> nodeIdMap;

        // --- Node Data ---
        vtkSmartPointer<vtkDoubleArray> pressureArray       = nullptr;
        vtkSmartPointer<vtkDoubleArray> concentrationArray  = nullptr;
        vtkSmartPointer<vtkIntArray>    nodeNameArray       = nullptr;

        if (config.do_save_data_in_vtk) {
            pressureArray = vtkSmartPointer<vtkDoubleArray>::New();
            pressureArray->SetName("pressure");

            concentrationArray = vtkSmartPointer<vtkDoubleArray>::New();
            concentrationArray->SetName("concentrations");
            concentrationArray->SetNumberOfComponents(enum_size_v<SOLUBLES>);

            nodeNameArray = vtkSmartPointer<vtkIntArray>::New();
            nodeNameArray->SetName("node_name");
        }

        // Adding Nodes
        for (const auto& node : nodes) {
            auto pos = node.get_pos();
            auto x = double(pos.x), y = double(pos.y), z = double(pos.z);
            vtkIdType id = vtk_points->InsertNextPoint(x, y, z);
            nodeIdMap.emplace(&node, id);

            if (config.do_save_data_in_vtk) {
                auto c_tmp = node.get_state().c.to_double_vector();
                concentrationArray->InsertNextTuple(c_tmp.data());
                pressureArray->InsertNextValue(static_cast<double>(node.get_u()));
                nodeNameArray->InsertNextValue(node.config.name);
            }
        }

        // --- Pore Data ---
        vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
        vtkSmartPointer<vtkDoubleArray> diameterArray   = nullptr;
        vtkSmartPointer<vtkDoubleArray> lengthArray     = nullptr;
        vtkSmartPointer<vtkDoubleArray> flowArray       = nullptr;
        vtkSmartPointer<vtkIntArray>    poreNameArray   = nullptr;

        if (config.do_save_data_in_vtk) {
            diameterArray = vtkSmartPointer<vtkDoubleArray>::New();
            diameterArray->SetName("diameter");
            lengthArray = vtkSmartPointer<vtkDoubleArray>::New();
            lengthArray->SetName("length");
            flowArray = vtkSmartPointer<vtkDoubleArray>::New();
            flowArray->SetName("flow");
            poreNameArray = vtkSmartPointer<vtkIntArray>::New();
            poreNameArray->SetName("pore_name");
        }

        for (const auto& pore : pores) {
            if (pore.get_nodes()[0]->get_pos() - pore.get_nodes()[1]->get_pos() < net_topo_config.N_x * 0.5_L)
            if (pore.get_nodes().size() == 2 && pore.get_nodes()[0] && pore.get_nodes()[1]) {
                auto node1 = nodeIdMap.find(pore.get_nodes()[0]);
                auto node2 = nodeIdMap.find(pore.get_nodes()[1]);

                if (node1 != nodeIdMap.end() && node2 != nodeIdMap.end()) {
                    vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
                    line->GetPointIds()->SetId(0, node1->second);
                    line->GetPointIds()->SetId(1, node2->second);
                    lines->InsertNextCell(line);

                    if (config.do_save_data_in_vtk) {
                        diameterArray->InsertNextValue(static_cast<double>(pore.get_d()));
                        lengthArray->InsertNextValue(static_cast<double>(pore.get_l()));
                        flowArray->InsertNextValue(static_cast<double>(pore.get_q()));
                        poreNameArray->InsertNextValue(pore.config.name);
                    }
                }
            }
        }


        // --- Grain data (grain as polygon) ---
        vtkSmartPointer<vtkCellArray> grainCells = vtkSmartPointer<vtkCellArray>::New();
        vtkSmartPointer<vtkDoubleArray> grainVolumeArray = nullptr;
        vtkSmartPointer<vtkDoubleArray> grainMineralArray = nullptr;
        vtkSmartPointer<vtkIntArray> grainNameArray = nullptr;
        if (add_grain_info){
            if (config.do_save_data_in_vtk) {
                grainVolumeArray = vtkSmartPointer<vtkDoubleArray>::New();
                grainVolumeArray->SetName("grain_total_volume");

                grainMineralArray = vtkSmartPointer<vtkDoubleArray>::New();
                grainMineralArray->SetName("mineral_volumes");
                grainMineralArray->SetNumberOfComponents(enum_size_v<SOLIDS>);

                grainNameArray = vtkSmartPointer<vtkIntArray>::New();
                grainNameArray->SetName("grain_name");
            }

            for (const auto& grain : grains) {
                if (grain.is_crossing_pbc())      continue;
                if (grain.get_nodes().size() < 3) continue;

                // Check if all nodes exist in the map
                bool allNodesExist = true;
                for (const auto& node : grain.get_nodes()) {
                    if (nodeIdMap.find(node) == nodeIdMap.end()) {
                        allNodesExist = false;
                        break;
                    }
                }

                if (!allNodesExist) continue;

                // Create the proper cell shell (depends on the number of nodes in the grain)
                vtkSmartPointer<vtkCell> poly;
                switch (grain.get_nodes().size()) {
                    case 3: poly = vtkSmartPointer<vtkTriangle>::New(); break;
                    case 4: poly = vtkSmartPointer<vtkQuad>::New();     break;
                    case 5: poly = vtkSmartPointer<vtkPolygon>::New();  break;
                    default:
                        poly = vtkSmartPointer<vtkPolygon>::New();
                        poly->GetPointIds()->SetNumberOfIds(int(grain.get_nodes().size()));
                        break;
                }

                for (size_t i = 0; i < grain.get_nodes().size(); ++i) {
                    poly->GetPointIds()->SetId(int(i), nodeIdMap.at(grain.get_nodes()[i]));
                }
                grainCells->InsertNextCell(poly);

                if (config.do_save_data_in_vtk) {
                    auto v_tmp = grain.get_state().v.to_double_vector();
                    grainMineralArray->InsertNextTuple(v_tmp.data());
                    grainNameArray->InsertNextValue(grain.config.name);
                    grainVolumeArray->InsertNextValue(static_cast<double>(grain.get_tot_v()));
                }
            }
        }

        // --- Creating Grid ---
        vtkSmartPointer<vtkUnstructuredGrid> grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        grid->SetPoints(vtk_points);

        // Add cells
        if (lines->GetNumberOfCells() > 0) {
            grid->SetCells(VTK_LINE, lines);
        }

        if(add_grain_info) if(grainCells->GetNumberOfCells() > 0) {
            grid->SetCells(VTK_POLYGON, grainCells);
        }

        if (config.do_save_data_in_vtk) {
            if (pressureArray && pressureArray->GetNumberOfTuples() > 0) {
                grid->GetPointData()->AddArray(pressureArray);
            }
            if (concentrationArray && concentrationArray->GetNumberOfTuples() > 0) {
                grid->GetPointData()->AddArray(concentrationArray);
            }
            if (nodeNameArray && nodeNameArray->GetNumberOfTuples() > 0) {
                grid->GetPointData()->AddArray(nodeNameArray);
            }

            if (diameterArray && diameterArray->GetNumberOfTuples() > 0) {
                grid->GetCellData()->AddArray(diameterArray);
            }
            if (lengthArray && lengthArray->GetNumberOfTuples() > 0) {
                grid->GetCellData()->AddArray(lengthArray);
            }
            if (flowArray && flowArray->GetNumberOfTuples() > 0) {
                grid->GetCellData()->AddArray(flowArray);
            }
            if (poreNameArray && poreNameArray->GetNumberOfTuples() > 0) {
                grid->GetCellData()->AddArray(poreNameArray);
            }
            if (add_grain_info){
                if (grainVolumeArray && grainVolumeArray->GetNumberOfTuples() > 0) {
                    grid->GetCellData()->AddArray(grainVolumeArray);
                }
                if (grainMineralArray && grainMineralArray->GetNumberOfTuples() > 0) {
                    grid->GetCellData()->AddArray(grainMineralArray);
                }
                if (grainNameArray && grainNameArray->GetNumberOfTuples() > 0) {
                    grid->GetCellData()->AddArray(grainNameArray);
                }
            }
        }

        vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
        writer->SetFileName((file.get_name() + "_" + std::to_string(file.get_page_count()) + ".vtu").c_str());
        writer->SetInputData(grid);
        writer->SetDataModeToBinary();
        writer->Write();
        file.add_page();
    }

} // namespace karst

#endif // KARST_4_0_VTK_WRITE_H


