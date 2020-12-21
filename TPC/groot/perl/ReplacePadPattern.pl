#! /usr/bin/perl -w
#
#  This script is used to identify any layer 1-13 vias inside the 
#  connector blocks and change them to have an extent of only 1-8.
#
use strict;
use CGI qw(:standard);

my %PolygonDefinition;
my %MicroVia;

open (ORIG, "<", "OUTPUT.XML") or die "Can't open original file $!/n";

my $CurrentSignal = "None";
while (<ORIG>)
{
    my $line = $_;
    
   if ($line =~ /<signal name/)
    {
	my @lineValues = split(/"/,$line);
	$CurrentSignal = $lineValues[1];
    }
    
    if ($CurrentSignal =~ /ZZ/)
    {
	if (($line =~ /polygon/) || ($line =~ /vertex/))
	{
	    push(@{$PolygonDefinition{$CurrentSignal}}, $line);
	}

	if (($line =~ /via/) && ($line =~ /extent="15-16"/))
	{
	    $MicroVia{$CurrentSignal} = $line;
	}

	
    }
}

close ORIG;

my $Fav = "ZZ.13.113";

# Print the Polygon
foreach my $element (@{$PolygonDefinition{$Fav}})
{
    print "       $element";
}

#  Print the MicroVia
print "    $MicroVia{$Fav}";

open (BRD, "<", "StonyBrookUniversity-R2pad-Rev_D-OldPads.brd") or die "Can't open original file $!/n";
open (OUT, ">", "StonyBrookUniversity-R2pad-Rev_D.brd") or die "Can't open original file $!/n";

$CurrentSignal = "None";
while (<BRD>)
{
    my $line = $_;
    
    if ($line =~ /<signal name/)
    {
	my @lineValues = split(/"/,$line);
	$CurrentSignal = $lineValues[1];
    }
    
    if ($CurrentSignal =~ /ZZ/)
    {
	my $WriteLine = "true";
	if (($line =~ /<polygon width/))
	{
	    $WriteLine = "false";
	    foreach my $element (@{$PolygonDefinition{$CurrentSignal}})
	    {
		print OUT $element;
	    }
	}

	if (($line =~ /vertex/) || ($line =~ /<\/polygon/))
	{
	    $WriteLine = "false";
	}

#	if (($line =~ /via/) && ($line =~ /extent="15-16"/))
#	{
#	    $WriteLine = "false";
#	    print OUT $MicroVia{$CurrentSignal};
#	}

#	if (($line =~ /wire/) && ($line =~ /layer="15"/))
#	{
#	    $WriteLine = "false";
#	}

#	if (($line =~ /via/) && ($line =~ /extent="10-15"/))
#	{
#	    my @lineValues = split(/"/,$line);
#	    my $x2 = $lineValues[1];
#	    my $y2 = $lineValues[3];

#	    my @viaValues = split(/"/,$MicroVia{$CurrentSignal});
#	    my $x1 = $viaValues[1];
#	    my $y1 = $viaValues[3];

#	    $WriteLine = "false";
#	    print OUT $line;
#	    print OUT "<wire x1=\"$x1\" y1=\"$y1\" x2=\"$x2\" y2=\"$y2\" width=\"0.1016\" layer=\"15\"/>\n";
#	}
	
	if ($WriteLine =~ /true/)
	{
	    print OUT $line;
	}
	
    }
    else
    {
	print OUT $line;
    }


}
close BRD;
close OUT;
