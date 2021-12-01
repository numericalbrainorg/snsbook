#!/usr/bin/ruby

Dirname = "animate"

Dir.mkdir(Dirname) unless Dir.exist?(Dirname)
f = open("./stick.dat", "r")
n = 0
until f.eof?
	x0, y0 = f.gets.chomp.split
	x1, y1 = f.gets.chomp.split
	x2, y2 = f.gets.chomp.split
	f.gets.chomp.split # skip
	x3, y3 = f.gets.chomp.split
	x4, y4 = f.gets.chomp.split
	x5, y5 = f.gets.chomp.split
	f.gets.chomp.split # skip
	ofn = sprintf("#{Dirname}/%05d", n)
	open("#{ofn}.dat", "w"){|o|
		o.puts "#{x0} #{y0}"
		o.puts "#{x1} #{y1}"
		o.puts "#{x2} #{y2}"
		o.puts "\n"
		o.puts "#{x3} #{y3}"
		o.puts "#{x4} #{y4}"
		o.puts "#{x5} #{y5}"
	}
	IO.popen("gnuplot", "w"){|gp|
		gp.puts "set terminal postscript eps 12 color"
		gp.puts "set output '#{ofn}.eps'"
		gp.puts "set ytics 10"
		gp.puts "set size 2, 0.5"
		gp.puts "plot [-1:30][-.1:1.2] '#{ofn}.dat' t '' w l lw 10"
	}
	n += 1
end
