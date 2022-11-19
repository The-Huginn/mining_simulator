reset

folder = "../simulation/percentage/90"
name = "fees-4-90"
length = 60000
average = 5

set title "Incoming fees and block values"
set ylabel "Value"
set xlabel "Blocks"
set bmargin 6
set tmargin 3
set offset .05, .05
set xrange [0:length]
# set yrange [0:4e10]

set format y '%#.2g'

# set key at screen 0.1, screen 0.8
set key outside
set style fill solid

# set term pngcairo interlace enhanced crop size 1920, 1080

set term pngcairo size 1920, 1080
set output sprintf("%s/%s.png", folder, name)

blue = "blue"
green = "green"
black = "black"

plot 1/0 w points pt 7 lw 6 lc rgb blue title "Fees in mempool", sprintf("< ./run-average.awk size=%d %s/feeSimulator-afterEvolution.txt", average, folder) w points ps 0.3 pt 7 lc rgb blue notitle,\
    1/0 w points pt 7 lw 6 lc rgb green title "Block Value", sprintf("< ./run-average.awk size=%d %s/blockValues-afterEvolution.txt", average, folder) w points ps 0.3 pt 7 lc rgb green notitle,\
    1/0 w points pt 7 lw 6 lc rgb black title "Expected income from Contract", sprintf("%s/toContract-afterEvolution.txt", folder) using 0:1 w points ps 0.3 pt 7 lc rgb black notitle