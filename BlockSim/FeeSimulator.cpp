//
//  FeeSimulator.cpp
//  BlockSim
//
//  Created by Rastislav Budinsky 10/28/22
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//

#include "FeeSimulator.hpp"

#include "json.hpp"
#include "logging.h"

#include <cassert>
#include <random>
#include <fstream>
#include <iostream>

FeeSimulator::FeeSimulatorEpoch::FeeSimulatorEpoch(HeightType start_, EpochType type_, const Value values_[]) : start(start_), type(type_) {
    for (int i = 0; i < TOTAL; i++) {
        values[i] = values_[i];
    }
}

Value FeeSimulator::FeeSimulatorEpoch::getValue(const HeightType current, const HeightType end) {

    assert(end > start);
    assert(current >= start && current < end);

    Value ret(0);

    if (type == CONSTANT) {
        ret = values[0];
    } else if (type == LINEAR) {
        ret = values[0] + ((values[1] - values[0]) / (end - start)) * (current - start);
    } else if (type == NORMAL) {
        std::random_device rd{};
        std::mt19937 gen{rd()};
        std::normal_distribution<> d{values[0] * 1.0, values[1] * 1.0};
        ret = std::round(d(gen));
    }

    return ret;
}

FeeSimulator::FeeSimulator() {
    using namespace nlohmann;

    Value mean(0), deviation(0);

    std::ifstream f(FEE_SIMULATOR_FILE);
    json data = json::parse(f);

    m_assert(data.contains("length") &&
            data.contains("mean") &&
            data.contains("deviation"),
            "Missing parameters in json");

    last = data.at("length").get<int>();
    mean = data.at("mean").get<Value>();
    deviation = data.at("deviation").get<Value>();

    m_assert(last > 0 &&
            mean > 0 &&
            deviation > 0,
            "Values are invalid");

    data = data.at("timeline");
    for (auto& it : data) {
        m_assert(it.contains("start") &&
                it.contains("epochType") &&
                it.contains("values"),
                "timeline element does not contain required elements in json");

        HeightType start = it.at("start").get<HeightType>();
        EpochType type = static_cast<EpochType>(it.at("epochType").get<int>());
        Value values[FeeSimulatorEpoch::TOTAL];

        json vals = it.at("values");
        for (long unsigned int i = 0; i < vals.size(); i++) {
            values[i] = vals[i];
        }
        
        if (type == NORMAL && vals.empty()) {
            values[0] = mean;
            values[1] = deviation;
        }

        timeline.push_back(FeeSimulatorEpoch(start, type, values));
    }

    std::sort(timeline.begin(), timeline.end());
}

unsigned int FeeSimulator::getCurrentEpoch(const BlockHeight current, unsigned int begin, unsigned int end) {
    
    if (begin >= end) {
        // Over last element
        if (begin == timeline.size())
            return end;

        if (current < timeline[begin].getStart())
            return begin - 1;

        return begin;
    }
    
    unsigned int mid = (begin + end) / 2;

    if (timeline[mid].getStart() <= current)
        return getCurrentEpoch(current, mid + 1, end);
    
    return getCurrentEpoch(current, begin, mid - 1);
}

Value FeeSimulator::getValue(BlockHeight current) {
    unsigned int currentEpoch = getCurrentEpoch(current, 0, timeline.size() - 1);
    BlockHeight end = currentEpoch == timeline.size() - 1 ? last : timeline[currentEpoch + 1].getStart();
    return timeline[currentEpoch].getValue(current, end);
}

const BlockHeight FeeSimulator::numberOfBlocks() {
    return last;
}