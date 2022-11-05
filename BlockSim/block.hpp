//
//  block.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef block_hpp
#define block_hpp

#include "typeDefs.hpp"
#include "FeeContract.hpp"

#include <vector>
#include <iterator>
#include <memory>

class Miner;

class Block {
protected:
    BlockTime timeBroadcast;
public:
    const Block *parent;
    const Miner *miner;
    BlockHeight height;
    BlockTime timeMined;
    Value value;
    Value txFeesInChain;
    Value valueInChain;
    Value blockReward;
    std::unique_ptr<FeeContract> feeContract;
    
    Block(const Block *parent_, const Miner *miner_, BlockTime timeSeconds, Value txFees, BlockHeight height, Value txFeesInChain, Value blockReward);
    
    Block(BlockValue blockReward, std::unique_ptr<FeeContract> _feeContract);
    Block(const Block *parent_, const Miner *miner_, BlockTime timeSeconds_, Value txFees);
    
    void reset(const Block *parent, const Miner *miner, BlockTime timeSeconds, Value txFees);
    
    void broadcast(BlockTime timePub);
    BlockTime getTimeBroadcast() const;
    bool isBroadcast() const;
    
    Value nextBlockReward() const;
    
    bool minedBy(const Miner *miner) const;
    void print(std::ostream& where, bool isPublished) const;
    std::vector<const Block *> getChain() const;
    
};

std::ostream& operator<< (std::ostream& out, const Block& mc);

#endif /* block_hpp */
