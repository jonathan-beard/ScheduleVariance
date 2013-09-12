#!/usr/bin/env perl
use strict;
use warnings;


######################
### CONFIG PARAMS ####
######################
my $min_mu = .00000025;
my $max_mu = .0001;
my $mu_delta = .00000025;
my $min_processes = 1;
my $max_processes = 20;
my $iterations = 1000;
my $core = 1;
######################
#### END CONFIGS #####
######################


## CHECK TO SEE IF EXE IS COMPILED ##
if( ! -e "./svar" )
{
   print STDERR "svar doesn't exist, please build it\n";
   exit( -1 );
}


my $outputfile = "/myscratch/rex_noop_loop_unrolled_12Sep2013.csv";

## GEN HEADER ##
`./svar -print_header true -f $outputfile`;

################


## GEN OUTPUT ##
for ( my $mu = $min_mu; $mu <= $max_mu; $mu += $mu_delta )
{  
   for ( my $procs = $min_processes; $procs <= $max_processes; $procs+=1 )
   {
      my $cmd = "./svar -p# $procs -mu $mu -iterations $iterations -f $outputfile";
      `$cmd`;
   }
}

exit( 0 );
