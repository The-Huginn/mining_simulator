# First Experiment

## Contracts
* share: 100%, length: 2016

## Percentages
* 30%
* 50%
* 70%
* 90%

## Tx-Fee Simulator
* fullMempool: true
* mode: 0, reward: 50 BTC, i.e. 5000000000

## Miners
* 100% Default

## Games
* 1

## Expected length
* 100000

# Settings

## Contracts
We can specify how many of different-length contracts and how many percent of the total 100% split each contract receives. Contracts redistribute reward back simply by their total value divided by their length.

### Percentages
This states how much percent of simulated tx fees go to the contracts

## Tx-Fee Simulator
This component simulates incoming fees into the blockchain. We can use one of 3 modes defined here.

### Full Mempool:
* true - simulating full mempool, reward per block is equal to reward specified in Tx-Fee Simulator
* false - simulating incoming transaction, calculated from Tx-Fee Simulator epochs and average block time.

#### **Please note** it is highly recommended to use Full Mempool set to true as the forking makes the average block time much longer and results might deviate quite heavily.

### Modes:
* 0 - constant reward per block
* 1 - linearly increasing reward from start to the next fee epoch
* 2 - pseudo-randomly generated reward based on normal distribution

If no mode is selected then simulation defaults to type 2 with default values in config file.

## Miners
* Default - mining accordingly to the protocol
* Petty-Honest - mining on top of most profitable chain for him
* Fork-Miner - undercutting mining with keeping fraction speficied by name e.g. 2 => keeps 1/2 of fees
* Lazy-Fork - undercutting mining with keeping 1/2 of the fees

For Better understanding of miners please navigate to original [paper](https://www.cs.princeton.edu/~arvindn/publications/mining_CCS.pdf)

## Games
Sets how many number of games are played

## Expected length
Sets how many blocks should be created in total.
#### **Please note** because of forking this might result it shorter final chain then this parameter specifies.