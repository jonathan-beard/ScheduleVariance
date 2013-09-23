#!/usr/bin/env perl
use strict;
use warnings;

my $inputfile = shift( @ARGV );

open INFILE,  "<$inputfile" or die "Couldn't open $inputfile\n";
my @lines = <INFILE>;
close INFILE;

my $loadProcessCountColumn = 5;
my $muColumn               = 7;
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


foreach my $muthree ( sort keys %tickMean )
{
   print STDOUT "$muthree - $tickMean{ $muthree }\n";
}

exit( 0 );
