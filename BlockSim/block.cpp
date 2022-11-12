//
//  block.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//
// Edited by Rastislav Budinsky on 
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//

#include "block.hpp"
#include "miner.hpp"

#include <cassert>
#include <iostream>
#include <limits>

constexpr auto timeMax = std::numeric_limits<TimeType>::max();

Block::Block(BlockValue blockReward_) : Block(nullptr, nullptr, BlockTime(0), Value(0), BlockHeight(0), Value(0), Value(rawValue(blockReward_))) {}

Block::Block(const Block *parent_, const Miner *miner_, BlockTime timeSeconds_, Value txFees, BlockHeight height_, Value txFeesInChain_, Value blockReward_) : timeBroadcast(timeMax), parent(parent_), miner(miner_), height(height_), timeMined(timeSeconds_), txFeesInChain(txFeesInChain_), blockReward(blockReward_) {
    if (parent != nullptr) {
        Value toContracts = FeeContract::backToContract(txFees);
        std::pair<Value, std::unique_ptr<FeeContract>> next = parent->feeContract->claimAndCollect(toContracts);
        value = txFees + parent->nextBlockReward() + next.first;
        feeContract = std::move(next.second);
        valueInChain = parent->valueInChain + value;

    } else {
        feeContract = std::make_unique<FeeContract>(nullptr);
        value = 0;
        valueInChain = 0;
    }
}

Block::Block(const Block *parent_, const Miner *miner_, BlockTime timeSeconds_, Value txFees) :
    Block(parent_, miner_, timeSeconds_, txFees, parent_->height + BlockHeight(1), parent_->txFeesInChain + txFees, parent_->nextBlockReward()) {}

Value Block::nextBlockReward() const {
    return blockReward;
}

Value Block::nextContractReward() const {
    return feeContract->nextClaim();
}

void Block::broadcast(BlockTime timePub) {
    timeBroadcast = timePub;
}

bool Block::isBroadcast() const {
    return timeBroadcast < timeMax;
}

BlockTime Block::getTimeBroadcast() const {
    return timeBroadcast;
}

std::ostream& operator<< (std::ostream& out, const Block& mc) {
    mc.print(out, true);
    return out;
}

std::vector<const Block *> Block::getChain() const {
    std::vector<const Block *> chain;
    const Block *current = this;
    while (current) {
        chain.push_back(current);
        current = current->parent;
    }
    return chain;
}

void Block::print(std::ostream& os, bool isPublished) const {
    if (height == BlockHeight(0)) {
        os << "[h:0, m:gen]";
        return;
    }
    if (isPublished) {
        os << "{";
    }
    else {
        os << "[";
    }
    
    os << "h:" << height << ", m:" << miner->params.name << ", v:" << value << ", t:" << timeMined;
    
    if (isPublished) {
        os << "}->";
    }
    else {
        os << "]->";
    }
}

bool Block::minedBy(const Miner *miner_) const {
    return miner == miner_;
}
