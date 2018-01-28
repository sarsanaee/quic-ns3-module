set terminal png size 680,480
set output "lt_tcp.png"
set xlabel "Throughput (Mbps)"
set ylabel "Latency (ns)"
plot "second_flow.dat" using 2:1 title 'Latency-Throughput - TCP' with linespoints
exit
