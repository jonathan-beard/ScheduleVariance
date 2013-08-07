ScheduleVariance
================

Simple program to benchmark a hardware / OS combination.  The initial
purpose for construction was to see how close a user could actually
set a running time for a micro-benchmark.  We've found it varies 
widely by scheduler implementation, processor and the overall environment.

Using the procstat framework we can also log details about interrupts
and other system events that could possibly have an effect on the 
results we're seeing.  

Things to do:
Finish threading implementation Aug 7, 2012
Finish Procstat integration "
