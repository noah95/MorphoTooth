//
// Created by Stefanie on 13.08.2018.
//

#include <cmath>
#include "Model.h"
#include "Geometrics.h"
#include "Parameters.h"

void Model::diffusion(std::vector<Cell> &cells, Parameters &params) {

    //Calculate for each cell its perimeter and area
    Geometrics::calculatePerimeterAndArea(cells, params.getCellsInSimulation());

    for (int cell = 0; cell < params.getCellsInSimulation(); ++cell) {
        //Total diffusion area = perimeter + 2 * area (bottom and top area)
        double perimeter = cells[cell].getPerimeter();
        double cellArea = cells[cell].getCellArea();

        double pDiffusionArea = perimeter + (2 * cellArea);
        double eDiffusionArea = perimeter + cellArea;

        // Set cell area relative to total diffusion area
        double pCellArea = cellArea / pDiffusionArea;
        double eCellArea = cellArea / eDiffusionArea;


        //Diffusion in all layers in all directions
        for (int protein = 0; protein < 4; ++protein) {
            for (int layer = 0; layer < cells[cell].getMesenchymeThickness(); ++layer) {
                if (layer != 0) { // if we are not within the epithelial layer
                    upDiffusion(cells, cell, layer, protein, pCellArea);
                    if (layer < (cells[cell].getMesenchymeThickness() - 1)) { // if its not the lowest layer
                        downDiffusion(cells, cell, layer, protein, pCellArea);
                    } else { // if its the lowest layer -> vertical sink
                        sink(cells, cell, layer, protein, pCellArea);
                    }
                    horizontalDiffusion(cells, cell, layer, protein, pDiffusionArea);
                } else if (layer == 0) { // if we are in the epithelium, do only horizontal Diffusion

                    horizontalDiffusion(cells, cell, layer, protein, eDiffusionArea);
                }
            }
        }
    }

    // Calculate the final protein concentrations (including diffusion coefficients and delta)
    for (int cell = 0; cell < params.getCellsInSimulation(); ++cell) {
        for (int protein = 0; protein < 4; ++protein) {
            for (int layer = 0; layer < cells[cell].getMesenchymeThickness(); ++layer) {
                double newConcentration = params.getDelta() * params.getDiffusionRates()[protein] *
                                          cells[cell].getTempProteinConcentrations()[protein][layer];
                cells[cell].addProteinConcentration(protein, layer, newConcentration);
            }
        }
        //Reset the temporary protein concentration matrix
        cells[cell].resetTempProteinConcentrations();
    }
}

void Model::upDiffusion(std::vector<Cell> &cells, int cell, int layer, int protein, double pCellArea) {
    double oldConcentration = cells[cell].getProteinConcentrations()[protein][layer];
    double neighbourConcentration = cells[cell].getProteinConcentrations()[protein][layer - 1];
    double newConcentration = (pCellArea * (neighbourConcentration - oldConcentration));

    cells[cell].addTempConcentration(protein, layer, newConcentration);
}

void Model::downDiffusion(std::vector<Cell> &cells, int cell, int layer, int protein, double pCellArea) {
    double oldConcentration = cells[cell].getProteinConcentrations()[protein][layer];
    double neighbourConcentration = cells[cell].getProteinConcentrations()[protein][layer + 1];
    double newConcentration = (pCellArea * (neighbourConcentration - oldConcentration));

    cells[cell].addTempConcentration(protein, layer, newConcentration);
}

void Model::sink(std::vector<Cell> &cells, int cell, int layer, int protein, double contactArea) {
    double oldConcentration = cells[cell].getProteinConcentrations()[protein][layer];
    double newConcentration = (contactArea * (-oldConcentration * 0.4));

    cells[cell].addTempConcentration(protein, layer, newConcentration);
}

void Model::horizontalDiffusion(std::vector<Cell> &cells, int cell, int layer, int protein, double diffusionArea) {
    double oldConcentration = cells[cell].getProteinConcentrations()[protein][layer];
    double newConcentration = 0;
    for (int neighbour = 0; neighbour < cells[cell].getNeighbours().size(); ++neighbour) {
        int neighbourID = cells[cell].getNeighbours()[neighbour];
        if (cells[neighbourID].isInSimulation()) {
            double neighbourConcentration = cells[neighbourID].getProteinConcentrations()[protein][layer];
            double pPerimeterPart = (cells[cell].getPerimeterParts()[neighbour] / diffusionArea);
            newConcentration += (pPerimeterPart * (neighbourConcentration - oldConcentration));
            cells[cell].addTempConcentration(protein, layer, newConcentration);
        } else {          // if the neighbour is not within simulation, there is a sink
            sink(cells, cell, layer, protein, diffusionArea);
        }
    }
}

void Model::reaction(std::vector<Cell> &cells, Parameters &params) {
    for (int cell = 0; cell < params.getCellsInSimulation(); ++cell) {
        EKDifferentiation(cells, params, cell);
        ActReactionAndDegradation(cells, params, cell);
        InhProduction(cells, params, cell);
        Sec1Production(cells, params, cell);
        Sec2Production(cells, params, cell);
    }

    //Error handling (test if new concentrations are too high)?

    //Update the final protein concentrations (including delta)
    for (int cell = 0; cell < params.getCellsInSimulation(); ++cell) {
        for (int protein = 0; protein < 4; ++protein) {
            for (int layer = 0; layer < cells[cell].getMesenchymeThickness(); ++layer) {
                double newConcentration = params.getDelta() * cells[cell].getTempProteinConcentrations()[protein][layer];
                cells[cell].addProteinConcentration(protein, layer, newConcentration);
                //Remove negative concentration values
                if (cells[cell].getProteinConcentrations()[protein][layer] < 0){
                    cells[cell].setProteinConcentration(protein, layer, 0);
                }
            }
        }
        //Reset the temporary protein concentration matrix
        cells[cell].resetTempProteinConcentrations();
    }
}

void Model::EKDifferentiation(std::vector<Cell> &cells, Parameters &params, int cell) {
    //if the Act concentration in the epithelial layer is high enough
    //and if it is in the centre, then it becomes/is a knot cell
    if (cells[cell].getProteinConcentrations()[0][0] > 1) {
        if (cells[cell].isInCentre()) {
            cells[cell].setKnotCell(true);
        }
    }
}

void Model::ActReactionAndDegradation(std::vector<Cell> &cells, Parameters &params, int cell) {
    double epithelialActConcentration = cells[cell].getProteinConcentrations()[0][0];
    double epithelialInhConcentration = cells[cell].getProteinConcentrations()[1][0];
    double epithelialSec2Concentration = cells[cell].getProteinConcentrations()[3][0];

    double positiveTerm = params.getAct() * epithelialActConcentration - epithelialSec2Concentration;
    if (positiveTerm < 0) {
        positiveTerm = 0;
    }
    double negativeTerm = 1 + params.getInh() * epithelialInhConcentration;
    double degradation = params.getMu() * epithelialActConcentration;

    //concentration difference: reaction - degradation
    double newConcentration = positiveTerm / negativeTerm - degradation;
    cells[cell].addTempConcentration(0, 0, newConcentration);
}

void Model::InhProduction(std::vector<Cell> &cells, Parameters &params, int cell) {
    //Inh is produced if diff state is higher than threshold or if the cell is an EK cell
    double diffState = cells[cell].getDiffState();
    double epithelialInhConcentration = cells[cell].getProteinConcentrations()[1][0];
    double epithelialActConcentration = cells[cell].getProteinConcentrations()[0][0];
    bool isKnotCell = cells[cell].isKnotCell();
    double newConcentration;

    if (diffState > params.getInT()) {
        newConcentration = epithelialActConcentration * diffState - params.getMu() * epithelialInhConcentration;
    }
    else if (isKnotCell) {
        newConcentration = epithelialActConcentration - params.getMu() * epithelialInhConcentration;
    }
    cells[cell].addTempConcentration(1, 0, newConcentration);
}

void Model::Sec1Production(std::vector<Cell> &cells, Parameters &params, int cell) {
    double diffState = cells[cell].getDiffState();
    double epithelialSec1Concentration = cells[cell].getProteinConcentrations()[2][0];
    bool isKnotCell = cells[cell].isKnotCell();
    double newConcentration;

    if (diffState > params.getSet()){
        newConcentration = params.getSec() * diffState - params.getMu() * epithelialSec1Concentration;
    }
    else if (isKnotCell){
        newConcentration = params.getSec() - params.getMu() * epithelialSec1Concentration;
    }

    if (newConcentration < 0){
        newConcentration = 0;
    }
    cells[cell].addTempConcentration(2, 0, newConcentration);
}

void Model::Sec2Production(std::vector<Cell> &cells, Parameters &params, int cell) {
    double epithelialActConcentration = cells[cell].getProteinConcentrations()[0][0];
    double epithelialSec1Concentration = cells[cell].getProteinConcentrations()[2][0];
    double epithelialSec2Concentration = cells[cell].getProteinConcentrations()[3][0];

    double newConcentration = params.getAct() * epithelialActConcentration - params.getMu() * epithelialSec2Concentration - params.getSec2Inhibition() * epithelialSec1Concentration;
    if (newConcentration < 0){
        newConcentration = 0;
    }

    cells[cell].addTempConcentration(3, 0, newConcentration);
}