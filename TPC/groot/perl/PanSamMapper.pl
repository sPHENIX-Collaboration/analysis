#! /usr/bin/perl -w
#
#
use strict;
#use CGI qw(:standard);

open (INPUT,  "<", "PanSam_REV_A.brd") or die "Can't open original file $!/n";
open (OUTPUT, ">", "PanSamMap.txt") or die "Can't open original file $!/n";

my %PadMap;
my $CurrentSignal="GND";
my @Connector = ();
my @Pad = ();

while (<INPUT>)
{
    my $line = $_;
    
    if ($line =~ /<signal name/)
    {
	my @lineValues = split(/"/,$line);
	$CurrentSignal = $lineValues[1];
	@Connector = ();
	@Pad       = ();
    }
    
    if (!($CurrentSignal =~ /GND/) && $line =~ /<contactref/ )
    {
	print " push ";
	my @ContactRefValues = split(/"/,$line);
	push @Connector, $ContactRefValues[1];
	push @Pad, $ContactRefValues[3];
    }

    if (!($CurrentSignal =~ /GND/) && $line =~ /<\/signal/)
    {
	my $length = $#Connector + 1;
	print "$length\n";
	for (my $i=0; $i <= $#Connector; $i++)
	{
	    print OUTPUT "$Connector[$i]$Pad[$i],";
	}
	print OUTPUT "\n";
    }


}
close (INPUT);
close (OUTPUT);
