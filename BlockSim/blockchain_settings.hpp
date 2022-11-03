//
//  blockchain_settings.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/26/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//
//  Edited by Rastislav Budinsky 10/28/22
//  Copyright © 2016 Rastislav Budinsky. All rights reserved.
//

#ifndef blockchain_settings_hpp
#define blockchain_settings_hpp

#include "typeDefs.hpp"

struct BlockchainSettings {
    BlockRate secondsPerBlock;
    BlockValue blockReward;
    BlockCount numberOfBlocks;
};

#endif /* blockchain_settings_hpp */
