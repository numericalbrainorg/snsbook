set terminal postscript eps 12 color
set output 'stick.eps'
unset key
set ytics 100
set size 2, 0.5
plot [0:10][-0.1:1.2] 'stick100.dat' t '' w l, 'stick500.dat' t '' w l
