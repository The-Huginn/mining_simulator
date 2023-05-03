reset

folder = "../simulation/single"
name = "fees-1-percentage"
length = 49000
average = 5
# scale = 10e9
scale = 1

set title "Incoming fees and block values"
set ylabel "Value"
set xlabel "Blocks"
set bmargin 6
set tmargin 3
set key box opaque
set offset .05, .05
# set xrange [0:length] reenable after and remove every ::::
set xrange [0:2016]
# set yrange [0:4e10]

set format y '%#.2g'

set key top right
# set key outside
# unset key
set style fill solid


set term pngcairo interlace enhanced size 1000, 650
set output sprintf("%s/%s.png", folder, name)

blue = "blue"
green = "green"
black = "black"

plot 1/0 w points pt 7 lw 6 lc rgb blue title "Fees in mempool", sprintf("< ./run-average.awk size=%d %s/feeSimulator-afterEvolution.txt", average, folder) every ::21168::23184 using 0:($1/scale) w points ps 0.3 pt 7 lc rgb blue notitle,\
    1/0 w points pt 7 lw 6 lc rgb green title "Block Value", sprintf("< ./run-average.awk size=%d %s/blockValues-afterEvolution.txt", average, folder) every ::21168::23184 using 0:($1/scale) w points ps 0.3 pt 7 lc rgb green notitle,\
    1/0 w points pt 7 lw 6 lc rgb black title "Expected income from Contract", sprintf("%s/toContract-afterEvolution.txt", folder) every ::21168::23184 using 0:($1/scale) w points ps 0.3 pt 7 lc rgb black notitle

# unset border
# unset tics

# plot [0:1] [1:0] 1/0 w points pt 7 lw 6 lc rgb blue title "Fees in mempool",\
#     1/0 w points pt 7 lw 6 lc rgb green title "Block Value",\
#     1/0 w points pt 7 lw 6 lc rgb black title "Expected income from Contract"