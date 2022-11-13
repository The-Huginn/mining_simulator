//
//  StatLogger.hpp
//  BlockSim
//
//  Created by Rastislav Budinsky 11/13/22
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//

#ifndef STAT_LOGGER
#define STAT_LOGGER

#include <string>

#include "blockchain.hpp"

class StatLogger {
    private:
        std::string feeContracts;
        std::string feeSimulator;

    public:
        StatLogger(std::string feeContracts_, std::string feeSimulator_);

        void log(unsigned int gameNum, Blockchain &blockchain);
};

#endif  /* STAT_LOGGER */