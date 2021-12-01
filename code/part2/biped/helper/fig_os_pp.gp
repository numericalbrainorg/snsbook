set terminal postscript eps 12 color
set output 'os_pp.eps'
unset key

plot [5:10][-14:11] \
     'os_pp.dat' using 1:2 w l,\
     'os_pp.dat' using 1:3 w l,\
     'os_pp.dat' using 1:4 w l,\
     'os_pp.dat' using 1:5 w l,\
     'os_pp.dat' using 1:6 w l,\
     'os_pp.dat' using 1:7 w l,\
     'os_pp.dat' using 1:8 w l,\
     'os_pp.dat' using 1:9 w l,\
     'os_pp.dat' using 1:10 w l,\
     'os_pp.dat' using 1:11 w l,\
     'os_pp.dat' using 1:12 w l,\
     'os_pp.dat' using 1:13 w l