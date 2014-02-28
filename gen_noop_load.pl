#!/usr/bin/env perl
use strict;
use warnings;
##
# breaknoop - takes total count of noops to generate,
# returns an array with the number to generate for each
# __asm__ segment, assumes segments should be no more
# than 100,000 noops per segment
##
sub breaknoop( $ ); 
##
# gennoop - generates the  noop loops specified by
# breaknoop
##
sub gennoop( $ );
##
# gentiming - gen rdtsc timing statement, use "before" to generate
# first hook and "after" otherwise
##
sub gentiming( $ );

my $num = shift( @ARGV );
my $seconds = shift( @ARGV );
my $mean = shift( @ARGV );
my $stddev = shift( @ARGV );
my $file = "noop_loop_unrolled_load.cpp";
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
print OUTFILE     gentiming( "before" );
print OUTFILE     "\n";
my $blocks_required = breaknoop( $num );
foreach( @$blocks_required )
{
   print OUTFILE gennoop( $_ );
   print OUTFILE "\n";
}
print OUTFILE     gentiming( "after" );
print OUTFILE     "uint64_t cyclesbefore
                     = (lowBitsBefore & 0xffffffff) | (highBitsBefore << 32);\n";
print OUTFILE     "uint64_t cyclesafter
                     = (lowBitsAfter & 0xffffffff) | (highBitsAfter << 32);\n";
print OUTFILE     "int64_t diff = ( cyclesafter - cyclesbefore ) - expectedMeanCycles ;\n";                     
close OUTFILE;


sub breaknoop( $ )
{
   my $total_noops_required = shift;
   my $limit = 1e5;
   my @output;
   while( $total_noops_required > 0 )
   {
      if( $total_noops_required < $limit )
      {
         push( @output, $total_noops_required );
         $total_noops_required -= $total_noops_required;
      }
      else
      {
         push( @output, $limit );
         $total_noops_required -= $limit;
      }
   }
   return( \@output );
}

sub gentiming( $ )
{
   my $output = "";
   my $which = shift;
   if( $which eq "before" )
   {
      $output.="__asm__ volatile(\"\\\n";
      $output.="                 lfence                           \\n\\\n";
      $output.="                 rdtsc                            \\n\\\n";
      $output.="                 movq     %%rax, %[lowb]          \\n\\\n";
      $output.="                 movq     %%rdx, %[highb]         \"\n";
      $output.="                  :\n";
      $output.="                  [lowb]    \"=r\" (lowBitsBefore),\n";
      $output.="                  [highb]   \"=r\" (highBitsBefore)\n";
      $output.="                  :\n";
      $output.="                 /*no inputs*/\n";
      $output.="                  :\n";
      $output.="                  \"rax\",\"rdx\"";
      $output.="                 );\n";
   }
   elsif( $which eq "after" )
   {
      $output.="__asm__ volatile(\"\\\n";
      $output.="                 lfence                           \\n\\\n";
      $output.="                 rdtsc                            \\n\\\n";
      $output.="                 movq     %%rax, %[lowa]          \\n\\\n";
      $output.="                 movq     %%rdx, %[higha]         \"\n";
      $output.="                  :\n";
      $output.="                  [lowa]    \"=r\" (lowBitsAfter),\n";
      $output.="                  [higha]   \"=r\" (highBitsAfter)\n";
      $output.="                  :\n";
      $output.="                 /*no inputs*/\n";
      $output.="                  :\n";
      $output.="                  \"rax\",\"rdx\"";
      $output.="                 );\n";
   }
   return( $output );
}

sub gennoop( $ )
{
   my $output = "";
   my $num = shift;
   $output .= "__asm__ volatile(\"\\\n";
   for( my $i = 0; $i < $num; $i++ )
   {
      $output.="                 nop \\n\\\n";
   }
   $output .= "\"\n";
   $output .= "                  :\n";
   $output .= "                 /*no inputs*/\n";
   $output .= "                  :\n";
   $output .= "                 );\n";
   return( $output );
}

exit( 0 );
