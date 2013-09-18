#!/usr/bin/env perl
use strict;
use warnings;

my $file = shift( @ARGV );

open INFILE,  "<$file" or die "Couldn't open $file";
my @lines = <INFILE>;
close( INFILE );


my @capturefields = ( 12 );

my $boolfield = 14;

shift( @lines );

foreach my $process ( 1 .. 20 )
{
   my $outfile = $file;
   $outfile =~ s/\..+$/_ServiceRate_$process\.csv/;
   open OUTFILE, ">$outfile" or die "Couldn't open $outfile";
   foreach my $line (@lines)
   {
      chomp( $line );
      my @field = split/,/, $line; 
      if( $field[ $boolfield ] == $process )
      {
         my $outline = "";
         foreach my $index( @capturefields )
         {
            $outline .= $field[ $index ].",";
         }
         $outline =~ s/,$//;
         print OUTFILE $outline."\n";
      }
   }
   close( OUTFILE );
}
#my OUTFILE, ">

exit( 0 );
