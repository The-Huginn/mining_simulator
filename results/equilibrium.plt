reset

# set title ""
set ylabel "Equilibrium Distribution of Strategies"
set xlabel "Fraction Default Compliant"
set bmargin 6
set tmargin 6
set xrange [0:0.27]
# set yrange [0:1]

# set key outside
# unset key
set key box opaque
set style fill solid

set term pngcairo interlace enhanced crop size 1000, 1000
set output "equilibrium.png"

default = "violet"
petty = "red"
lazy = "green"
fork1 = "yellow"
fork2 = "light-green"
fork4 = "blue"
fork8 = "dark-violet"
black = "black"

# Sum all weights and ignore all below 0.01 i.e. 1%
total = "tail -n 1 mult-$i/index-* | grep 300000 | awk '$2 >= 0.01 {sum += $2} END{print sum}'"
printing = sprintf("awk -v x=$i -v total=$(%s) '{if ($2 > 0.01) print x/100 \" \" $2/total*(100-x)/100; else print x/100 \" 0\";}'", total)

petty_values = sprintf("for i in {0..27..3}; do tail -n 1 mult-$i/index-0-petty-honest.txt | %s; done", printing)
fork1_values = sprintf("for i in {0..27..3}; do tail -n 1 mult-$i/index-1-function-fork-1.txt | %s; done", printing)
fork2_values = sprintf("for i in {0..27..3}; do tail -n 1 mult-$i/index-2-function-fork-2.txt | %s; done", printing)
fork4_values = sprintf("for i in {0..27..3}; do tail -n 1 mult-$i/index-3-function-fork-4.txt | %s; done", printing)
fork8_values = sprintf("for i in {0..27..3}; do tail -n 1 mult-$i/index-4-function-fork-8.txt | %s; done", printing)
lazy_values = sprintf("for i in {0..27..3}; do tail -n 1 mult-$i/index-5-lazy-fork.txt | %s; done", printing)

set arrow from graph 0,first 1 to graph 1,first 0.73 nohead linewidth 2 lc rgb black

plot 1 notitle with filledcurves x1 fillstyle solid fc rgb default,\
    1/0 w points pt 6 lc rgb default title "Default Compliant", \
    1/0 w points pt 6 lc rgb fork8 title "f(x) = x/8", sprintf("< paste <(%s) <(%s) <(%s) <(%s) <(%s) <(%s)", fork8_values, fork4_values, fork2_values, lazy_values, fork1_values, petty_values) using 1:(($2 + $4 + $6 + $8 + $10 + $12)) notitle w filledcurves x1 lc rgb fork8, \
    sprintf("< paste <(%s) <(%s) <(%s) <(%s) <(%s) <(%s)", fork8_values, fork4_values, fork2_values, lazy_values, fork1_values, petty_values) using 1:(($2 + $4 + $6 + $8 + $10 + $12)) notitle w line lc rgb black lw 2, \
    1/0 w points pt 6 lc rgb fork4 title "f(x) = x/4", sprintf("< paste <(%s) <(%s) <(%s) <(%s) <(%s)", fork4_values, fork2_values, lazy_values, fork1_values, petty_values) using 1:(($2 + $4 + $6 + $8 + $10)) notitle w filledcurves x1 lc rgb fork4, \
    sprintf("< paste <(%s) <(%s) <(%s) <(%s) <(%s)", fork4_values, fork2_values, lazy_values, fork1_values, petty_values) using 1:(($2 + $4 + $6 + $8 + $10)) notitle w line lc rgb black lw 2, \
    1/0 w points pt 6 lc rgb fork2 title "f(x) = x/2", sprintf("< paste <(%s) <(%s) <(%s) <(%s)", fork2_values, lazy_values, fork1_values, petty_values) using 1:(($2 + $4 + $6 + $8)) notitle w filledcurves x1 lc rgb fork2, \
    sprintf("< paste <(%s) <(%s) <(%s) <(%s)", fork2_values, lazy_values, fork1_values, petty_values) using 1:(($2 + $4 + $6 + $8)) notitle w line lc rgb black lw 2, \
    1/0 w points pt 6 lc rgb lazy title "Lazy", sprintf("< paste <(%s) <(%s) <(%s)", lazy_values, fork1_values, petty_values) using 1:(($2 + $4 + $6)) notitle w filledcurves x1 lc rgb lazy, \
    sprintf("< paste <(%s) <(%s) <(%s)", lazy_values, fork1_values, petty_values) using 1:(($2 + $4 + $6)) notitle w line lc rgb black lw 2, \
    1/0 w points pt 6 lc rgb fork1 title "f(x) = x", sprintf("< paste <(%s) <(%s)", fork1_values, petty_values) using 1:(($2 + $4)) notitle w filledcurves x1 lc rgb fork1, \
    sprintf("< paste <(%s) <(%s)", fork1_values, petty_values) using 1:(($2 + $4)) notitle w line lc rgb black lw 2, \
    1/0 w points pt 6 lc rgb petty title "Petty Compliant", sprintf("< %s", petty_values) using 1:2 notitle w filledcurves x1 lc rgb petty, \
    sprintf("< %s", petty_values) using 1:2 notitle w line lc rgb black lw 2