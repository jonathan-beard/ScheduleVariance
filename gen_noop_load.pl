#!/usr/bin/env perl
use strict;
use warnings;

my $num = shift( @ARGV );
my $seconds = shift( @ARGV );
my $mean = shift( @ARGV );
my $stddev = shift( @ARGV );
my $file = "/tmp/noop_loop_unrolled_load.cpp";
my $headerfile = "noop_loop_unrolled_load_seconds.hpp";

open HEADERFILE, ">$headerfile" or die "Couldn't open headerfile!!\n";
print HEADERFILE     "const double  expectedSeconds = $seconds;\n";
close HEADERFILE;

##
# Simply writes the noop assembly "unrolled loop" to the file
# which is then used by the cpp file.  This should be called
# pror to compilation otherwise bad things could happen.
##
open OUTFILE, ">$file" or die "Couldn't open load file!!\n";
print OUTFILE     "uint64_t highBitsBefore = 0x0, lowBitsBefore = 0x0;\n";
print OUTFILE     "uint64_t highBitsAfter  = 0x0, lowBitsAfter  = 0x0;\n";
print OUTFILE     "const uint64_t  expectedMeanCycles = $mean;\n";
#print OUTFILE     "const double    expectedSTDCycles  = $stddev;\n";
print OUTFILE     "uint64_t theNoopCount   = $num;\n";
print OUTFILE     "__asm__ volatile(\"\\\n";
print OUTFILE     "                 lfence                           \\n\\\n";
print OUTFILE     "                 rdtsc                            \\n\\\n";
print OUTFILE     "                 movq     %%rax, %[lowb]          \\n\\\n";
print OUTFILE     "                 movq     %%rdx, %[highb]         \\n\\\n";
for( my $i = 0; $i < $num; $i++ )
{
   print OUTFILE  "                 nop \\n\\\n";
}
print OUTFILE     "                 lfence                           \\n\\\n";
print OUTFILE     "                 rdtsc                            \\n\\\n";
print OUTFILE     "                 movq     %%rax, %[lowa]          \\n\\\n";
print OUTFILE     "                 movq     %%rdx, %[higha]         \"\n";
print OUTFILE     "                  :\n";
print OUTFILE     "                  [lowb]    \"=r\" (lowBitsBefore),\n";
print OUTFILE     "                  [highb]   \"=r\" (highBitsBefore),\n";
print OUTFILE     "                  [lowa]    \"=r\" (lowBitsAfter),\n";
print OUTFILE     "                  [higha]   \"=r\" (highBitsAfter)\n";
print OUTFILE     "                  :\n";
print OUTFILE     "                 /*no inputs*/\n";
print OUTFILE     "                  :\n";
print OUTFILE     "                  \"rax\",\"rdx\"";
print OUTFILE     "                 );\n";
print OUTFILE     "uint64_t cyclesbefore
                     = (lowBitsBefore & 0xffffffff) | (highBitsBefore << 32);\n";
print OUTFILE     "uint64_t cyclesafter
                     = (lowBitsAfter & 0xffffffff) | (highBitsAfter << 32);\n";
print OUTFILE     "int64_t diff = ( cyclesafter - cyclesbefore ) - expectedMeanCycles ;\n";                     
close OUTFILE;
`ln -s /tmp/noop_loop_unrolled_load.cpp ./noop_loop_unrolled_load.cpp`;

exit( 0 );
