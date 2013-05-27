set multiplot layout 2,1
set title "IRQ response Latency "
set xlabel "Latency Max:51.0us,Avg:12.4us,Min:11.0us"
set ylabel "Time(us)"
set xrange [0:5000]
set xtics 500,500,5000
plot 'noload_irq_nort.log' with lines lw 2 title "RT:IRQ response"
set xlabel "Latency Max:98.0us,Avg:42.2us,Min:33.0us"
set ylabel "Time(us)"
set xrange [0:5000]
set xtics 500,500,5000
plot 'noload_irq_rt.log' with lines lw 2 title "NON_RT:IRQ response"