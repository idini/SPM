 #!/bin/bash
printf $(clear)
printf "\n--------------------------------------------------\n"
printf "        	ReduceVideo                       		\n"
printf "   Implemented with FastFlow and Threads C++11      \n"
printf "        	Maurizio Idini            				\n"
printf "\n--------------------------------------------------\n\n"
#set gnuplot env variables
PATH=$PWD/bin:$PWD/usr/bin:$PATH
echo "Start test."
echo ""
echo "Check compiler version...."
compiler=$(gcc -dumpversion);
if(("${compiler:0:1}" >= 4))
        then
        if(("${compiler:2:1}" >= 8))
        then
		printf "Ok, your version is "
		printf "$compiler \n\n"

		echo "Compile the software...."
		make -s

		echo "Ok. Start test time on machine."
		printf "Please wait a while.....\n\n"

		echo "Compute Sequential"
		timeSequential=$(./Sequential video/WarIsOver.avi 2 videoOutput/Seq.avi)
		printf "0" >> tmp/ff_tc.dat
		printf " " >> tmp/ff_tc.dat
		printf "$timeSequential" >> tmp/ff_tc.dat
		printf "\n" >> tmp/ff_tc.dat

		printf "0" >> tmp/th_tc.dat
		printf " " >> tmp/th_tc.dat
		printf "$timeSequential" >> tmp/th_tc.dat
		printf "\n" >> tmp/th_tc.dat

		#compute FastFlow
		printf "Compute FastFlow with numthreads "
		for i in {1..24}
		do
			printf "$i "
			out=$(./ReduceVideo_FastFlow video/WarIsOver.avi 2 videoOutput/FF_$i.avi $i)
			IFS=' ' read -r -a array <<< "$out"
			# time_emitter="${array[0]}" #SUPERFLUO
			time_worker=${array[1]}
			frames_per_worker=${array[2]}
			time_collector=${array[3]}
			tparn=${array[4]}
			printf "$i $time_worker $time_collector \n" >> tmp/ff_evidence_time.dat
			printf "$i $frames_per_worker \n" >> tmp/ff_evidence_frame.dat
			speedup=$(bc -l <<< "$timeSequential/$tparn")
			if [ "$i" -eq 1 ]
				then
				tpar1=$tparn
			fi
			scalab=$(bc -l <<< "$tpar1/$tparn")
			tideal=$(bc -l <<< "$timeSequential/$i")
			effic=$(bc -l <<< "$tideal/$tparn")
			printf "$i $tparn\n" 	>> tmp/ff_tc.dat
			printf "$i $speedup\n" 	>> tmp/ff_sp.dat
			printf "$i $scalab\n" 	>> tmp/ff_sc.dat
			printf "$i $effic\n" 	>> tmp/ff_ef.dat
		done

		printf "\n"

		echo "Create plots...."
		gnuplot -persist <<-EOFMarker 
		set xlabel 'n'
		set xrange [0:24]
		set xtics 0,1,24
		set tics font 'Courier,10,oblique'
		set ylabel 'Time (s)'
		set style line 1 lt 1 lw 3 pt 3 linecolor rgb 'red'
		set terminal pngcairo
		set output 'plot/ff_evidence_time.png'
		plot 'tmp/ff_evidence_time.dat' using 2 w lines lt rgb 'blue' title 'Time Worker', \
		 'tmp/ff_evidence_time.dat' using 3 w lines lt rgb 'purple' title 'Time Collector'

		set xlabel 'n'
		set xrange [0:24]
		set xtics 0,1,24
		set tics font 'Courier,10,oblique'
		set ylabel '# Frame'
		set style line 1 lt 1 lw 3 pt 3 linecolor rgb 'red'
		set terminal pngcairo
		set output 'plot/ff_evidence_frame.png'
		plot 'tmp/ff_evidence_frame.dat' using 1:2 w lines lt rgb 'red' title 'Frame Per Worker', \
		
		set xlabel 'n'
		set xrange [0:24]
		set xtics 0,1,24
		set tics font 'Courier,10,oblique'
		set ylabel 'Time (s)'
		set style line 1 lt 1 lw 3 pt 3 linecolor rgb 'red'
		set terminal pngcairo
		set output 'plot/ff_tc.png'
		plot 'tmp/ff_tc.dat' using 1:2 w lines lt rgb 'blue' title 'Completion Time'

		set xlabel 'n'
		set xrange [0:24]
		set xtics 0,1,24
		set tics font 'Courier,10,oblique'
		set ylabel 'Time (s)'
		set style line 1 lt 1 lw 3 pt 3 linecolor rgb 'red'
		set terminal pngcairo
		set output 'plot/ff_sp.png'
		plot 'tmp/ff_sp.dat' using 1:2 w lines lt rgb 'blue' title 'SpeedUp'

		set xlabel 'n'
		set xrange [0:24]
		set xtics 0,1,24
		set tics font 'Courier,10,oblique'
		set ylabel 'Time (s)'
		set style line 1 lt 1 lw 3 pt 3 linecolor rgb 'red'
		set terminal pngcairo
		set output 'plot/ff_sc.png'
		plot 'tmp/ff_sc.dat' using 1:2 w lines lt rgb 'blue' title 'Scalability'

		set xlabel 'n'
		set xrange [0:24]
		set xtics 0,1,24
		set tics font 'Courier,10,oblique'
		set ylabel 'Time (s)'
		set style line 1 lt 1 lw 3 pt 3 linecolor rgb 'red'
		set terminal pngcairo
		set output 'plot/ff_ef.png'
		plot 'tmp/ff_ef.dat' using 1:2 w lines lt rgb 'blue' title 'Efficenty'
		EOFMarker

		echo "Done."

		printf "compute Thread C++11 with numthreads "
		for i in {1..24}
		do
			printf "$i "
			tparn=$(./ReduceVideo_Threads video/WarIsOver.avi 2 videoOutput/Th_$i.avi $i)
			speedup=$(bc -l <<< "$timeSequential/$tparn")
			if [ "$i" -eq 1 ]
				then
				tpar1=$tparn
			fi
			scalab=$(bc -l <<< "$tpar1/$tparn")
			tideal=$(bc -l <<< "$timeSequential/$i")
			effic=$(bc -l <<< "$tideal/$tparn")
			printf "$i $tparn\n"	>> tmp/th_tc.dat
			printf "$i $speedup\n" 	>> tmp/th_sp.dat
			printf "$i $scalab\n" 	>> tmp/th_sc.dat
			printf "$i $effic\n" 	>> tmp/th_ef.dat
		done

		printf "\n"
		echo "Create plots...."

		gnuplot -persist <<-EOFMarker
		set xlabel 'n'
		set xrange [0:24]
		set xtics 0,1,24
		set tics font 'Courier,10,oblique'
		set ylabel ''
		set style line 1 lt 1 lw 3 pt 3 linecolor rgb 'red'
		set terminal pngcairo
		set output 'plot/th_tc.png'
		plot 'tmp/th_tc.dat' using 1:2 w lines lt rgb 'blue' title 'Completion Time'

		set xlabel 'n'
		set xrange [0:24]
		set xtics 0,1,24
		set tics font 'Courier,10,oblique'
		set ylabel 'Time (s)'
		set style line 1 lt 1 lw 3 pt 3 linecolor rgb 'red'
		set terminal pngcairo
		set output 'plot/th_sp.png'
		plot 'tmp/th_sp.dat' using 1:2 w lines lt rgb 'blue' title 'SpeedUp'
	
		set xlabel 'n'
		set xrange [0:24]
		set xtics 0,1,24
		set tics font 'Courier,10,oblique'
		set ylabel 'Time (s)'
		set style line 1 lt 1 lw 3 pt 3 linecolor rgb 'red'
		set terminal pngcairo
		set output 'plot/th_sc.png'
		plot 'tmp/th_sc.dat' using 1:2 w lines lt rgb 'blue' title 'Scalability'
	
		set xlabel 'n'
		set xrange [0:24]
		set xtics 0,1,24
		set tics font 'Courier,10,oblique'
		set ylabel 'Time (s)'
		set style line 1 lt 1 lw 3 pt 3 linecolor rgb 'red'
		set terminal pngcairo
		set output 'plot/th_ef.png'
		plot 'tmp/th_ef.dat' using 1:2 w lines lt rgb 'blue' title 'Efficenty'
		EOFMarker


		echo "Done."
	
else 
	echo "Error. Required compiler version greater or equal to 4.8"
	echo "Try to update your compiler in order to compile and execute this software."
fi
fi

exit