ScheduleVariance
================

Simple program to benchmark a hardware / OS combination.  The initial
purpose for construction was to see how close a user could actually
set a running time for a micro-benchmark.

Using the procstat framework we can also log details about interrupts
and other system events that could possibly have an effect on the 
results we're seeing.  

How to compile:

1) get repository

2) ensure you have a c++11 compatible compiler

3) clone procstat (git@github.com:jonathan-beard/procstat.git) into the ScheduleVariance dir

4) cd procstat; make procstatlib; cd ..

4a) run ./gen_calibrate_load.pl 100 -1 in order to get a starter noop load.

4b) cd Calibrate; ./gen_calibrate_load.pl; make lib; cd ..

5) make 

6) check out options; ./svar -h

7) run

Optionally: 
Design a new load by extending load.hpp base class.

Things to do:
change noop unrolled load to hand assembly instead of using gcc
