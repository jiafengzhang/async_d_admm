# async_d_admm
## make sure your system is Linux and MPICH is installed
		for ubuntu you can install MPICH by:
		sudo apt-get install mpich
## make and run
### change directory path
		cd src
### make the code
		make
### edit the configure file
		cd ../conf			(suppose your path at src directory)
		vim admm.conf
####congfigure
			s is the partial barrier
			delta is the bounded delay
			dgree is the neighbors num. generally, dgree is not large than the agnet num. and s <= dgree
			when adpstep is set as true, the program run with adptive stepsize
			and stepsize is the w in the paper in 2.3 chapter 
### run
		mpirun -n 4 ./admm
		in this case, s is set to 2, delat is set to 10, adpstep is set to true, stepsize is set as 0.5
		and the program is run in a single machine, you can run it on multi-agent with the hostfile.



