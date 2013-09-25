#!/usr/bin/env perl
use strict;
use warnings;

my $inputfile = shift( @ARGV );

open INFILE,  "<$inputfile" or die "Couldn't open $inputfile\n";
my @lines = <INFILE>;
close INFILE;

my @outputColumns = ( "LoadProcesses", 
                      "VoluntaryContextSwaps",
                      "Non-VoluntaryContextswaps",
                      "Load",
                      "TickDelta",
                      "ServiceTime" );




my %fieldIndex = (
0     => "TimeStamp", 
1     => "Iteration",
2     => "ProcessID",
3     => "VoluntaryContextSwaps",
4     => "Non-VoluntaryContextswaps",
5     => "LoadProcesses",
6     => "Load",
7     => "Distribution",
8     => "ServiceTime",
9     => "Frequency",
10    => "TicksToSpin",
11    => "TargetStopTick",
12    => "ActualStopTick",
13    => "TickDelta" );

#didn't feel like re-writing so:
%fieldIndex = reverse %fieldIndex;



my %tickCount;
my %tickAccum;
my %tickMean;

#get rid of header
shift @lines;

foreach my $line ( @lines )
{
   chomp( $line );
   $line =~ s/\\n$//;
   my @field = split /,/, $line;
   my $startTick = $field[ $fieldIndex{ "TargetStopTick" } ] - $field[ $fieldIndex{ "TicksToSpin" } ];
   my $endTick   = $field[ $fieldIndex{ "ActualStopTick" } ];
   my $totalTicks = $endTick - $startTick;
   my $mu = $field[ $fieldIndex{ "ServiceTime" } ];
   my $process = $field[ $fieldIndex{ "LoadProcesses" } ];
   if( $process == 1 )
   {
      $tickCount{ $mu }++;
      $tickAccum{ $mu } += $totalTicks;
   }
}

foreach my $mutwo ( keys %tickCount )
{
   $tickMean{ $mutwo } = $tickAccum{ $mutwo } / $tickCount{ $mutwo };
}

#test
#foreach my $mu ( keys %tickMean )
#{
#   print "$mu - Mean: $tickMean{ $mu }\n";
#}

my $defaultfunc = sub{ 
                        my $fields = shift;
                        my $val = shift;
                        return( $val );
                     };

my %columnFunction;
foreach my $key ( keys %fieldIndex )
{
   $columnFunction{ $key } = $defaultfunc;
}
#custom functions here
$columnFunction{ "TickDelta" } = sub {
   my $fields = shift;
   my $val    = shift;
   my $correctedTicks = $val - $tickMean{ @$fields[ $fieldIndex{ "ServiceTime" } ] };
   my $correctedSeconds = $correctedTicks / @$fields[ $fieldIndex{ "Frequency" }  ];
   return( $correctedSeconds );
   };   

#heading
my $head = "";
foreach my $heading ( @outputColumns )
{
   $head .= "$heading,";
}
$head =~ s/,$//g;

print STDOUT "$head\n";

foreach my $line ( @lines )
{
   chomp( $line );
   $line =~ s/\\n//;
   my @field = split /,/, $line;
   my $outputline = "";
   foreach my $column (@outputColumns)
   {
      $outputline .= &{ $columnFunction{ $column } }( \@field, $field[ $fieldIndex{ $column } ] ).",";
   }
   $outputline =~ s/,$//;
   print STDOUT "$outputline\n";
}

exit( 0 );
