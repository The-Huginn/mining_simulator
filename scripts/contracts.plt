reset

folder = "../simulation/single"
name = "contracts-1-percentage"
length = 49000
# scale = 10e10
# scale2 = 10e9
scale = 1
scale2 = 1

set title "Separate values from nextClaim"
set ylabel "Value"
set xlabel "Blocks"
set bmargin 6
set tmargin 6
set xrange [0:length]
set yrange [0:2e10]
# set yrange [0:3e8]

# set y2tics nomirror
set format y '%#.2g'

set key top right
# set key outside
# unset key
set style fill solid

# set logscale y 2

set term pngcairo interlace enhanced size 1000, 650
set output sprintf("%s/%s.png", folder, name)

violet = "violet"
green = "green"
yellow = "yellow"
red = "red"
black = "black"

plot 1/0 w points pt 7 lc rgb black title "nextClaim", sprintf("%s/toContract-afterEvolution.txt", folder) using (($1)/scale2) w points ps 0.3 pt 7 lc rgb black notitle

# plot 1/0 w points pt 7 lc rgb violet title "partClaim^{4}", sprintf("%s/toContract-afterEvolution.txt", folder) using (($2 + $3 + $4 + $5)/scale) notitle w filledcurves x1 lc rgb violet,\
#     1/0 w points pt 7 lc rgb green title "partClaim^{3}",sprintf("%s/toContract-afterEvolution.txt", folder) using (($2 + $3 + $4)/scale) notitle w filledcurves x1 lc rgb green,\
#     1/0 w points pt 7 lc rgb yellow title "partClaim^{2}",sprintf("%s/toContract-afterEvolution.txt", folder) using (($2 + $3)/scale) notitle w filledcurves x1 lc rgb yellow,\
#     1/0 w points pt 7 lc rgb red title "partClaim^{1}",sprintf("%s/toContract-afterEvolution.txt", folder) using (($2)/scale) notitle w filledcurves x1 lc rgb red,\
#     1/0 w points pt 7 lc rgb black title "nextClaim", sprintf("%s/toContract-afterEvolution.txt", folder) using ($1/scale2) w points ps 0.3 pt 7 lc rgb black notitle

# plot 1/0 w points pt 7 lc rgb violet title "partClaim^{4}", sprintf("%s/toContract-afterEvolution.txt", folder) using (($5)/51) notitle w points ps 0.3 pt 7 lc rgb violet,\
#     1/0 w points pt 7 lc rgb green title "partClaim^{3}",sprintf("%s/toContract-afterEvolution.txt", folder) using (($4)/28) notitle w points ps 0.3 pt 7 lc rgb green,\
#     1/0 w points pt 7 lc rgb yellow title "partClaim^{2}",sprintf("%s/toContract-afterEvolution.txt", folder) using (($3)/14) notitle w points ps 0.3 pt 7 lc rgb yellow,\
#     1/0 w points pt 7 lc rgb red title "partClaim^{1}",sprintf("%s/toContract-afterEvolution.txt", folder) using (($2)/7) notitle w points ps 0.3 pt 7 lc rgb red
    
    
    
# unset border
# unset tics

# plot [0:1] [1:0] 1/0 w points pt 7 lc rgb violet title "Contract length 8064",\
    # 1/0 w points pt 7 lc rgb green title "Contract length 4032",\
    # 1/0 w points pt 7 lc rgb yellow title "Contract length 2016",\
    # 1/0 w points pt 7 lc rgb red title "Contract length 1008",\
    # 1/0 w points pt 7 lc rgb black title "Expected income from Contract"