#!/usr/bin/env perl
use strict;
use warnings;

my $file = shift( @ARGV );

##COLUMNS
my %columns = ( 3 => "VoluntaryContextSwaps",
                4 => "NonVoluntaryContextSwaps",
                5 => "LoadPorcesses",
                8 => "ServiceTime",
                13 => "Delta" );

my $defaultFunction = sub {
                              my $fields = shift;
                              my $val    = shift;
                              return( $val );
                          };

my %columnFunctions = ( 3 => $defaultFunction,
                        4 => $defaultFunction,
                        5 => $defaultFunction,
                        8 => $defaultFunction,
                        13 => sub {
                                     my $fields = shift;
                                     my $delta  = shift;
                                     my $frequency = @$fields[ 9 ];
                                     my $adjustedDelta = $delta / $frequency;
                                     return( $adjustedDelta );
                                  }
                      );
                
open INFILE,  "<$file" or die "Couldn't open $file";
my @lines = <INFILE>;
close( INFILE );

shift( @lines );

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
   foreach my $column ( sort keys %columns )
   {
      $outputline .= &{ $columnFunctions{ $column } }( \@field, $field[ $column ] ).",";
   }
   $outputline =~ s/,$//;
   print STDOUT "$outputline\n";
}

exit( 0 );
