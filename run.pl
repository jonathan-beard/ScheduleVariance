#!/usr/bin/env perl
use strict;
use warnings;


######################
### CONFIG PARAMS ####
######################
my $min_mu = .000050;
my $max_mu = 1;
my $mu_delta = .0000050;
my $min_processes = 1;
my $max_processes = 20;
my $iterations = 1000;
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

# else assume arg is for the filename 
##
my $outputfile = $arg;

## CHECK TO SEE IF EXE IS COMPILED ##
if( ! -e "./svar" )
{
   print STDERR "svar doesn't exist, please build it\n";
   exit( -1 );
}



## GEN HEADER ##
#`./svar -print_header true -f $outputfile`;

################


## GEN OUTPUT ##
for ( my $mu = $min_mu; $mu <= $max_mu; $mu += $mu_delta )
{  
   for ( my $procs = $min_processes; $procs <= ($max_processes - 4 ); $procs+=4 )
   {
      my $cmd = "./svar -cluster true -p# $procs -mu $mu -iterations $iterations -f $outputfile";
      system($cmd);
      $cmd = "./svar -cluster true -p# ".($procs+1)." -mu $mu -iterations $iterations -f $outputfile";
      system($cmd);
      $cmd = "./svar -cluster true -p# ".($procs+2)." -mu $mu -iterations $iterations -f $outputfile";
      system($cmd);
      $cmd = "./svar -cluster true -p# ".($procs+3)." -mu $mu -iterations $iterations -f $outputfile";
      `$cmd`;
   }
}

exit( 0 );
