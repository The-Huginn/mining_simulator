//
//  main.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//
//  Edited by Rastislav Budinsky on 10/31/22.
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//


#include "BlockSim/block.hpp"
#include "BlockSim/blockchain.hpp"
#include "BlockSim/minerStrategies.h"
#include "BlockSim/logging.h"
#include "BlockSim/game.hpp"
#include "BlockSim/minerGroup.hpp"
#include "BlockSim/miner_result.hpp"
#include "BlockSim/game_result.hpp"
#include "BlockSim/typeDefs.hpp"
#include "BlockSim/miner.hpp"
#include "BlockSim/StatLogger.hpp"

#include "multiplicative_weights_learning_model.hpp"
#include "exp3_learning_model.hpp"
#include "learning_strategy.hpp"

#include <iostream>
#include <math.h>

//--more representative of smaller miners, where the chance that you mine the
//next block is ~0 (not to be confused with the strategy selfish mining)

#define ATOMIC false     //not realistic, but do not force miners to mine on top of their own blocks
#define NOISE_IN_TRANSACTIONS false //miners don't put the max value they can into a block (simulate tx latency)

#define NETWORK_DELAY BlockTime(0)         //network delay in seconds for network to hear about your block
#define EXPECTED_NUMBER_OF_BLOCKS BlockCount(10000)

#define LAMBERT_COEFF 0.13533528323661//coeff for lambert func equil  must be in [0,.2]
//0.13533528323661 = 1/(e^2)

#define SEC_PER_BLOCK BlockRate(600)     //mean time in seconds to find a block

#define B BlockValue(0 * SATOSHI_PER_BITCOIN) // Block reward
#define A BlockValue(50 * SATOSHI_PER_BITCOIN)/SEC_PER_BLOCK  //rate transactions come in


struct RunSettings {
    unsigned int numberOfGames;
    MinerCount totalMiners;
    MinerCount fixedDefault;
    GameSettings gameSettings;
    std::string folderPrefix;
};

void runStratGame(RunSettings settings, std::vector<std::unique_ptr<LearningStrategy>> &learningStrategies, std::unique_ptr<Strategy> defaultStrategy);

void runSingleStratGame(RunSettings settings);

void runStratGame(RunSettings settings, std::vector<std::unique_ptr<LearningStrategy>> &learningStrategies, std::unique_ptr<Strategy> defaultStrategy) {
    
    //start running games
    BlockCount totalBlocksMined(0);
    BlockCount blocksInLongestChain(0);
    
    std::string resultFolder = "";
    
    if (settings.folderPrefix.length() > 0) {
        resultFolder += settings.folderPrefix + "-";
    }

    StatLogger statLogger("contracts", "simulator");
    
    resultFolder += std::to_string(rawCount(settings.fixedDefault));
    
    std::vector<std::unique_ptr<Miner>> miners;
    std::vector<Miner *> learningMiners;
    
    HashRate hashRate(1.0/rawCount(settings.totalMiners));
    MinerCount numberRandomMiners(settings.totalMiners - settings.fixedDefault);
    
    for (MinerCount i(0); i < settings.totalMiners; i++) {
        auto minerName = std::to_string(rawCount(i));
        MinerParameters parameters {rawCount(i), minerName, hashRate, NETWORK_DELAY, COST_PER_SEC_TO_MINE};
        miners.push_back(std::make_unique<Miner>(parameters, *defaultStrategy));
        if (i < numberRandomMiners) {
            learningMiners.push_back(miners.back().get());
        }
    }
    
//    LearningModel *learningModel = new MultiplicativeWeightsLearningModel(learningStrategies, learningMiners.size(), resultFolder);
    LearningModel *learningModel = new Exp3LearningModel(learningStrategies, learningMiners.size(), resultFolder);
    
    MinerGroup minerGroup(std::move(miners));
    
//    double phi = std::sqrt(strategies.size() * std::log(strategies.size())) / std::sqrt(settings.numberOfGames);
//    double phi = std::sqrt(strategies.size() * std::log(strategies.size())) / std::sqrt(settings.numberOfGames / 100);
    
    double phi = .01;
    
    auto blockchain = std::make_unique<Blockchain>(settings.gameSettings.blockchainSettings);
    
    for (unsigned int gameNum = 0; gameNum < settings.numberOfGames; gameNum++) {
//        double n = gameNum;
//        double nMax = settings.numberOfGames;
//        double phi = std::pow(.9, (n / nMax) * 30.0);
        
        blockchain->reset(settings.gameSettings.blockchainSettings);
        
        learningModel->writeWeights(gameNum);
        minerGroup.reset(*blockchain);
        learningModel->pickNewStrategies(phi, learningMiners, *blockchain);
        minerGroup.resetOrder();
        
        GAMEINFO("\n\nGame#: " << gameNum << " The board is set, the pieces are in motion..." << std::endl);
        
        auto result = runGame(minerGroup, *blockchain, settings.gameSettings);
        statLogger.log(gameNum, *blockchain);


        GAMEINFO("Value in longest: " << result.moneyInLongestChain << " (" << gameNum + 1 << "/" << settings.numberOfGames << ")" << std::endl);
        GAMEINFO("The game is complete. Calculate the scores:" << std::endl);
        
        Value maxProfit = (A * (EXPECTED_NUMBER_OF_BLOCKS * settings.gameSettings.blockchainSettings.secondsPerBlock) - result.moneyLeftAtEnd) / Value(rawCount(settings.totalMiners) / 4);
        
        // Steps 3, 4, 5
        learningModel->updateWeights(result, maxProfit, phi);
        
        totalBlocksMined += result.totalBlocksMined;
        blocksInLongestChain += result.blocksInLongestChain;
        
       BlockCount staleBlocks(result.totalBlocksMined - result.blocksInLongestChain);
       std::cout << result.moneyInLongestChain << " in chain and " <<
       result.moneyLeftAtEnd << " left with " << 100 * double(rawCount(staleBlocks)) / double(rawCount(result.totalBlocksMined)) << "% orphan rate" <<  std::endl;
        
       std::cout << totalBlocksMined << " total blocks mined" << std::endl;
    }
    learningModel->writeWeights(settings.numberOfGames);
    
    GAMEINFOBLOCK(
                  GAMEINFO("Total blocks mined: " << totalBlocksMined << std::endl);
                  GAMEINFO("Longest chain: " << blocksInLongestChain << std::endl);
                  GAMEINFO("Games over. Final strategy weights:\n");
                  learningModel->printWeights();
                  )
    
    delete learningModel;
//    std::cout << 100 * double(rawCount(blocksInLongestChain)) / double(rawCount(totalBlocksMined)) << "% in final chain" << std::endl;
}

void runSingleStratGame(RunSettings settings) {
    using std::placeholders::_1;
    using std::placeholders::_2;
    
    std::vector<std::unique_ptr<LearningStrategy>> learningStrategies;
    
    StratWeight defaultWeight(1);
    
    std::unique_ptr<Strategy> defaultStrategy(createDefaultStrategy(ATOMIC, NOISE_IN_TRANSACTIONS));
    
    learningStrategies.push_back(std::make_unique<LearningStrategy>(createPettyStrategy(ATOMIC, NOISE_IN_TRANSACTIONS), defaultWeight));
    for (int i = -1; i < 3; i++) {
        int funcCoeff = static_cast<int>(pow(2, (i + 1)));
        std::function<Value(const Blockchain &, Value)> forkFunc(std::bind(functionForkPercentage, _1, _2, funcCoeff));
        learningStrategies.push_back(std::make_unique<LearningStrategy>(createFunctionForkStrategy(ATOMIC, forkFunc, std::to_string(funcCoeff)), defaultWeight));
    }
//  strategies.push_back(createFunctionForkStrategy(NO_SELF_MINING, std::bind(functionForkLambert, _1, _2, LAMBERT_COEFF), "lambert"));
    learningStrategies.push_back(std::make_unique<LearningStrategy>(createLazyForkStrategy(ATOMIC), defaultWeight));
    
    
    runStratGame(settings, learningStrategies, std::move(defaultStrategy));
}

int main(int argc, const char * argv[]) {
    
    if (argc < 3) {
        std::cerr << "Please specify how many games and length of chain using arguments" << std::endl;
        return -1;
    }

    unsigned int expectedBlocks = std::atoi(argv[1]);
    unsigned int gameCount = std::atoi(argv[2]);

    BlockchainSettings blockchainSettings = {SEC_PER_BLOCK, B, expectedBlocks};
    GameSettings gameSettings = {blockchainSettings};
    
//    
//    for (MinerCount i(0); i < MinerCount(71); i += MinerCount(6)) {
//        RunSettings runSettings = {300000, MinerCount(100), i, gameSettings, "mult"};
//        runSingleStratGame(runSettings);
//    }
    
    RunSettings runSettings = {gameCount, MinerCount(200), MinerCount(200), gameSettings, "test"};
    runSingleStratGame(runSettings);
    
}
