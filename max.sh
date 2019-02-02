gnuplot << EOP
set yrange[0:20]
set terminal jpeg size 640,480
set output "Max.jpg"
set title "Max"
plot 'plot' u 1:3 w l
EOP
