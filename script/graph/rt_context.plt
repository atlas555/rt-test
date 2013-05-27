set title "RT-Context_switch Latency "
set xlabel "NOLOAD Max:8.9us,Avg:0.8us,Min:0.8us \
            100%LOAD Max:9.9us,Avg:0.8us,Min:0.8us"
set ylabel "Time(us)"
set xrange [0:150000]
set xtics 20000,20000,150000
plot 'noload_rt_context.log' with points pt 8 title "NO LOAD",'100load_rt_context.log' with points pt 4 title "%100LOAD"