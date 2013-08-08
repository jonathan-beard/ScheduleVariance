#!/usr/bin/env perl
use strict;
use warnings;


######################
### CONFIG PARAMS ####
######################
my $min_mu = .25;
my $max_mu = .5;
my $mu_delta = .25;
my $min_processes = 1;
my $max_processes = 2;
my $iterations = 10;
my $core = 1;
######################
#### END CONFIGS #####
######################

## Hacky named functions ##
my %statsfunctions = (
   OtherProcessCount => sub {
                              my $line = shift;
                              if( $line eq "Header" )
                              {
                                 return( "SystemProcesses" );
                              }
                              elsif( $line eq "Values" )
                              {
                                 return( `ps -A | wc -l` );
                              }
                            }
   ProcessNumber     => sub {
                              my $line = shift;
                              if( $line eq "Header" )
                              {
                                 return( "TotalProcesses" );
                              }
                            }
);


## GEN HEADER ##
my $output = `./svar -print_header true`;
for (keys %statsfunctions )
{
   $output .= ",";
   $output .= &{ $statsfunctions{ $_ } }( "Header" );
}
$output .= "\n";

################

## GEN OUTPUT ##
for ( my $mu = $min_mu; $mu <= $max_mu; $mu += $mu_delta )
{  
   for ( my $procs = $min_processes; $procs <= $max_processes; $procs+=1 )
   {
      my $cmd = "./svar -p# $procs -mu $mu -iterations $iterations";
      my $otherprocs = &{ $statsfunctions{ OtherProcessCount } }("Values");
      my $input = `$cmd`;
      my @lines = split /\n/, $input;
      foreach my $line (@lines ){
        $output .= $line.",".$otherprocs.",".$procs."\n"; 
      }
   }
}

open OUTFILE, "<output.txt" or die "Couldn't open outfile!!";
print OUTFILE $output;
close OUTFILE;

exit( 0 );
