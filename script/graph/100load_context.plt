set multiplot layout 2,1
set title "%100LOAD:Context_switch Latency "
set xlabel "Latency Max:0.8us,Avg:0.8us,Min:9.9us"
set ylabel "Time(us)"
set xrange [0:150000]
set xtics 20000,20000,150000
plot '100load_rt_context.log' with lines lw 2 title "RT-Context_switch"
set xlabel "Latency Max:2612.4us,Avg:3.5us,Min:2.0us"
set ylabel "Time(us)"
set xrange [0:150000]
set xtics 20000,20000,150000
plot '100load_nort_context.log' with lines lw 2 title "NON_RT-Context_switch"