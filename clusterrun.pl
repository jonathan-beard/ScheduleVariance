#!/usr/bin/env perl
use strict;
use warnings;


######################
### CONFIG PARAMS ####
######################
my $min_mu = .0005;
my $max_mu = 1;
my $mu_delta = .00005;
my $min_processes = 1;
my $max_processes = 20;
my $iterations = 1000;
my $core = 1;
######################
#### END CONFIGS #####
######################

my $length = @ARGV;

my $filename = shift( @ARGV );
my $cluster_user = shift( @ARGV );

## CHECK TO SEE IF EXE IS COMPILED ##
if( ! -e "./svar" )
{
   print STDERR "svar doesn't exist, please build it\n";
   exit( -1 );
}


my $outputfile = $filename;

## GEN HEADER ##
`./svar -print_header true -f $outputfile`;
################


## GEN OUTPUT ##
for ( my $mu = $min_mu; $mu <= $max_mu; $mu += $mu_delta )
{  
   for ( my $procs = $min_processes; $procs <= $max_processes; $procs+=1 )
   {
      my $clusterOutputFile = 
      "\$!/bin/sh\n" +
      "#\$ -cwd\n" +
      "./svar -p\# $procs -mu $mu -iterations $iterations -f $outputfile\n";
      open( QSUBOUT, ">", "svarjob.sh" );
      print QSUBOUT, $clusterOutputFile;
      close( QSUBOUT );
      `qsub -q all.q svarjob.sh`;
      sleep( 2 );
      while( `qstat -u beardj | wc -l` > 20 )
      {
         sleep( 10 );
      }
   }
}

exit( 0 );
