#!/usr/local/bin/perl

use strict;
use warnings;
my @momarray = (1,2,4,8,12,16,32,40,50);
my @etaarray = (0);
my @partarray = ("anti_neutron","anti_proton","e+","e-","neutron","pi+","pi-","proton");
foreach my $particle (@partarray)
{
    foreach my $eta (@etaarray)
    {
	foreach my $mom (@momarray)
	{
	    my $cmdline = sprintf("perl run_condor.pl %s %s %s /sphenix/user/pinkenbu/jade",$particle,$eta,$mom);
	    print "cmd: $cmdline\n";
	    system($cmdline);
	}
    }
}
