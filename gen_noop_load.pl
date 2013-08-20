#!/usr/bin/env perl
use strict;
use warnings;

my $num = shift(@ARGV);
my $file = "noop_loop_unrolled_load.cpp";

##
# Simply writes the noop assembly "unrolled loop" to the file
# which is then used by the cpp file.  This should be called
# pror to compilation otherwise bad things could happen.
##
open OUTFILE, ">$file" or die "Couldn't open load file!!\n";
print OUTFILE     "uint64_t highBitsBefore = 0x0, lowBitsBefore = 0x0;\n";
print OUTFILE     "uint64_t highBitsAfter = 0x0, lowBitsAfter = 0x0;\n";
print OUTFILE     "uint64_t theNoopCount = $num;\n";
print OUTFILE     "__asm__ volatile(\"\\";
print OUTFILE     "lfence                            \n\\";
print OUTFILE     "rdtsc                             \n\\";
print OUTFILE     "movq     %%rax, %[lowb]           \n\\";
print OUTFILE     "movq     %%rdx, %[highb]          \n\\";
for( my $i = 0; $i < $num; $i++ )
{
   print OUTFILE  "                 nop\n";
}
print OUTFILE     "lfence                            \n\\";
print OUTFILE     "rdtsc                             \n\\";
print OUTFILE     "movq     %%rax, %[lowa]           \n\\";
print OUTFILE     "movq     %%rdx, %[higha]          \n\\";
print OUTFILE     "                \":\n";
print OUTFILE     "      [lowb]    \"=r\" (lowBitsBefore),\n";
print OUTFILE     "      [highb]   \"=r\" (highBitsBefore),\n";
print OUTFILE     "      [lowa]    \"=r\" (lowBitsAfter),\n";
print OUTFILE     "      [higha]   \"=r\" (highBitsAfter)\n";
print OUTFILE     "                  :\n";
print OUTFILE     "\"rax\",\"rdx\"";
print OUTFILE     "                  ;";
print OUTFILE     "uint64_t cyclesbefore
                     = (lowBitsBefore & 0xffffffff) | (highBitsBefore << 32);\n";
print OUTFILE     "uint64_t cyclesafter
                     = (lowBitsAfter & 0xffffffff) | (highBitsAfter << 32);\n";
print OUTFILE     "uint64_t diff = cyclesbefore - cyclesafter;\n";                     
close OUTFILE;
exit( 0 );
