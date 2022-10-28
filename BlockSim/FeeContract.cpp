//
//  FeeContract.cpp
//  BlockSim
//
//  Created by Rastislav Budinsky
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//
//  Please see description in file fee_contract.hpp

#include <utility>
#include <algorithm>
#include <fstream>

#include "FeeContract.hpp"
#include "json.hpp"
#include "logging.h"

FeeContract::Contract::Contract(const Value initial_value, const HeightType length_red_, int max_ = 10000) : value_con(initial_value), length_red(length_red_), avg(initial_value / length_red_), max(max_ + 100) {}

std::pair<Value, std::unique_ptr<FeeContract::Contract>> FeeContract::Contract::claimAndCollect(const Value collected) {
    Value claim(nextClaim());
    avg = avg * (length_red - 1) / length_red + claim;
    
    value_con += collected;

    std::pair<Value, std::unique_ptr<FeeContract::Contract>> pair(claim, std::make_unique<Contract>(value_con, length_red));
    return pair;
}

Value FeeContract::Contract::nextClaim() {
    return std::min(Value(value_con / length_red), max / 100 * avg);
}

FeeContract::FeeContract() {};

FeeContract::FeeContract(Value initial_total_value) {
    using namespace nlohmann;
    
    std::vector<std::pair<ContractCount, HeightType>> initial_split;
    std::fstream f(FEE_CONTRACT_FILE);
    json data = json::parse(f);
    m_assert(data.contains("contracts"), "Missing json parameters");
    data = data.at("contracts");

    ContractCount total(0);
    for (auto& it : data) {
        m_assert(it.contains("percentage") &&
                it.contains("length"),
                "Missing parameters for contract");

        ContractCount percentage = it.at("percentage").get<ContractCount>();
        HeightType length = it.at("length").get<HeightType>();
        Value initial_value = (initial_total_value / 100) / percentage; //  switching order for better manipulation in satoshis: initial / (first / 100) is the same
        contracts.push_back(std::make_pair(percentage, std::make_unique<Contract>(initial_value, length)));
        total += percentage;
    }

    m_assert(total == CountType(100),
            "Expecting total sum of contracts' percentages to equal 100%");
}

std::pair<Value, std::unique_ptr<FeeContract>> FeeContract::claimAndCollect(Value collected) {
    Value claim(0);
    auto next_contract = std::make_unique<FeeContract>();
    for (const auto& pair : contracts) {
        Value value((collected / 100) / pair.first);
        auto reply = pair.second->claimAndCollect(value);
        claim += reply.first;
        next_contract->contracts.push_back(std::make_pair(pair.first, std::move(reply.second)));
    }
    auto reply = std::make_pair(claim, std::move(next_contract));
    return reply;
}

Value FeeContract::nextClaim() {
    Value claim(0);
    for (const auto& pair : contracts) {
        claim += pair.second->nextClaim();
    }
    return claim;
}
