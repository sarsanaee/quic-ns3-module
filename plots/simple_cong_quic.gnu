set terminal png size 680,480
set output "quic_cong.png"
set xlabel "Sample"
set ylabel "WindowsSize(Bytes)"
plot "quic_cong.dat" using 2 title 'Congestion Window Size - QUIC' with linespoints
exit