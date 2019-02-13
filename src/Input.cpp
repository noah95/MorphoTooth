//
// Created by Stefanie on 07.09.2018.
//

#include "Input.h"
#include "Utility.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

Parameters Input::setParametersInitial(std::string nameInputFile) {
    // Set up vector and struct
    Parameters params;
    double parameter[60] = {0};

    // Read in values from inputFile
    std::string line;
    int counter = 0;

    std::ifstream InputFile(nameInputFile);
    std::string delimiter = ":";

    if (InputFile.is_open()) {
        while (getline(InputFile, line)) {
            Utility::trimString(line);
            if (line.length() > 0) {
                std::string token = line.substr(line.find(delimiter) + 1, line.length());
                parameter[counter] = std::stod(token);
                counter += 1;
            } else {
                std::cout << "There was an empty line in InputFile.txt at line " << counter << std::endl;
                std::cout << "Therefore the reading in was stopped" << std::endl;
                break;
            }
        }
        InputFile.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }

    // Initialize parameters with values from the file
    //Model parameters
    params.initialRadius = static_cast<int>(parameter[0]);
    params.distanceCellDivision = parameter[1];
    params.EKThreshold = parameter[2];
    params.repDistance = parameter[3];
    params.powerOfRep = static_cast<int>(parameter[4]);
    params.sinkAmount = parameter[5];
    params.ActDiffusion = parameter[6];
    params.InhDiffusion = parameter[7];
    params.SecDiffusion = parameter[8];
    params.delta = parameter[9];
    params.act = parameter[10];
    params.inh = parameter[11];
    params.mu = parameter[12];
    params.inT = parameter[13];
    params.set = parameter[14];
    params.sec = parameter[15];
    params.lbi = parameter[16];
    params.bbi = parameter[17];
    params.swi = parameter[18];
    params.dff = parameter[19];
    params.egr = parameter[20];
    params.mgr = parameter[21];
    params.dgr = parameter[22];
    params.boy = parameter[23];
    params.rep = parameter[24];
    params.adh = parameter[25];
    params.ntr = parameter[26];
    params.bwi = parameter[27];
    params.abi = parameter[28];
    params.pbi = parameter[29];
    params.bgr = parameter[30];

    //Implementation parameters
    params.newInhAndSecProduction = static_cast<int>(parameter[31]);        
    params.maxNrOfIterations = static_cast<int>(parameter[32]);
    params.minNrOfCells = static_cast<int>(parameter[33]);
    params.outputInterval = static_cast<int>(parameter[34]);
    params.printInterval = static_cast<int>(parameter[35]);
    params.outputPrecision = static_cast<int>(parameter[36]);
    params.repetitions = static_cast<int>(parameter[37]);
    params.parameterWithNoise = static_cast<int>(parameter[38]);
    params.sdPercentage = parameter[39];
    params.noiseDuration = static_cast<int>(parameter[40]);

    // Manage the parameters to change
    params.nrOfParametersToChange = static_cast<int>(parameter[41]);
    int indexOfParameter = 42;
    for (int parameterToChange = 0; parameterToChange < params.nrOfParametersToChange; ++parameterToChange) {
        double dParameterToChange = parameter[indexOfParameter];
        double totalPlusMinusScope = parameter[indexOfParameter + 1];
        double percentageSteps = parameter[indexOfParameter + 2];
        double valueOfParameterToChange = parameter[static_cast<int>(dParameterToChange)];
        indexOfParameter += 3;

        std::vector<double> currentValuesOfParameterToChange = {dParameterToChange, totalPlusMinusScope,
                                                                percentageSteps, valueOfParameterToChange};
        params.parameterToChangeValues.push_back(currentValuesOfParameterToChange);
    }

    params.valueOfParameterAffectedByNoise = parameter[params.parameterWithNoise];

    return params;
}

void Input::createInputFileTemp(int parameter, Parameters params, std::string fileName) {
    std::ofstream InputFileTemp(fileName);
    std::string line;

    if (InputFileTemp.is_open()) {
        InputFileTemp << params.initialRadius << "\n";
        InputFileTemp << params.distanceCellDivision << "\n";
        InputFileTemp << params.EKThreshold << "\n";
        InputFileTemp << params.repDistance << "\n";
        InputFileTemp << params.powerOfRep << "\n";
        InputFileTemp << params.sinkAmount << "\n";
        InputFileTemp << params.ActDiffusion << "\n";
        InputFileTemp << params.InhDiffusion << "\n";
        InputFileTemp << params.SecDiffusion << "\n";
        InputFileTemp << params.delta << "\n";
        InputFileTemp << params.act << "\n";
        InputFileTemp << params.inh << "\n";
        InputFileTemp << params.mu << "\n";
        InputFileTemp << params.inT << "\n";
        InputFileTemp << params.set << "\n";
        InputFileTemp << params.sec << "\n";
        InputFileTemp << params.lbi << "\n";
        InputFileTemp << params.bbi << "\n";
        InputFileTemp << params.swi << "\n";
        InputFileTemp << params.dff << "\n";
        InputFileTemp << params.egr << "\n";
        InputFileTemp << params.mgr << "\n";
        InputFileTemp << params.dgr << "\n";
        InputFileTemp << params.boy << "\n";
        InputFileTemp << params.rep << "\n";
        InputFileTemp << params.adh << "\n";
        InputFileTemp << params.ntr << "\n";
        InputFileTemp << params.bwi << "\n";
        InputFileTemp << params.abi << "\n";
        InputFileTemp << params.pbi << "\n";
        InputFileTemp << params.bgr << "\n";
        InputFileTemp << params.newInhAndSecProduction << "\n";
        InputFileTemp << params.maxNrOfIterations << "\n";
        InputFileTemp << params.minNrOfCells << "\n";
        InputFileTemp << params.outputInterval << "\n";
        InputFileTemp << params.printInterval << "\n";
        InputFileTemp << params.outputPrecision << "\n";
        InputFileTemp << params.repetitions << "\n";
        InputFileTemp << params.parameterWithNoise << "\n";
        InputFileTemp << params.sdPercentage << "\n";
        InputFileTemp << params.noiseDuration << "\n";
        InputFileTemp << params.parameterToChangeValues[parameter][0] << "\n";
        InputFileTemp << params.parameterToChangeValues[parameter][1] << "\n";
        InputFileTemp << params.parameterToChangeValues[parameter][2] << "\n";
        InputFileTemp << params.parameterToChangeValues[parameter][3] << "\n";
        InputFileTemp << params.valueOfParameterAffectedByNoise << "\n";
    }

    InputFileTemp.close();
}

Parameters Input::setParameters(std::string InputFileName) {
    // Set up vector and struct
    Parameters params;
    double parameter[60] = {0};

    // Set initial values independent of inputFile
    params.error = false;
    params.endOfSimulation = false;
    params.currentIteration = 0;
    params.nrOfConditions = 1;
    params.powerOfRep = 8;

    // Read in values from inputFile
    std::string line;
    int counter = 0;

    std::ifstream InputFile(InputFileName);
    if (InputFile.is_open()) {
        while (getline(InputFile, line)) {
            parameter[counter] = std::stod(line);
            counter += 1;
        }
        InputFile.close();
    } else {
        std::cout << "Unable to open file";
    }

    // Initialize parameters with values from the file
    //Model parameters
    params.initialRadius = static_cast<int>(parameter[0]);
    params.distanceCellDivision = parameter[1];
    params.EKThreshold = parameter[2];
    params.repDistance = parameter[3];
    params.powerOfRep = static_cast<int>(parameter[4]);
    params.sinkAmount = parameter[5];
    params.ActDiffusion = parameter[6];
    params.InhDiffusion = parameter[7];
    params.SecDiffusion = parameter[8];
    params.delta = parameter[9];
    params.act = parameter[10];
    params.inh = parameter[11];
    params.mu = parameter[12];
    params.inT = parameter[13];
    params.set = parameter[14];
    params.sec = parameter[15];
    params.lbi = parameter[16];
    params.bbi = parameter[17];
    params.swi = parameter[18];
    params.dff = parameter[19];
    params.egr = parameter[20];
    params.mgr = parameter[21];
    params.dgr = parameter[22];
    params.boy = parameter[23];
    params.rep = parameter[24];
    params.adh = parameter[25];
    params.ntr = parameter[26];
    params.bwi = parameter[27];
    params.abi = parameter[28];
    params.pbi = parameter[29];
    params.bgr = parameter[30];

    //Implementation parameters
    params.newInhAndSecProduction = static_cast<int>(parameter[31]);        
    params.maxNrOfIterations = static_cast<int>(parameter[32]);
    params.minNrOfCells = static_cast<int>(parameter[33]);
    params.outputInterval = static_cast<int>(parameter[34]);
    params.printInterval = static_cast<int>(parameter[35]);
    params.outputPrecision = static_cast<int>(parameter[36]);
    params.repetitions = static_cast<int>(parameter[37]);
    params.parameterWithNoise = static_cast<int>(parameter[38]);
    params.sdPercentage = parameter[39];
    params.noiseDuration = static_cast<int>(parameter[40]);

    // Parameters to change
    params.parameterToChange = static_cast<int>(parameter[41]);
    params.totalPlusMinusScope = parameter[42];
    params.percentageSteps = parameter[43];
    params.valueOfParameterToChange = parameter[params.parameterToChange];

    params.valueOfParameterAffectedByNoise = parameter[45];

    return params;
}

void Input::changeInputFileTemp(Parameters params, double newValue, std::string nameInputFileTemp) {
    // Read in the InputFileTemp
    std::string lineTemp;
    int counter = 0;

    // Create name for output (only temporary)
    std::stringstream outFileS;
    outFileS << "TempOut" << nameInputFileTemp.substr(13, 2) << ".txt";
    std::string outFileString = outFileS.str();


    std::ifstream InputFile(nameInputFileTemp);
    std::ofstream InputFileout(outFileString);

    // Convert the newValue into a string
    std::ostringstream newValueS;
    newValueS << newValue;
    std::string newValueString = newValueS.str();

    // Read in the file and write each line (possibly changed) into the InputFileout
    if (InputFile.is_open() && InputFileout.is_open()) {
        while (getline(InputFile, lineTemp)) {
            if (counter == params.parameterToChange) {
                lineTemp = newValueString;
            }
            counter += 1;
            lineTemp += "\n";
            InputFileout << lineTemp;
        }
        InputFile.close();
        InputFileout.close();
    } else {
        std::cout << "Unable to open file";
    }

    // Convert the std::strings into const chars*
    const char *cInputFileTemp = nameInputFileTemp.c_str();
    const char *cInputFileOut = outFileString.c_str();

    // Delete old InputFileTemp
    remove(cInputFileTemp);

    //Rename the InputFileout to InputFileTemp
    rename(cInputFileOut, cInputFileTemp);

}
