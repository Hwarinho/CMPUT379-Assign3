# Thanks to: https://stackoverflow.com/questions/55569996/gnuplot-2-questions-about-my-histograms-x-axis-and-add-percentage

f(w) = (strlen(w) > 10 ? word(w, 1) . "\n" . word(w, 2) : w)

set title "TLB Miss Ratio For Variation Blah"
set terminal png truecolor size 960, 720
set output "TLB.png"
set bmargin at screen 0.1
set key top right
set grid
set ylabel "TLB Miss Ratio (%)"
set xlabel "Programs with Different Conditions"
set style data histograms
set style fill solid 1.25 border -1
set boxwidth 0.65 relative
set yrange [0:]
set format y "%g%%"
set datafile separator ","
plot 'data.csv' using 2:xtic(f(stringcolumn(1))) title " Variation blah.. ", \
'' using 3 title " Variation blah.. ", \
'' using 4 title " Variation blah.. ", \
'' using 0:($2+1):(sprintf("%g%%",$2)) with labels notitle, \
'' using 0:($3+1):(sprintf("     %g%%",$3)) with labels notitle
