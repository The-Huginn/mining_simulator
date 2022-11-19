reset

folder = "../simulation/percentage/90"
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
set key center top
set style fill solid

# set term pngcairo interlace enhanced crop size 1920, 1080
# set output "fees.png"
dpi = 300 ## dpi (variable)
width = 90 ## mm (variable)
height = 164.5 ## mm (variable)

in2mm = 25.4 # mm (fixed)
pt2mm = 0.3528 # mm (fixed)

mm2px = dpi/in2mm
ptscale = pt2mm*mm2px
round(x) = x - floor(x) < 0.5 ? floor(x) : ceil(x)
wpx = round(width * mm2px)
hpx = round(height * mm2px)
# set terminal pngcairo size 1920, 1080
# set output "fees.png"

set term pngcairo size 1920, 1080
set output sprintf("%s/fees.png", folder)
blue = "blue"
green = "green"
black = "black"

plot 1/0 w points pt 7 lw 6 lc rgb blue title "Fees in mempool", sprintf("< ./run-average.awk size=%d %s/feeSimulator-afterEvolution.txt", average, folder) w points ps 0.3 pt 7 lc rgb blue notitle,\
    1/0 w points pt 7 lw 6 lc rgb green title "Block Value", sprintf("< ./run-average.awk size=%d %s/blockValues-afterEvolution.txt", average, folder) w points ps 0.3 pt 7 lc rgb green notitle,\
    1/0 w points pt 7 lw 6 lc rgb black title "Expected income from Contract", sprintf("%s/toContract-afterEvolution.txt", folder) using 0:1 w points ps 0.3 pt 7 lc rgb black notitle