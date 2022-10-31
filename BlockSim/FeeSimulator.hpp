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
    #define FEE_SIMULATOR_FILE "feeSimulation.json"

    private:

        enum EpochType {CONSTANT = 0, LINEAR, NORMAL};

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
        std::vector<Value> snapshots;
        HeightType last;
        BlockRate secondsPerBlock;

        unsigned int getCurrentEpoch(const BlockHeight current, unsigned int begin, unsigned int end);

    public:
        FeeSimulator(BlockRate secondsPerBlock_);

        Value getValue(BlockTime from, BlockTime until);

        const BlockHeight numberOfBlocks();

        void reset();
};

#endif /* FEE_SIMULATOR */