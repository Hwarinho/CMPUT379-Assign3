# Thanks to: https://stackoverflow.com/questions/55569996/gnuplot-2-questions-about-my-histograms-x-axis-and-add-percentage

f(w) = (strlen(w) > 10 ? word(w, 1) . "\n" . word(w, 2) : w)

set title "TLB Miss Ratio For FIFO Policy, Medium Data Size, With FLUSH 1,000,000, No (-i)"
set terminal png truecolor size 960, 720
set output "TLB_F.png"
set bmargin at screen 0.1
set key top right outside
set grid
set ylabel "TLB Miss Ratio (%)"
set xlabel "Programs with Different Conditions"
set style data histograms
set style fill solid 1.25 border -1
set boxwidth 0.65 relative
set yrange [0:]
set format y "%g%%"
set datafile separator ","
plot 'FIFO_i.csv' using 2:xtic(f(stringcolumn(1))) title "pgsize (512), tablesize (32)", \
'' using 3 title "pgsize (4096), tablesize (32)", \
'' using 4 title "pgsize (32768), tablesize (32)", \
'' using 5 title "pgsize (512), tablesize (128)", \
'' using 6 title "pgsize (4096), tablesize (128)", \
'' using 7 title "pgsize (32768), tablesize (128)", \
'' using 8 title "pgsize (512), tablesize (512) ", \
'' using 9 title "pgsize (4096), tablesize (512)", \
'' using 10 title "pgsize (32768), tablesize (512)"
