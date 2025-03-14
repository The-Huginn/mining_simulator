//
//  blockchain.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//
//  Edited by Rastislav Budinsky on 10/31/22.
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//

#include "blockchain.hpp"
#include "utils.hpp"
#include "block.hpp"
#include "blockchain_settings.hpp"
#include "logging.h"

#include <cassert>
#include <unordered_set>
#include <iostream>
#include <limits>
#include <random>

Blockchain::Blockchain(BlockchainSettings blockchainSettings) :
    valueNetworkTotal(0),
    timeInSecs(0),
    secondsPerBlock(blockchainSettings.secondsPerBlock),
    feeSimulator(FeeSimulator(blockchainSettings)),
    _maxHeightPub(0)
{
    _blocks.reserve(rawCount(blockchainSettings.numberOfBlocks) * 2);
    _blocksIndex.resize(rawCount(blockchainSettings.numberOfBlocks) * 2);
    _smallestBlocks.resize(rawCount(blockchainSettings.numberOfBlocks) * 2);
    reset(blockchainSettings);
}

std::unique_ptr<Block> Blockchain::createBlock(const Block *parent, const Miner *miner, Value value) {
    Value txFees = value - parent->nextBlockReward() - parent->nextContractReward();
    auto block = std::make_unique<Block>(parent, miner, getTime(), txFees);
    COMMENTARY("\n\nparent mine time: " << parent->timeMined << " current time: " << getTime() << " height: " << parent->height + HeightType(1) << "\ntxFee to miner: " << txFees << " value added by feeSim: " << feeSimulator.getValue(parent->timeMined, getTime(), parent->height + HeightType(1)) << "\nContract reward: " << FeeContract::backToContract(txFees) << " total block value: " << block->value << "\ntxFeesInChain parent: " << parent->txFeesInChain << " FeesInChain now: " << block->txFeesInChain << std::endl << std::endl);
    return block;
}

void Blockchain::reset(BlockchainSettings blockchainSettings) {
    valueNetworkTotal = 0;
    timeInSecs = BlockTime(0);
    secondsPerBlock = blockchainSettings.secondsPerBlock;

    feeSimulator.reset();

    _maxHeightPub = BlockHeight(0);
    _blocks.clear();
    _smallestBlocks[0].clear();
    _blocksIndex[0].clear();
    auto genesis = std::make_unique<Block>(blockchainSettings.blockReward);
    _smallestBlocks[0].push_back(genesis.get());
    _blocksIndex[0].push_back(_blocks.size());
    _blocks.push_back(std::move(genesis));
}

void Blockchain::publishBlock(std::unique_ptr<Block> block) {
    assert(block);
    assert(block->height <= _maxHeightPub + BlockHeight(1));
    
    HeightType height = rawHeight(block->height);
    
    if (block->height > _maxHeightPub) {
        _blocksIndex[height].clear();
        _smallestBlocks[height].clear();
        _smallestBlocks[height].push_back(block.get());
        _maxHeightPub = block->height;
    } else {
        std::vector<Block *> &smallestVector = _smallestBlocks[height];
        
        if (block->value < smallestVector.front()->value) {
            smallestVector.clear();
            smallestVector.push_back(block.get());
        } else if (block->value == smallestVector.front()->value) {
            smallestVector.push_back(block.get());
        }
    }

    _blocksIndex[height].push_back(_blocks.size());
    _blocks.push_back(std::move(block));
}

BlockCount Blockchain::blocksOfHeight(BlockHeight height) const {
    return BlockCount(_blocksIndex[rawHeight(height)].size());
}

const std::vector<Block *> Blockchain::oldestBlocks(BlockHeight height) const {
    BlockTime minTimePublished(std::numeric_limits<BlockTime>::max());
    for (size_t index : _blocksIndex[rawHeight(height)]) {
        minTimePublished = std::min(_blocks[index]->getTimeBroadcast(), minTimePublished);
    }
    
    std::vector<Block *> possiblities;
    for (size_t index : _blocksIndex[rawHeight(height)]) {
        if (_blocks[index]->getTimeBroadcast() == minTimePublished) {
            possiblities.push_back(_blocks[index].get());
        }
    }
    assert(possiblities.size() > 0);
    return possiblities;
}

Block &Blockchain::oldest(BlockHeight height) const {
    auto possiblities = oldestBlocks(height);
    return *possiblities[selectRandomIndex(possiblities.size())];
}

Block &Blockchain::most(BlockHeight height) const {
    auto &smallestBlocks = _smallestBlocks[rawHeight(height)];
    size_t index = selectRandomIndex(smallestBlocks.size());
    Block *block = smallestBlocks[index];
    return *block;
}

const Block &Blockchain::winningHead() const {
    Value largestValue(0);
    for (size_t index : _blocksIndex[rawHeight(getMaxHeightPub())]) {
        largestValue = std::max(largestValue, _blocks[index]->valueInChain);
    }
    
    std::vector<Block *> possiblities;
    for (size_t index : _blocksIndex[rawHeight(getMaxHeightPub())]) {
        if (_blocks[index]->valueInChain == largestValue) {
            possiblities.push_back(_blocks[index].get());
        }
    }
    std::uniform_int_distribution<std::size_t> vectorDis(0, possiblities.size() - 1);
    return *possiblities[selectRandomIndex(possiblities.size())];
}

void Blockchain::advanceToTime(BlockTime time) {
    assert(time >= timeInSecs);
    valueNetworkTotal += FeeContract::getFeeRate(feeSimulator.addValueToChain(timeInSecs, time, getMaxHeightPub())).first;
    timeInSecs = time;
}

FeeSimulator Blockchain::getFeeSimulator() {
    return feeSimulator;
}

BlockValue Blockchain::expectedBlockSize() {
    return BlockValue(feeSimulator.getValue(timeInSecs, timeInSecs + secondsPerBlock, getMaxHeightPub()) +
            BlockValue(_smallestBlocks[rawHeight(getMaxHeightPub())].front()->nextBlockReward()) +
            BlockValue(_smallestBlocks[rawHeight(getMaxHeightPub())].front()->nextContractReward()));
}

TimeRate Blockchain::chanceToWin(HashRate hashRate) const {
    return hashRate / secondsPerBlock;
}

Block *Blockchain::blockByMinerAtHeight(BlockHeight height, const Miner &miner) const {
    for (size_t index : _blocksIndex[rawHeight(height)]) {
        if (_blocks[index]->minedBy(&miner)) {
            return _blocks[index].get();
        }
    }
    return nullptr;
}

Block &Blockchain::most(BlockHeight height, const Miner &miner) const {
    Block *block = blockByMinerAtHeight(height, miner);
    if (block) {
        return *block;
    }
    
    return most(height);
}

Block &Blockchain::oldest(BlockHeight height, const Miner &miner) const {
    Block *block = blockByMinerAtHeight(height, miner);
    if (block) {
        return *block;
    }
    
    return oldest(height);
}

Value Blockchain::gap(BlockHeight height) const {
    return rem(most(height - BlockHeight(1))) - rem(most(height));
}

Value Blockchain::rem(const Block &block) const {
    return valueNetworkTotal - block.txFeesInChain;
}

const std::vector<const Block *> Blockchain::getHeads() const {
    std::unordered_set<const Block *> nonHeadBlocks;
    std::unordered_set<const Block *> possibleHeadBlocks;
    for (auto &block : _blocks) {
        if (block->parent != nullptr) {
            nonHeadBlocks.insert(block->parent);
            possibleHeadBlocks.erase(block->parent);
        }
        if (nonHeadBlocks.find(block.get()) == end(nonHeadBlocks)) {
            possibleHeadBlocks.insert(block.get());
        }
    }
    return std::vector<const Block *>(std::begin(possibleHeadBlocks), std::end(possibleHeadBlocks));
}

void Blockchain::printBlockchain() const {
    COMMENTARY("Total tx fees value in chain (without block reward and fee contracts): " << valueNetworkTotal << std::endl);
    std::unordered_set<const Block *> printedBlocks;
    for (auto &current : getHeads()) {
        auto chain = current->getChain();
        for (auto block : chain) {
            if (printedBlocks.find(block) == end(printedBlocks)) {
                std::cout << *block;
                printedBlocks.insert(block);
            } else {
                break;
            }
        }
        std::cout << std::endl;
    }
}

void Blockchain::printHeads() const {
    std::cout << "heads:" << std::endl;
    for (auto current : getHeads()) {
        std::cout << *current << std::endl;
    }
    std::cout << "end heads." << std::endl;
}


