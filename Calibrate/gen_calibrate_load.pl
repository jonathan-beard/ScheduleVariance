#!/usr/bin/env perl
use strict;
use warnings;


sub clean($$); # args, then hash
sub gen($);
sub makefile($);
sub includelocal($$);
sub cinclude($);
sub cppinclude($);
sub genfunc($$);
sub modifycpp($);

##
# NOTE: Be aware that the changes to list will automatically be 
# updated in calibrate.cpp, therefore calibrate.cpp should be 
# recompiled if you add extra calibration loads to %list
##

my @externalcppfiles = qw( calibrate formula linearformula profilename 
                           getexpectation );
my @externalcfiles   = qw( queryCPUID );

my %list = ('ten.cpp'            => 10,
            'twenty.cpp'         => 20,
            'thirty.cpp'         => 30,
            'forty.cpp'          => 40,
            'fifty.cpp'          => 50,
            'sixty.cpp'          => 60,
            'seventy.cpp'        => 70,
            'eighty.cpp'         => 80,
            'ninety.cpp'         => 90,
            'hundred.cpp'        => 100,
            'twohundred.cpp'     => 200,
            'threehundred.cpp'   => 300,
            'fourhundred.cpp'    => 400, 
            'fivehundred.cpp'    => 500,
            'thousand.cpp'       => 1000,
            'twothousand.cpp'    => 2000,
            'threethousand.cpp'  => 3000,
            'fourthousand.cpp'   => 4000,
            'fivethousand.cpp'   => 5000,
            'tenthousand.cpp'    => 10000,
            'twentythousand.cpp' => 20000,
            'onehundredthousand.cpp' => 100000,
            'twohundredthousand.cpp' => 200000,
            'threehundredthousand.cpp' => 300000,
            'fourhundredthousand.cpp' =>  400000,
            'fivehundredthousand.cpp' =>  500000
            );
##
# clean - call to see if we need to clean up 
##
clean( \@ARGV, \%list );
##
# clean will exit if called
##
gen( \%list );
makefile( \%list );
modifycpp( \%list );
exit( 0 );

sub gen( $ )
{
   my $filelist = shift( @_ );
   foreach my $file ( keys %$filelist )
   {
      my $num = $filelist->{ $file };
      open OUTFILE,     ">$file" or die "Couldn't open load file!!\n";
      print OUTFILE     cppinclude( "cstdint" );
      print OUTFILE     includelocal( "system_query", "h" );
      print OUTFILE     includelocal( "sample", "hpp" );
      print OUTFILE     "\n";
      $file =~s/\.[^.]+$//;
      print OUTFILE     genfunc( $file , 0 );
      print OUTFILE     "{\n";
      print OUTFILE     "  uint64_t highBitsBefore = 0x0, lowBitsBefore = 0x0;\n";
      print OUTFILE     "  uint64_t highBitsAfter  = 0x0, lowBitsAfter  = 0x0;\n";
      print OUTFILE     "  __asm__ volatile(\"\\\n";
      print OUTFILE     "                   lfence                           \\n\\\n";
      print OUTFILE     "                   rdtsc                            \\n\\\n";
      print OUTFILE     "                   movq     %%rax, %[lowb]          \\n\\\n";
      print OUTFILE     "                   movq     %%rdx, %[highb]         \\n\\\n";
      for( my $i = 0; $i < $num; $i++ )
      {
         print OUTFILE     "                   nop \\n\\\n";
      }


      print OUTFILE     "                   lfence                           \\n\\\n";
      print OUTFILE     "                   rdtsc                            \\n\\\n";
      print OUTFILE     "                   movq     %%rax, %[lowa]          \\n\\\n";
      print OUTFILE     "                   movq     %%rdx, %[higha]         \"\n";
      print OUTFILE     "                    :\n";
      print OUTFILE     "                    [lowb]    \"=r\" (lowBitsBefore),\n";
      print OUTFILE     "                    [highb]   \"=r\" (highBitsBefore),\n";
      print OUTFILE     "                    [lowa]    \"=r\" (lowBitsAfter),\n";
      print OUTFILE     "                    [higha]   \"=r\" (highBitsAfter)\n";
      print OUTFILE     "                    :\n";
      print OUTFILE     "                   /*no inputs*/\n";
      print OUTFILE     "                    :\n";
      print OUTFILE     "                    \"rax\",\"rdx\"";
      print OUTFILE     "                   );\n";
      print OUTFILE     "  uint64_t cyclesbefore
                             = (lowBitsBefore & 0xffffffff) | (highBitsBefore << 32);\n";
      print OUTFILE     "  uint64_t cyclesafter
                             = (lowBitsAfter & 0xffffffff) | (highBitsAfter << 32);\n";
      print OUTFILE     "  uint64_t diff = cyclesafter - cyclesbefore;\n";
      print OUTFILE     "  const clock_t frequency( getStatedCPUFrequency() );\n";
      print OUTFILE     "  Sample sample_output;\n";
      print OUTFILE     "  sample_output.measure = (measure_t) $num;\n";
      print OUTFILE     "  sample_output.index   = (sample_index_t) diff / (sample_index_t) frequency;\n";
      print OUTFILE     "  return( sample_output );\n";
      print OUTFILE     "}\n";
      close OUTFILE;
   }
}


sub clean( $$ )
{
   my $args = $_[0];
   my $filelist  = $_[1];
   
   my $length = @$args;
   if( $length == 1 )
   {
      my $clean = shift( @$args );
      if($clean eq "clean")
      {
         my $pwd = `pwd`;
         chomp( $pwd );
         foreach my $filename (keys %$filelist )
         {
            my $cmd = "$pwd/$filename";
            `rm -rf $cmd`;
         }
         my $cmd = "$pwd/Makefile";
         `rm -rf $cmd`;
         exit( 0 );
      }
   }
   #get here then no cleaning to do
}

sub makefile( $ )
{
   my $filelist = shift( @_ );
   my $outfile = "Makefile";
   open OUTFILE, ">$outfile" or die "Couldn't open Makefile!!\n";
   my $pwd = `pwd`;
   chomp( $pwd );
   $pwd = "$pwd/";
	print OUTFILE "CC = gcc\n";
	print OUTFILE "CXX = g++\n";
	print OUTFILE "CXXFLAGS = -I. -I../ -Wall -std=c++11 -O0 -g\n";
	print OUTFILE "CFLAGS = -I. -I../ -Wall -std=c99 -O0 -g\n";
   print OUTFILE "CALIBRATECPPCODEBASE = \\\n";
   {
      my $files = "";
      foreach my $name ( keys %$filelist )
      {
         $name =~ s/\.[^.]+$//;
         $files .= "   $name \\\n";
      }
      $files .= "   $_ \\\n", foreach ( @externalcppfiles );
      $files =~ s/\\\n$//;
      print OUTFILE $files;
   }
	print OUTFILE "\n";
   print OUTFILE "CALIBRATECCODEBASE = \\\n";
   {
      my $files = "";
      $files .= "   $_ \\\n", foreach( @externalcfiles );
      $files =~ s/\\\n$//;
      print OUTFILE $files;
   }
   print OUTFILE "\n";
   print OUTFILE "OBJFILES = \$(addsuffix .o, \$(CALIBRATECPPCODEBASE) ) \$(addsuffix .o, \$(CALIBRATECCODEBASE) )\n";
   print OUTFILE "\n\n";
   print OUTFILE "lib: \n";
   print OUTFILE "	\$(MAKE) \$(OBJFILES)\n";
	print OUTFILE "	\$(AR)	rcs	libcalibrate.a \$(OBJFILES)\n";
	print OUTFILE "\n";
	print OUTFILE "cleanlib: \n";
	print OUTFILE "	rm -rf libcalibrate.a \$(OBJFILES)\n";
   close OUTFILE;
}

sub cinclude($){
    my $out;
    my $include = shift;
    my $extension = ".h";
    $out.="#include <".$include.".".$extension.">\n";
    return $out;
}

sub cppinclude($){
    my $out;
    my $include = shift;
    $out.="#include <".$include.">\n";
    return $out;
}

sub includelocal($$){
    my $out;
    my $include = shift;
    my $extension = shift;
    $out.="#include \"".$include.".".$extension."\"\n";
    return $out;
}

sub genfunc($$)
{
   my $name    = $_[0];
   my $decl    = $_[1];
   my $output  = "";
   $output .= "extern ", if( $decl );
   $output .= ( $decl ? "Sample " : "Sample\n" );
   $output .= "$name()".( $decl ? ";\n" : "\n" );
   return( $output );
}

sub modifycpp($)
{
   my $list = shift( @_ );
   my $cppfile = "calibrate.cpp";
   open INFILE, "<$cppfile" or die "Couldn't open $cppfile\n";
   my @lines = <INFILE>;
   close INFILE;
   my $output = "";
   my $skip = 0;
   my $state = 0;
   my $datastructure = "samplefunctions";
   for my $line ( @lines )
   {
      if( $state == 0 )
      {
      if( $line =~ m/STARTCALIBRATEDECL/ )
      {
         $skip = 1;
         #insert lines here
         $output.= $line;
         foreach my $name (keys %$list )
         {
            $name =~ s/\.[^.]+$//;
            $output .= genfunc( $name, 1 );
         }
      }
      #ignore old declarations
      if( $skip == 0 )
      {
         $output .= $line;
      }
      if( $line =~ m/ENDCALIBRATEDECL/ )
      {
         $skip = 0;
         $state = 1;
      }
      }
      #assume these aren't intermixed 
      if( $state == 1 )
      {
      if( $line =~ m/STARTFUNCTIONINSERT/ )
      {
         $skip = 1;
         #insert lines here
         $output.= $line;
         
         foreach my $name (keys %$list )
         {
            $name =~ s/\.[^.]+$//;
            $output .= "$datastructure.push_back( $name );\n";   
         }
      }
      #ignore old declarations
      if( $skip == 0 )
      {
         $output .= $line;
      }
      if( $line =~ m/ENDFUNCTIONINSERT/ )
      {
         $output .= $line;
         $skip = 0;
      }
      }
   }

   open OUTFILE, ">$cppfile" or die "Couldn't open $cppfile for writing!!\n";
   print OUTFILE $output;
   close OUTFILE;
}
