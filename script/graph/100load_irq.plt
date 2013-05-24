set multiplot layout 2,1
set title "IRQ response Latency "
set xlabel "Latency Max:90.0us,Avg:12.9us,Min:6.0us"
set ylabel "Time(us)"
set xrange [0:5000]
set xtics 500,500,5000
plot '100load_irq_nort.log' with lines lw 2 title "RT:IRQ response"
set xlabel "Latency Max:100.0us,Avg:38us,Min:11.0us"
set ylabel "Time(us)"
set xrange [0:5000]
set xtics 500,500,5000
plot '100load_irq_rt.log' with lines lw 2 title "NON_RT:IRQ response"