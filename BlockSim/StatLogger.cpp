//
//  StatLogger.cpp
//  BlockSim
//
//  Created by Rastislav Budinsky 11/13/22
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//

#include "StatLogger.hpp"
#include "block.hpp"

#include <sys/stat.h>
#include <fstream>
#include <algorithm>

StatLogger::StatLogger(std::string feeContracts_, std::string feeSimulator_) : feeContracts(feeContracts_), feeSimulator(feeSimulator_) {

    // Dangerous! Clears directories from previous runs
    system(("rm -r ./" + feeContracts).c_str());
    system(("rm -r ./" + feeSimulator).c_str());
    
    mkdir(("./" + feeContracts).c_str(), 0775);
    mkdir(("./" + feeSimulator).c_str(), 0775);
}

// In case of more classes to be logged it is recommended to create common interface
void StatLogger::log(unsigned int gameNum, Blockchain &blockchain) {
    auto winningChain = blockchain.winningHead().getChain();
    std::reverse(winningChain.begin(), winningChain.end());
    std::ofstream output(feeContracts + "/game-" + std::to_string(gameNum) + ".txt");
    std::ofstream output2(feeSimulator + "/game-blocks-" + std::to_string(gameNum) + ".txt");

    // Header is the same for every block
    winningChain.back()->feeContract->printHeader(output);
    for (auto block : winningChain) {
        block->feeContract->printValue(output);
        output2 << block->value << std::endl;
    }
    output.close();
    output2.close();

    output = std::ofstream(feeSimulator + "/game-" + std::to_string(gameNum) + ".txt");

    blockchain.getFeeSimulator().print(output);

    output.close();
}