//
//  exp3_learning_model.hpp
//  BlockSim
//
//  Created by Harry Kalodner on 10/24/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//
//  Edited by Rastislav Budinsky on 11/11/22.
//  Copyright © 2022 Rastislav Budinsky. All rights reserved.
//

#ifndef exp3_learning_model_hpp
#define exp3_learning_model_hpp

#include "learning_model.hpp"

#include <stdio.h>

class Exp3LearningModel : public LearningModel {
private:
    std::vector<std::vector<StratWeight>> minersWeights;
    std::vector<std::vector<double>> minersProbabilities;
    
    void updateWeights(std::vector<Value> profits, Value maxPossibleProfit, double phi) override;
    std::vector<double> probabilitiesForMiner(size_t minerIndex, double phi) override;
    
public:
    Exp3LearningModel(std::vector<std::unique_ptr<LearningStrategy>> &learningStrategies, size_t minerCount, std::string resultFolder);
};



#endif /* exp3_learning_model_hpp */
