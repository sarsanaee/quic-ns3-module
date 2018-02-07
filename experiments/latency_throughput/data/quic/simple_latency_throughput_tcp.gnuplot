#!/usr/bin/gnuplot

#set terminal postscript eps enhanced color "Helvetica" 20
#set multiplot
set autoscale                        # scale axes automatically
set xtic auto                        # set xtics automatically
set ytic auto                        # set ytics automatically
set terminal png size 680,480
set output "lt_quic.png"
set xlabel "Throughput (Mbps)"
set ylabel "Latency (ns)"
plot "second_flow_1Mbps.dat" using 2:1 title 'Latency-Throughput -  1Mbps - QUIC' with linespoints, \
     "second_flow_10Mbps.dat" using 2:1 title 'Latency-Throughput - 10Mbps - QUIC' with linespoints, \
     "second_flow_100Mbps.dat" using 2:1 title 'Latency-Throughput - 100Mbps - QUIC' with linespoints, \
     "second_flow_1000Mbps.dat" using 2:1 title 'Latency-Throughput - 1000Mbps - QUIC' with linespoints

exit
