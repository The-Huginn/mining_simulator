reset

folder = "../simulation/percentage/90"
name = "contracts-4-90"
length = 60000

set title "Stacked value in contracts"
set ylabel "Value"
set xlabel "Blocks"
set bmargin 6
set tmargin 6
set xrange [0:length]
set yrange [6e10:]

set y2tics nomirror
set format y '%#.4g'

set key outside
set style fill solid

set logscale y 2

set term pngcairo size 1920, 1080
set output sprintf("%s/%s.png", folder, name)

violet = "violet"
green = "green"
yellow = "yellow"
red = "red"
black = "black"

plot 1/0 w points pt 7 lc rgb violet title "Length 8064", sprintf("%s/toContract-afterEvolution.txt", folder) using ($2 + $3 + $4 + $5) notitle w filledcurves x1 lc rgb violet,\
    1/0 w points pt 7 lc rgb green title "Length 4032",sprintf("%s/toContract-afterEvolution.txt", folder) using ($2 + $3 + $4) notitle w filledcurves x1  lc rgb green,\
    1/0 w points pt 7 lc rgb yellow title "Length 2016",sprintf("%s/toContract-afterEvolution.txt", folder) using ($2 + $3) notitle w filledcurves x1  lc rgb yellow,\
    1/0 w points pt 7 lc rgb red title "Length 1008",sprintf("%s/toContract-afterEvolution.txt", folder) using ($2) notitle w filledcurves x1  lc rgb red,\
    1/0 w points pt 7 lc rgb black title "Expected reward", sprintf("%s/toContract-afterEvolution.txt", folder) using 0:1 w points ps 0.3 pt 7 lc rgb black notitle axes x1y2