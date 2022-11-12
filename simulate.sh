# !/bin/bash

length=2016
mempool=false

games=100

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
    echo -e "-v     Script will not redirect output of program and will display it in the console.\n"
}


#####################
# Loading arguments #
#####################
while getopts "schprg:v" opt;
do
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
            re='^[0-9]+$'
            if ! [[ $games =~ $re ]] ; then
                echo "Number of games is expected to be integer, exiting..." >&2
                exit 1
            fi
            echo -e "Each simulation will be run with ${games} games.\n"
            ;;
        v)
            echo -e "Will be priting outpus of program.\n"
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

[ ! -f $contracts ] && jq -n "{\"value\":0,\"toContracts\":0,\"contracts\":[{\"percentage\":100,\"length\":${length}}]}" > $contracts

tmp=$(mktemp /tmp/tmp.XXXXXXX)

#######################
# Execution of script #
#######################

echo -e "\n----------------\n"

set_fee_values() {
    # Total expected from contracts
    value=$((fees * $1 / 100))

    newValues=$(jq -r ".contracts[] | .value = ${value} * .percentage / 100 * .length" $contracts)
    newValues=$(echo $newValues | jq -n ".contracts |= [inputs]")
}

run_simulation() {
    if [ $PRINT_OUTPUT ]
    then
        ./strat $games
    else
        ./strat $games >/dev/null 2>&1
    fi
}

if [ $PERCENTAGE ]
then
    for i in {0..95..5}
    do
        echo "toContract set to $i"

        set_fee_values $i

        # echo $newValues | jq ".toContracts = ${i}"  > "$tmp" && mv "$tmp"  $contracts
        jq ".contracts = ${newValues}.contracts | .toContracts = ${i}" $contracts > "$tmp" && mv "$tmp"  $contracts
        
        run_simulation

        mkdir -p $outputDir/$i
        cp contracts/game-0.txt $outputDir/$i/toContract-beforeEvolution.txt
        cp contracts/game-$(($games - 1)).txt $outputDir/$i/toContract-afterEvolution.txt
        cp $indexDir/* $outputDir/$i
    done
fi

if [ $SINGLE ]
then
    set_fee_values $(jq ".toContracts" $contracts)
    jq ".contracts = ${newValues}.contracts" $contracts > "$tmp" && mv "$tmp"  $contracts

    run_simulation

    mkdir -p $outputDir/single
    cp contracts/game-0.txt $outputDir/single/toContract.txt
    cp $indexDir/* $outputDir/single
fi

mv $tmpDir/$contracts .
mv $tmpDir/$simulator .

rm -r $tmpDir

echo -e "\n----------------\n"
echo -e "Results can be found under directory ${outputDir}"