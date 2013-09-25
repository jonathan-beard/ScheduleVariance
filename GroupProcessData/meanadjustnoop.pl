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




#get rid of header
my $headerline = shift( @lines );
chomp( $headerline );
$headerline =~ s/\\n$//;
my @tempfields = split /,/, $headerline;

my %fieldIndex = (
"TimeStamp" => 0, 
"Iteration" => 0,
"ProcessID" => 0,
"VoluntaryContextSwaps" => 0,
"Non-VoluntaryContextswaps" => 0,
"LoadProcesses" => 0,
"Load" => 0,
"Distribution" => 0,
"ServiceTime" => 0,
"Frequency" => 0,
"TicksToSpin" => 0,
"TargetStopTick" => 0,
"ActualStopTick" => 0,
"TickDelta" => 0 );

my $index = 0;
foreach( @tempfields )
{
   $fieldIndex{ $_ } = $index++;
}


my %tickCount;
my %tickAccum;
my %tickMean;


foreach my $line ( @lines )
{
   chomp( $line );
   $line =~ s/\\n$//;
   my @field = split /,/, $line;
   my $startTick = $field[ $fieldIndex{ "TargetStopTick" } ] - 
                     $field[ $fieldIndex{ "TicksToSpin" } ];
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
   
   my @field = @$fields;

   my $startTick = $field[ $fieldIndex{ "TargetStopTick" } ] - 
                     $field[ $fieldIndex{ "TicksToSpin" } ];
   
   my $endTick   = $field[ $fieldIndex{ "ActualStopTick" } ];
   my $totalTicks = $endTick - $startTick;
   
   my $correctedTicks = $totalTicks - 
                  $tickMean{ @$fields[ $fieldIndex{ "ServiceTime" } ] };
   my $correctedSeconds = 
      $correctedTicks / @$fields[ $fieldIndex{ "Frequency" }  ];
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
