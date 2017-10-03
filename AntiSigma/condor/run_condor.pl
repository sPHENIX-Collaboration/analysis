#!/usr/local/bin/perl

use strict;
use warnings;
use Getopt::Long;
use File::Path;
use File::Basename;
use File::Copy;

my $test;
GetOptions("test"=>\$test);
if ($#ARGV < 3)
{
    print "usage: run_condor.pl <particle> <eta> <momentum> <outdir>\n";
    print "options:\n";
    print "-test: testmode - no condor submission\n";
    exit(-2);
}
else
{
    print "running condor\n";
}
my $curdir = `pwd`;
chomp $curdir;
my $rundir = $curdir;

my $executable = sprintf("run_single_particle.csh");

my $particle = $ARGV[0];
my $eta = $ARGV[1];
my $mom = $ARGV[2];
my $outdir = $ARGV[3];
mkpath($outdir);

my $indir = "/sphenix/data/data02/review_2017-08-02/single_particle/spacal2d/fieldmap";

my $suffix = sprintf("%s_eta%s_%sGeV",$particle,$eta,$mom);
my $listfile = sprintf("%s/%s.list",$outdir,$suffix);
my $outputdir = sprintf("%s/ntuple",$outdir);
mkpath($outputdir);
my $outputfile = sprintf("%s/%s.root",$outputdir,$suffix);
open(F,">$listfile");
my $nfiles = 0;
for (my $iseg=0; $iseg<12; $iseg++)
{
    my $infile = sprintf("%s/G4Hits_sPHENIX_%s_eta%s_%sGeV-%04d.root",$indir,$particle,$eta,$mom,$iseg);
    if (! -f $infile)
    {
	print "could not locate $infile\n";
	next;
    }
    print F "$infile\n";
    $nfiles++;
}
close(F);
if ($nfiles <= 0)
{
    die "no files in list - dying now\n";
}
my $logdir = sprintf("%s/condor/log",$outdir);
mkpath($logdir);
my $jobfile = sprintf("%s/condor_%s.job",$logdir,$suffix);
my $condorlogfile = sprintf("%s/condor_%s.log",$logdir,$suffix);
if (-f $condorlogfile)
{
    unlink $condorlogfile;
}
my $outfile = sprintf("%s/condor_%s.out",$logdir,$suffix);
my $errfile = sprintf("%s/condor_%s.err",$logdir,$suffix);
print "job: $jobfile\n";
open(F,">$jobfile");
print F "Universe       = vanilla\n";
print F "Executable     = $executable\n";
print F "+Experiment    = \"phenix\"\n";
#print F "+Job_Type      = \"highmem\"\n";
print F "Arguments       = \"$listfile $outputfile\"\n";
print F "notification   = Error\n";
print F "Output         = $outfile\n";
print F "Error          = $errfile\n";
print F "Log            = $condorlogfile\n";
print F "Initialdir     = $rundir\n";
print F "PeriodicHold   = (NumJobStarts>=1 && JobStatus == 1)\n";
#print F "Requirements   = (CPU_Experiment == \"phenix\" && (SlotID == 39 ||SlotID == 40)) \n";
print F "Requirements   = (CPU_Experiment == \"phenix\") \n";
print F "Priority       = 42\n";
print F "job_lease_duration = 3600\n";
print F "Queue 1\n";
close(F);

if (defined $test)
{
    print "would submit $jobfile\n";
}
else
{
    system("condor_submit $jobfile");
}
