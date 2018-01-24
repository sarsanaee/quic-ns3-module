set terminal png size 680,480
set output "cwnd.png"
set xlabel "Sample"
set ylabel "WindowsSize(Bytes)"
plot "cwnd.dat" using 2 title 'Congestion Window Size - TCP' with linespoints
exit
