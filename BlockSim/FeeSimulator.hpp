//
//  FeeSimulator.hpp
//  BlockSim
//
//  Created by Rastislav Budinsky on 10/28/22
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//

#ifndef FEE_SIMULATOR
#define FEE_SIMULATOR

#include <vector>

#include "typeDefs.hpp"

// Stateless class so we do not need to reet it
class FeeSimulator {

    private:

        enum EpochType {CONSTANT = 0, LINEAR, NORMAL};
        #define PROFITABILITY_FILE "feeSimulation.json"

        // Stateless class so we do not need to reset it
        class FeeSimulatorEpoch {
            public:
                static constexpr int TOTAL = 2; // max total of variables needed
            
            private:
                HeightType start;
                EpochType type;
                Value values[TOTAL];

            public:
                FeeSimulatorEpoch(HeightType start_, EpochType type_, const Value values_[]);

                Value getValue(const HeightType current, const HeightType end);

                inline BlockHeight getStart() {return start;}

                inline bool operator < (const FeeSimulatorEpoch& other) const {
                    return start < other.start;
                }
        };

        std::vector<FeeSimulatorEpoch> timeline;
        HeightType last;

        unsigned int getCurrentEpoch(const BlockHeight current, unsigned int begin, unsigned int end);

    public:
        FeeSimulator();

        Value getValue(HeightType current);

        const BlockHeight numberOfBlocks();
};

#endif /* FEE_SIMULATOR */