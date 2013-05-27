set title "IRQ response Latency "
set xlabel "NO LOAD Max:51.0us,Avg:12.4us,Min:11.0us  \
            100 LOAD Max:90.0us,Avg:12.9us,Min:6.0us"
set ylabel "Time(us)"
set xrange [0:5000]
set xtics 500,500,5000
plot 'noload_irq_nort.log' with points pt 8 title "NO LOAD",'100load_irq_nort.log' with points pt 4 title "%100LOAD"