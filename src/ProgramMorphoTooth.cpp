//
// Created by Stefanie on 26.11.2018.
//

#include <iostream>
#include <cmath>
#include "ProgramMorphoTooth.h"
#include "Input.h"
#include "Initial.h"
#include "Output.h"
#include "Geometrics.h"
#include "Model.h"
#include "Parameters.h"
#include "consts.h"
#include "Print.h"

void ProgramMorphoTooth::runProgram(Parameters &params) {

    //Print information that program is started with value of parameter to change
    Print::printStartOfSimulation(params);

    //Vector containing all cells
    Cell cells[maxNrOfCells];

    //Construct the initial grid of cells
    Initial::makeInitialGrid(params, cells);

    //The big loop
    //In each iteration mechanisms as diffusion, reaction, growth, and cell division take place
    for (int step = 0; step < params.maxNrOfIterations + 1; ++step) {
        Model::iterationStep(cells, params);

        //Abort the loop if there is an error
        if (Model::errorTesting(cells, params)) {
            std::cout << "The simulation was stopped because there was an error at iteration: " << step << std::endl;
            break;
        }

        //Abort the loop if one of the end-determing variables reaches its maximum (number of cell divisions or iteration)
        if (Model::endOfSimulation(params, step)){
            break;
        }
        
        //For debugging
        if (params.currentIteration == 1156) {
            int a = 0;
        }

        //Print every 1000 iteration the count
        if (step % params.printInterval == 0) {
            std::cout << step << std::endl;
            std::cout << "ncels: " << params.nrCellsInSimulation << std::endl;
            std::cout.flush();
        }


        /*//All 1000 iterations do an output
        if (step % params.outputInterval == 0) {
            Output::ROutput(cells, params);
            Output::geomorphLinkOutput(cells, params);
            Output::XYZOutputSimple(cells, params);
        }*/
        params.currentIteration += 1;
    }

    Output::ROutput(cells, params);
    Output::geomorphLinkOutput(cells, params);
    //Output::XYZOutputSimple(cells, params);

    Print::printEndOfSimulation();

}

void ProgramMorphoTooth::runProgramWithDifferentConditions(Parameters &paramsInitial, bool parameterToChangeIsInt) {

    //Calculate how many conditions there are
    double conditionsDouble;
    int conditions;

    if (paramsInitial.totalPlusMinusScope == 0) {
        conditions = 1;
    } else {
        conditionsDouble =
                (2 * paramsInitial.totalPlusMinusScope / paramsInitial.percentageSteps) + 1; // in both directions -> *2
        conditions = static_cast<int>(std::floor(conditionsDouble + 0.5));
    }

    paramsInitial.nrOfConditions = conditions;

    /*if (parameterToChangeIsInt) {
        //Calculate starting condition
        double startingValueDouble = paramsInitial.valueOfParameterToChange -
                                     (paramsInitial.valueOfParameterToChange * paramsInitial.totalPlusMinusScope);
        int startingValue = static_cast<int>(std::floor(startingValueDouble + 0.5));

        // Calculate the change per step (per condition)
        double changePerConditionDouble = paramsInitial.valueOfParameterToChange * paramsInitial.percentageSteps;
        int changePerCondition = static_cast<int>(std::floor(changePerConditionDouble + 0.5));

        //Set starting conditions
        Input::changeInputFileTemp(paramsInitial, startingValue);

        //Loop that starts the program with different conditions (input parameters)
        for (int condition = 0; condition < paramsInitial.nrOfConditions; ++condition) {
            //Re-read the InputFile
            std::string InputFileName = "InputFileTemp.txt";
            Parameters params = Input::setParameters(InputFileName);

            // run the program with current conditions
            ProgramMorphoTooth::runProgram(params);

            // change the conditions
            double newValueDouble = ((condition + 1) * changePerCondition) + startingValue;
            int newValue = static_cast<int>(std::floor(newValueDouble + 0.5));
            Input::changeInputFileTemp(params, newValue);
        }

    } else {*/
    //Calculate starting condition
    double startingValueDouble = paramsInitial.valueOfParameterToChange -
                                 (paramsInitial.valueOfParameterToChange * paramsInitial.totalPlusMinusScope);

    // Calculate the change per step (per condition)
    double changePerConditionDouble = paramsInitial.valueOfParameterToChange * paramsInitial.percentageSteps;

    //Set starting conditions
    Input::changeInputFileTemp(paramsInitial, startingValueDouble);

    //Loop that starts the program with different conditions (input parameters)
    for (int condition = 0; condition < paramsInitial.nrOfConditions; ++condition) {
        //Re-read the InputFile
        std::string InputFileName = "InputFileTemp.txt";
        Parameters params = Input::setParameters(InputFileName);

        // run the program with current conditions
        ProgramMorphoTooth::runProgram(params);

        // change the conditions
        double newValue = ((condition + 1) * changePerConditionDouble) + startingValueDouble;
        Input::changeInputFileTemp(params, newValue);

    }
}




