# !/bin/bash

# When using generated feeContract
length=2016
to_contracts=50

# When using generated feeSimulation
mempool=false

games=100
blockchain_length=10000

# Where evolution of miners is saved
indexDir="test-0"

tmpDir="tmp"

# Basing on 50BTC of fees per block and 100% to miner
# This is calculated on fees incomining based on time as in original simulator
# Should not be changed other parts are hard coded depending on this parameter
fees=5000000000

# -- Do not edit after this --

echo -e "\nSimulation for different contract percentage of fee ratio toMiner/toContract\n"

Color_Off='\033[0m'
Red='\033[0;31m'
Green='\033[0;32m'
Yellow='\033[0;33m'

contracts="feeContracts.json"
simulator="feeSimulation.json"

mkdir -p $tmpDir
mv $contracts $simulator $tmpDir

print_help() {
    echo -e "-h     Displays this message.${Yellow} One of these flags should be specified{-p, -r}\n${Color_Off}"
    echo -e "-c     Script will use custom feeContracts.json. It will not change your current feeContracts.json config file\n           Please note, if you do not change fees variable inside script it will always recalculate '.value' for every contract to match expected settings.\n"
    echo -e "-s     Script will use custom feeSimulation.json. It will not change your current feeSimulation.json config file\n"
    echo -e "-p     Script will run simulations with different '.percentage' parameter in feeContract.json\n"
    echo -e "-r     Script will run single simulation.\n"
    echo -e "-g     Requires one parameter. Sets how many games will be run for each execution of the simulator.\n"
    echo -e "-b     Requires one parameter. Sets how many block should be on average created.\n"
    echo -e "-v     Script will not redirect output of program and will display it in the console.\n ${Green}Please note you will need to call 'make clean' before calling this script after changing logging.h for changes to apply.\n ${Color_Off}"
}


#####################
# Loading arguments #
#####################
while getopts "schprg:b:v" opt;
do
    re='^[0-9]+$'
    case $opt in
        s)
            echo -e "Using custom Fee Simulator [$simulator]\n"
            cp $tmpDir/$simulator $simulator
            ;;

        c)
            echo -e "Using custom Fee Contracts [$contracts]\n"
            cp $tmpDir/$contracts $contracts
            ;;
        h)
            print_help
            exit 0
            ;;
        p)
            echo -e "Will try different '.percentage' parameters.\n"
            PERCENTAGE=true
            ;;
        r)
            echo -e "Will run single simulation.\n"
            SINGLE=true
            ;;
        g)
            games=${OPTARG}
            if ! [[ $games =~ $re ]] ; then
                echo "Number of games is expected to be integer, exiting..." >&2
                exit 1
            fi
            echo -e "Each simulation will be run with ${games} games.\n"
            ;;
        b)
            blockchain_length=${OPTARG}
            if ! [[ $blockchain_length =~ $re ]] ; then
                echo "Average number of block is expected to be integer, exiting..." >&2
                exit 1
            fi
            echo -e "Each simulation will try to create ${blockchain_length} blocks.\n"
            ;;
        v)
            echo -e "Will be priting outputs of program.\n"
            PRINT_OUTPUT=true
            ;;

    esac

done
shift $(($OPTIND -1))

if [ -z $PERCENTAGE ] && [ -z $SINGLE ]
then
    echo -e "${Red}Wrong usage\n${Color_Off}"
    print_help
    exit 0
fi

####################
# Set up variables #
####################

make all

outputDir=$([ $1 ] && echo $1 || echo "simulation")

[ ! -f $simulator ] && jq -n "{\"length\": 100000,\"mean\": 100000000,\"deviation\": 100000,\"fullMempool\": $mempool,\"timeline\": [{\"start\": 0,\"epochType\": 0,\"values\": [5000000000]}]}" > $simulator

[ ! -f $contracts ] && jq -n "{\"toContracts\":$to_contracts,\"contracts\":[{\"percentage\":100,\"length\":${length}}]}" > $contracts

tmp=$(mktemp /tmp/tmp.XXXXXXX)

#######################
# Execution of script #
#######################

echo -e "${Green}\n+----------------------+\n"
echo -e "| Starting Simulations |"
echo -e "\n+----------------------+\n${Color_Off}"

set_fee_values() {
    # Total expected from contracts
    value=$((fees * $1 / 100))

    # Might want to uncomment, but simulating with time-based reward is not recommended
    # if [ $(jq ".fullMempool" $simulator) == "false" ]
    # then
    #     value=$(echo "scale=0; $value * 2" | bc)
    # fi

    newValues=$(jq -r ".contracts[] | .value = ${value} * .percentage / 100 * .length" $contracts)
    newValues=$(echo $newValues | jq -n ".contracts |= [inputs]")
}

run_simulation() {
    rm -rf $indexDir/*
    if [ $PRINT_OUTPUT ]
    then
        ./strat $blockchain_length $games
    else
        ./strat $blockchain_length $games >/dev/null 2>&1
    fi
}

if [ $PERCENTAGE ]
then
    # for i in {0..95..5}
    for i in 30 50 70 90
    do
        echo "toContract set to $i"

        set_fee_values $i

        # echo $newValues | jq ".toContracts = ${i}"  > "$tmp" && mv "$tmp"  $contracts
        jq ".contracts = ${newValues}.contracts | .toContracts = ${i}" $contracts > "$tmp" && mv "$tmp"  $contracts
        
        run_simulation

        mkdir -p $outputDir/percentage/$i
        rm -rf $outputDir/percentage/$i/*

        cp contracts/game-0.txt $outputDir/percentage/$i/toContract-beforeEvolution.txt
        cp contracts/game-$(($games - 1)).txt $outputDir/percentage/$i/toContract-afterEvolution.txt
        cp simulator/game-0.txt $outputDir/percentage/$i/feeSimulator-beforeEvolution.txt
        cp simulator/game-$(($games - 1)).txt $outputDir/percentage/$i/feeSimulator-afterEvolution.txt
        cp $indexDir/* $outputDir/percentage/$i
    done
fi

if [ $SINGLE ]
then
    set_fee_values $(jq ".toContracts" $contracts)
    jq ".contracts = ${newValues}.contracts" $contracts > "$tmp" && mv "$tmp"  $contracts

    run_simulation

    mkdir -p $outputDir/single
    rm -rf $outputDir/single/*

    cp contracts/game-0.txt $outputDir/single/toContract-beforeEvolution.txt
    cp contracts/game-$(($games - 1)).txt $outputDir/single/toContract-afterEvolution.txt
    cp simulator/game-0.txt $outputDir/percentage/$i/feeSimulator-beforeEvolution.txt
    cp simulator/game-$(($games - 1)).txt $outputDir/percentage/$i/feeSimulator-afterEvolution.txt
    cp $indexDir/* $outputDir/single
fi

mv $tmpDir/$contracts .
mv $tmpDir/$simulator .

rm -r $tmpDir

echo -e "${Green}\n+----------------------+\n"
echo -e "| Finished Simulations |"
echo -e "\n+----------------------+\n${Color_Off}"
echo -e "Results can be found under directory ${outputDir}"