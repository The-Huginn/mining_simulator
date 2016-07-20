//
//  petty_miner.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#ifndef petty_miner_hpp
#define petty_miner_hpp

#include <memory>

class Strategy;

Strategy createPettyStrategy(bool noSelfMining, bool noiseInTransactions);

#endif /* petty_miner_hpp */
