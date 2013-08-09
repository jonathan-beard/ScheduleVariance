#!/usr/bin/env perl
use strict;
use warnings;


######################
### CONFIG PARAMS ####
######################
my $min_mu = .25;
my $max_mu = 10;
my $mu_delta = .25;
my $min_processes = 1;
my $max_processes = 20;
my $iterations = 1000;
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
                                 my $count = `ps -A | wc -l`;
                                 chomp( $count );
                                 return( $count );
                              }
                            },
   ProcessNumber     => sub {
                              my $line = shift;
                              if( $line eq "Header" )
                              {
                                 return( "LoadProcesses" );
                              }
                            }
);

## CHECK TO SEE IF EXE IS COMPILED ##
if( ! -e "./svar" )
{
   print STDERR "svar doesn't exist, please build it\n";
   exit( -1 );
}


## GEN HEADER ##
my $output = `./svar -print_header true`;
chomp( $output );
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
      chomp( $input );
      my @lines = split /\n/, $input;
      foreach my $line (@lines ){
        $output .= $line.",".$otherprocs.",".$procs."\n"; 
      }
   }
}

open (OUTFILE, ">./output.csv");
print OUTFILE $output;
close(OUTFILE);

exit( 0 );
