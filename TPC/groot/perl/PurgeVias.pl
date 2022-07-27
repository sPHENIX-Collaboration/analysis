#! /usr/bin/perl -w
#
#  This script is used to identify any layer 1-13 vias inside the 
#  connector blocks and change them to have an extent of only 1-8.
#
use strict;
use CGI qw(:standard);

open (OLD, "<", "StonyBrookUniversity-R2pad-Rev_B-extraVia.brd") or die "Can't open original file $!/n";
open (NEW, ">", "StonyBrookUniversity-R2pad-Rev_B-cleanVia.brd") or die "Can't open original file $!/n";

my %PadMap;

my $CurrentSignal="NOSIGNAL";
my @X=();
my @Y=();
my @ViaBuffer=();

while (<OLD>)
{
    my $line = $_;
    
    if ($line =~ /<signal name/)
    {
	my @lineValues = split(/"/,$line);
	$CurrentSignal = $lineValues[1];
	@X=();
	@Y=();
	@ViaBuffer=();
    }
    
    my $WriteCurrentLine = "true";
    if ($CurrentSignal =~ /ZZ/ && $line =~ /<via/ && $line =~ /extent=\"2-15\"/)
    {
	push @ViaBuffer, $line;
	$WriteCurrentLine = "false";
    }

    if ($CurrentSignal =~ /ZZ/ && $line =~ /<wire/ && $line =~ /layer=\"2\"/)
    {
	my @WireDetails = split(/"/,$line);
	push @X, $WireDetails[1], $WireDetails[5];
	push @Y, $WireDetails[3], $WireDetails[7];
    }

    if ($CurrentSignal =~ /ZZ/ && $line =~ /<\/signal/)
    {
	#print "$CurrentSignal\n";
	foreach my $via (@ViaBuffer)
	{
	    my @ViaDetails = split(/"/,$via);
	    my $viax = $ViaDetails[1];
	    my $viay = $ViaDetails[3];
	    my $Layer2Matches = 0;
	    #print "$viax  $viay\n";
	    for (my $i=0; $i <= $#X; $i++)
	    {
		#print "    $X[$i]  $Y[$i]\n";		
		if (($X[$i] == $viax) && ($Y[$i] == $viay))
		{
		    $Layer2Matches++;
		}
	    }
	    #print "Matches = $Layer2Matches\n";
	    if ($Layer2Matches < 2)
	    {
		print NEW $via;
	    }
	    else
	    {
		print "Killed an Extraneous Via!!\n";
	    }
	}
    }

    if ($WriteCurrentLine =~ /true/)
    {
	print NEW $line;
    }
}
close (OLD);
close (NEW);
