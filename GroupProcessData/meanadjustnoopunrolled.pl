#!/usr/bin/env perl
use strict;
use warnings;

my $inputfile = shift( @ARGV );

open INFILE,  "<$inputfile" or die "Couldn't open $inputfile\n";
my @lines = <INFILE>;
close INFILE;

my $loadProcessCountColumn = 5;
my $muColumn               = 7;
my $frequencyColumn        = 8;
my $deltaColumn            = 12;

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
   my $process = $field[ $loadProcessCountColumn ];
   my $mu      = $field[ $muColumn               ];
   my $delta   = $field[ $deltaColumn            ];
   if( $process == 1 )
   {
      $tickCount{ $mu }++;
      $tickAccum{ $mu } += $delta;
   }
}

foreach my $mutwo ( keys %tickCount )
{
   $tickMean{ $mutwo } = $tickAccum{ $mutwo } / $tickCount{ $mutwo };
}

#add columns and order to output
my %columns = ( 3 => "VoluntaryContextSwaps",
                4 => "NonVoluntaryContextSwaps",
                5 => "LoadProcesses",
                7 => "ServiceTime",
                12 => "Delta" );

my $defaultfunc = sub{ 
                        my $fields = shift;
                        my $val = shift;
                        return( $val );
                     };

my %columnfunctions = ( 3 => $defaultfunc,
                        4 => $defaultfunc,
                        5 => $defaultfunc,
                        7 => $defaultfunc,
                        12 => sub {
                           my $fields = shift;
                           my $val    = shift;
                           my $correctedTicks = $val - $tickMean{ @$fields[ $muColumn ] };
                           my $correctedSeconds = $correctedTicks / @$fields[ $frequencyColumn ];
                           return( $correctedSeconds );
                        } );
            



#go back through each line changing delta to be the difference from the mean
my $head = "";
foreach my $heading ( sort keys %columns )
{
   $head .= "$columns{ $heading },";
}
$head =~ s/,$//g;

print STDOUT "$head\n";

foreach my $line ( @lines )
{
   chomp( $line );
   $line =~ s/\\n//;
   my @field = split /,/, $line;
   my $outputline = "";
   foreach my $column (sort keys %columns)
   {
      $outputline .= &{ $columnfunctions{ $column } }( \@field, $field[ $column ] ).",";
   }
   $outputline =~ s/,$//;
   print STDOUT "$outputline\n";
}

exit( 0 );
