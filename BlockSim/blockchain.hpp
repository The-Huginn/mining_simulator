//
//  blockchain.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//
//  Edited by Rastislav Budinsky on 10/31/22.
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//

#ifndef blockchain_hpp
#define blockchain_hpp

#include "typeDefs.hpp"
#include "FeeContract.hpp"
#include "FeeSimulator.hpp"

#include <queue>
#include <stddef.h>
#include <memory>

class Block;
class GenesisBlock;
class Miner;
struct BlockchainSettings;

class Blockchain {    
    Value valueNetworkTotal;
    BlockTime timeInSecs;
    BlockRate secondsPerBlock;
    FeeSimulator feeSimulator;
    
    BlockHeight _maxHeightPub;
    std::vector<std::vector<size_t>> _blocksIndex;      // holds indexes to all blocks at their respective height
    std::vector<std::vector<Block *>> _smallestBlocks; // cache smallest blocks of a given height
    std::vector<std::unique_ptr<Block>> _blocks;        // holds all blocks created
    
    Block *blockByMinerAtHeight(BlockHeight height, const Miner &miner) const;
    
public:
    Blockchain(BlockchainSettings blockchainSettings);
    
    std::unique_ptr<Block> createBlock(const Block *parent, const Miner *miner, Value value);
    void reset(BlockchainSettings blockchainSettings);

    void publishBlock(std::unique_ptr<Block> block);
    
    const std::vector<const Block *> getHeads() const;
    void printBlockchain() const;
    void printHeads() const;
    
    const Block &winningHead() const;
    
    BlockCount blocksOfHeight(BlockHeight height) const;
    
    const std::vector<Block *> oldestBlocks(BlockHeight height) const;
    Block &oldest(BlockHeight height) const;
    Block &most(BlockHeight age) const;

    FeeSimulator getFeeSimulator();
    
    void advanceToTime(BlockTime time);
    
    inline BlockHeight getMaxHeightPub() const {
        return _maxHeightPub;
    }
    
    inline BlockTime getTime() const {
        return timeInSecs;
    }
    
    inline Value getTotalFees() const {
        return valueNetworkTotal;
    }
    
    BlockValue expectedBlockSize();
    TimeRate chanceToWin(HashRate hashRate) const;
    
    Value gap(BlockHeight i) const;
    Value rem(const Block &block) const;
    
    Block &most(BlockHeight age, const Miner &miner) const;
    Block &oldest(BlockHeight age, const Miner &miner) const;
};

#endif /* blockchain_hpp */
