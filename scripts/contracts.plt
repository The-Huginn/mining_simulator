reset

folder = "../simulation/percentage/90"
name = "contracts-4-90"
length = 60000
scale = 10e10
scale2 = 10e9

set title "Stacked value in contracts"
set ylabel "Value"
set xlabel "Blocks"
set bmargin 6
set tmargin 6
set xrange [0:length]
set yrange [0:]

set y2tics nomirror
set format y '%#.4g'

set key outside
unset key
set style fill solid

set logscale y 2

set term pngcairo interlace enhanced crop size 1000, 750
set output sprintf("%s/%s.png", folder, name)

violet = "violet"
green = "green"
yellow = "yellow"
red = "red"
black = "black"

# plot 1/0 w points pt 7 lc rgb red title "Contract length 2016",sprintf("%s/toContract-afterEvolution.txt", folder) using (($2)/scale) notitle w filledcurves x1 lc rgb red,\
    # 1/0 w points pt 7 lc rgb black title "Expected income from Contract", sprintf("%s/toContract-afterEvolution.txt", folder) using ($1/scale2) w points ps 0.3 pt 7 lc rgb black notitle axes x1y2
    
plot 1/0 w points pt 7 lc rgb violet title "Contract length 8064", sprintf("%s/toContract-afterEvolution.txt", folder) using (($2 + $3 + $4 + $5)/scale) notitle w filledcurves x1 lc rgb violet,\
    1/0 w points pt 7 lc rgb green title "Contract length 4032",sprintf("%s/toContract-afterEvolution.txt", folder) using (($2 + $3 + $4)/scale) notitle w filledcurves x1 lc rgb green,\
    1/0 w points pt 7 lc rgb yellow title "Contract length 2016",sprintf("%s/toContract-afterEvolution.txt", folder) using (($2 + $3)/scale) notitle w filledcurves x1 lc rgb yellow,\
    1/0 w points pt 7 lc rgb red title "Contract length 1008",sprintf("%s/toContract-afterEvolution.txt", folder) using (($2)/scale) notitle w filledcurves x1 lc rgb red,\
    1/0 w points pt 7 lc rgb black title "Expected income from Contract", sprintf("%s/toContract-afterEvolution.txt", folder) using ($1/scale2) w points ps 0.3 pt 7 lc rgb black notitle axes x1y2
# unset border
# unset tics

# plot [0:1] [1:0] 1/0 w points pt 7 lc rgb violet title "Contract length 8064",\
    # 1/0 w points pt 7 lc rgb green title "Contract length 4032",\
    # 1/0 w points pt 7 lc rgb yellow title "Contract length 2016",\
    # 1/0 w points pt 7 lc rgb red title "Contract length 1008",\
    # 1/0 w points pt 7 lc rgb black title "Expected income from Contract"