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

Value FeeContract::toContracts = 0;

FeeContract::Contract::Contract(const Value initial_value, const HeightType length_red_, int max_ = 10000) : value_con(initial_value), length_red(length_red_), avg(initial_value / length_red_), max(max_ + 100) {}

std::pair<Value, std::unique_ptr<FeeContract::Contract>> FeeContract::Contract::claimAndCollect(const Value collected) {
    Value claim(nextClaim());
    
    std::pair<Value, std::unique_ptr<FeeContract::Contract>> pair(claim, std::make_unique<Contract>(value_con - claim + collected, length_red, max));
    pair.second->avg = avg * (length_red - 1) / length_red + claim;
    
    return pair;
}

Value FeeContract::Contract::nextClaim() {
    return std::min(Value(value_con / length_red), max / 100 * avg);
}

Value FeeContract::Contract::totalValue() {
    return value_con;
}

HeightType FeeContract::Contract::getLength() {
    return length_red;
}

FeeContract::FeeContract() {};

FeeContract::FeeContract(const char filepath[]) {
    using namespace nlohmann;
    
    std::vector<std::pair<ContractCount, HeightType>> initial_split;
    std::fstream f(filepath == nullptr ? FEE_CONTRACT_FILE : filepath);
    json data = json::parse(f);
    f.close();

    m_assert(data.contains("contracts") &&
            data.contains("toContracts"),
            "Missing json parameters");

    toContracts = data.at("toContracts").get<Value>();
    m_assert(toContracts <= 100 &&
            toContracts >= 0,
            "Variable toContracts expects value in range [0,100]");
    data = data.at("contracts");

    ContractCount total(0);
    for (auto& it : data) {
        m_assert(it.contains("percentage") &&
                it.contains("length") &&
                it.contains("value"),
                "Missing parameters for contract");

        ContractCount percentage = it.at("percentage").get<ContractCount>();
        HeightType length = it.at("length").get<HeightType>();
        Value value = it.at("value").get<ValueType>();
        contracts.push_back(std::make_pair(percentage, std::make_unique<Contract>(value, length)));
        total += percentage;
    }

    m_assert(total == CountType(100),
            "Expecting total sum of contracts' percentages to equal 100%");
}

std::pair<Value, std::unique_ptr<FeeContract>> FeeContract::claimAndCollect(Value collected) {
    Value claim(0);
    auto next_contract = std::make_unique<FeeContract>();
    for (const auto& pair : contracts) {
        Value value(collected * pair.first / 100);
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

void FeeContract::printHeader(std::ofstream &output) {
    output << "next ";
    for (const auto& pair : contracts) {
        output << pair.first << "(" << pair.second->getLength() << ") ";
    }
    output << "total" << std::endl;
}

void FeeContract::printValue(std::ofstream &output) {
    Value total(0);

    output << nextClaim() << " ";
    for (const auto& pair : contracts) {
        total += pair.second->totalValue();
        output << pair.second->totalValue() << " ";
    }

    output << total << std::endl;
}

std::pair<Value, Value> FeeContract::getFeeRate(Value fees) {
    return std::make_pair(fees * (100 - toContracts) / 100, fees * toContracts / 100);
}

Value FeeContract::backToContract(Value fees) {
    m_assert(fees != 0 ||
            toContracts != 100,
            "Unable to calculate backToContract");
    return (toContracts / (100.0 - toContracts)) * fees;
}
