//
//  FeeContract.hpp
//  BlockSim
//
//  Created by Rastislav Budinsky on 10/18/22
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//
//  Please note, this implementation should not be considered correct
//  as it would waste space in real blockchain. This is just for a 
//  specific simulation use case and so it should be considered demostrative only.
//  It is not save nor space-saving implementation. Keep this in mind
//
//  In a real blockchain like Ergo this would be best implemented using smart contracts
//  to redistribute rewards to miners.
//

#ifndef FEE_CONTRACT
#define FEE_CONTRACT

#include <vector>
#include <memory>
#include <fstream>

#include "typeDefs.hpp"

class FeeContract {

    #define FEE_CONTRACT_FILE "feeContracts.json"

    private:
        class Contract {
            private:
                Value value_con;        // value in contract
                HeightType length_red;  // length of redistribution
                Value avg;              // average reward of last ${length_red} rewards
                int max;                // max allowed increase per reward in %
            public:
                Contract(const Value initial_value, const HeightType length_red_, const int max);

                std::pair<Value, std::unique_ptr<Contract>> claimAndCollect(Value collected);

                Value nextClaim();
                Value totalValue();

                HeightType getLength();
        };

        static Value toContracts;

    protected:
        std::vector<std::pair<ContractCount, std::unique_ptr<Contract>>> contracts;

    public:
        FeeContract();

        // We need to distinguish from default constructor
        // should be used with null as parameter
        FeeContract(const char filepath[]);

        std::pair<Value, std::unique_ptr<FeeContract>> claimAndCollect(Value collected);

        Value nextClaim();

        void printHeader(std::ofstream &output);
        void printValue(std::ofstream &output);

        // pair.first is kept as tx fees by the miner
        // pair.second is sent to the contract(s)
        static std::pair<Value, Value> getFeeRate(Value fees);

        static Value backToContract(Value fees);
};

#endif /* FEE_CONTRACT */