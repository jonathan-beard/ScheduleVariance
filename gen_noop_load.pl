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
print OUTFILE     "__asm__ volatile(\"\\";
for( my $i = 0; $i < $num; $i++ )
{
   print OUTFILE  "                 nop\n";
}
print OUTFILE     "                \":\n";
print OUTFILE     "                  :\n";
print OUTFILE     "                 ;";
close OUTFILE;
exit( 0 );
