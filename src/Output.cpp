//
// Created by Stefanie on 13.08.2018.
//

#include "Output.h"

#include <iostream>
#include <fstream>

void Output::initialCellCentersXY(std::vector<Cell> cells,  int InSimulationCells) {
    std::ofstream outputFile;
    outputFile.open ("Initial_Cell_Centers.txt");

    outputFile << "X\tY" << std::endl;

    for (int cell = 0; cell < InSimulationCells; ++cell) {
        outputFile << cells[cell].getX() << "\t" << cells[cell].getY() << std::endl;
    }

    outputFile.close();
}

void Output::initialCellBordersXY(std::vector<Cell> cells, int InSimulationCells) {
    std::ofstream outputFile;
    outputFile.open("Initial_Cell_Borders.txt");

    outputFile << "X\tY" << std::endl;

    for (int cell = 0; cell < InSimulationCells; ++cell) {
        for (int borderPoint = 0; borderPoint < cells[cell].getBorderPointsX().size(); ++borderPoint) {
            outputFile << cells[cell].getBorderPointsX()[borderPoint] << "\t" << cells[cell].getBorderPointsY()[borderPoint] << std::endl;
        }
    }

    outputFile.close();
}