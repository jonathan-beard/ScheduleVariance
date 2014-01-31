#!/usr/bin/env perl
use strict;
use warnings;


######################
### CONFIG PARAMS ####
######################
my $min_mu = .00000034;
my $max_mu = 1;
my $mu_delta = .0000050;
my $min_processes = 1;
my $max_processes = 20;
my $iterations = 10;
my $core = 1;
######################
#### END CONFIGS #####
######################

my $length = @ARGV;
my $arg = shift( @ARGV );

if( $length == 0 )
{
   print "At least one argument expected, either \"setup\" or output filename.\n";
   exit( 0 );
}

if( $arg eq "setup" )
{
   `cp noop_loop_unrolled_load_template.cpp /tmp/noop_loop_unrolled_load.cpp`;
   exit( 0 );
}

##
# else assume arg is for the filename 
##
my $filename = $arg;

## CHECK TO SEE IF EXE IS COMPILED ##
if( ! -e "./svar" )
{
   print STDERR "svar doesn't exist, please build it\n";
   exit( -1 );
}


my $outputfile = "/tmp/$filename";

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
