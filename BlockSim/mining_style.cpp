//
//  mining.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/11/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//
//  Edited by Rastislav Budinsky on 11/6/22.
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//

#include "mining_style.hpp"

#include "blockchain.hpp"
#include "miner.hpp"
#include "block.hpp"

#include <assert.h>

MiningStyle::MiningStyle(ParentSelectorFunc parentSelectorFunc_, BlockValueFunc blockValueFunc_) :
parentSelectorFunc(parentSelectorFunc_), blockValueFunc(blockValueFunc_) {}

MiningStyle::~MiningStyle() = default;

std::unique_ptr<Block> MiningStyle::createBlock(Blockchain &blockchain, const Miner &miner) {
    auto &parent = parentSelectorFunc(miner, blockchain);
    auto value = blockValueFunc(blockchain, parent);
    
    assert(value >= parent.nextBlockReward() + parent.nextContractReward());
    assert(value <= parent.nextBlockReward() + parent.nextContractReward() + blockchain.rem(parent));
    
    auto newBlock = blockchain.createBlock(&parent, &miner, value);
    
    return newBlock;
}
