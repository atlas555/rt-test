set multiplot layout 2,1
set title "Latency histogram"
set xlabel "Latency Max:21.0us,Avg:7.9us,Min:0.0us"
set ylabel "Time(us)"
set xrange [0:200000]
set xtics 20000,20000,200000
plot 'rt_cyclitest.log' with lines lw 2 title "RT_Cyclitest"
set xlabel "Latency Max:1701.0us,Avg:7.2us,Min:0.0us"
set ylabel "Time(us)"
set xrange [0:200000]
set xtics 20000,20000,200000
plot 'NON_rt_cyclitest.log' with lines lw 2 title "NON_RT_Cyclitest"