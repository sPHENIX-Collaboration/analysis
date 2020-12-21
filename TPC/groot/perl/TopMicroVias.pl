#! /usr/bin/perl -w
#
#  This script is used to identify any layer 1-13 vias inside the 
#  connector blocks and change them to have an extent of only 1-8.
#
use strict;
use CGI qw(:standard);

open (ORIG, "<", "PadMap.txt") or die "Can't open original file $!/n";
open (OLD, "<", "StonyBrookUniversity-R2pad-Rev_B-badVia.brd") or die "Can't open original file $!/n";
open (NEW, ">", "StonyBrookUniversity-R2pad-Rev_B-goodVia.brd") or die "Can't open original file $!/n";

my %PadMap;

while (<ORIG>)
{
    my $line = $_;
    my @lineValues = split(/,/,$line);

    my $CurrentSignal = $lineValues[0];
    $PadMap{$CurrentSignal}=$line;
}
close (ORIG);

my $CurrentSignal="NOSIGNAL";
my @X=();
my @Y=();
my @Xgnd=();
my @Ygnd=();

while (<OLD>)
{
    my $line = $_;
    
    if ($line =~ /<signal name/)
    {
	my @lineValues = split(/"/,$line);
	$CurrentSignal = $lineValues[1];
	@X=();
	@Y=();
	@Xgnd=();
	@Ygnd=();
    }
    
#  Fix up the current signal using the pad map...
    if ($CurrentSignal =~ /ZZ/ && $line =~ /<via/ && $line =~ /extent=\"2-15\"/)
    {
	my @lineValues = split(/"/,$line);
	push @X, $lineValues[1];
	push @Y, $lineValues[3];
    }

    if ($CurrentSignal =~ /ZZ/ && $line =~ /<\/signal/)
    {
	my @Offsets = (461.2386, 522.1986);
	my @ROTATE = (-0.202458193, -0.151843645, -0.101229097, -0.050614548, 0, 0.050614548, 0.101229097, 0.151843645);
	
	
	#  Determine which rotation and which offset using the PadMap.
	my $PadInfo = $PadMap{$CurrentSignal};
	my @PadDetails = split(/,/,$PadInfo);
	my $Connector = $PadDetails[1];
	my @ConnDetails = split(/\$/,$Connector);
	my $ConnIndex = $ConnDetails[1];
	print "ConnIndex $ConnIndex\n";
	my $iROT = int(($ConnIndex-1.0)/2.0);
	my $iSAM = ($ConnIndex)%2;
	
	my @let  = ("F", "E", "D", "C", "B", "A");
	my %Rows = ("F" => "0", "E"=>"1", "D"=>"2", "C"=>"3", "B"=>"4", "A"=>"5");
	
	my $iROW = $Rows{$PadDetails[3]};;
	my $iCOL = $PadDetails[4]-1;
	
	print "$iROT  $iSAM  $CurrentSignal\n";
	#  We have now reached the point where we need to add a contactref line to the file...
	my $BESTDISTANCE = 999999;
	my $BESTWIRE  = -1;
	my $BESTROT   = -1;
	my $BESTSAM   = -1;
	my $BESTROW   = -1;
	my $BESTCOL   = -1;
	for (my $iWIRE=0; $iWIRE<@X; $iWIRE++)
	{
	    my $THETA = $ROTATE[$iROT];
	    my $Xprime = ( cos($THETA)*$X[$iWIRE] + sin($THETA)*$Y[$iWIRE])*1000.0/25.4;
	    my $Yprime = (-sin($THETA)*$X[$iWIRE] + cos($THETA)*$Y[$iWIRE])*1000.0/25.4;
	    #print "$X[$iWIRE] $Y[$iWIRE] $iROT $THETA $Xprime $Yprime\n";
	    
	    my $Xoffset = 0.0;
	    my $Yoffset = $Offsets[$iSAM];
	    
	    my $Xcircle = $Xoffset + (-125.0+$iROW*50.0);
	    my $Ycircle = $Yoffset*1000.0/25.4 + (-(25.0 + 14.0*50.0) + $iCOL*50.0);
	    
	    my $Distance = sqrt(($Xprime-$Xcircle)**2.0 + ($Yprime-$Ycircle)**2.0);
	    if ($Distance < $BESTDISTANCE)
	    {
		$BESTDISTANCE = $Distance;
		$BESTWIRE     = $iWIRE;
	    }
	}
	
	print "Distance = $BESTDISTANCE mils ";
	print "iWIRE = $BESTWIRE ";
	print "iROT  = $BESTROT ";
	print "iSAM  = $BESTSAM ";
	print "iROW  = $BESTROW ";
	print "iCOL  = $BESTCOL \n";
	
	if ($BESTDISTANCE > 50)
	{
	    print "FAILURE ON DISTANCE!\n";
	    print "$BESTDISTANCE \n";
	    exit;
	}
	
	#  Put in one via and two new wires...good luck...
	my $THETA = -$ROTATE[$iROT];  # rotate the OTHER way...
	my $Xoffset = 0.0;
	my $Yoffset = $Offsets[$iSAM];	
        my $Xcircle = $Xoffset + (-125.0+$iROW*50.0);
	my $Ycircle = $Yoffset*1000.0/25.4 + (-(25.0 + 14.0*50.0) + $iCOL*50.0);
	my $x1 = ( cos($THETA)*$Xcircle + sin($THETA)*$Ycircle)*25.4/1000.0;   # location of the original pin...
	my $y1 = (-sin($THETA)*$Xcircle + cos($THETA)*$Ycircle)*25.4/1000.0;


	$Ycircle = $Ycircle + 25;
	my $x2 = ( cos($THETA)*$Xcircle + sin($THETA)*$Ycircle)*25.4/1000.0;   # location of the new via...
	my $y2 = (-sin($THETA)*$Xcircle + cos($THETA)*$Ycircle)*25.4/1000.0;

	my $x3 = $X[$BESTWIRE];  # location of the existing via...
	my $y3 = $Y[$BESTWIRE];


	print NEW "<via x=\"$x2\" y=\"$y2\" extent=\"1-2\" drill=\"0.127\"/>\n";
	print NEW "<wire x1=\"$x1\" y1=\"$y1\" x2=\"$x2\" y2=\"$y2\" width=\"0.1016\" layer=\"1\"/>\n";
	print NEW "<wire x1=\"$x2\" y1=\"$y2\" x2=\"$x3\" y2=\"$y3\" width=\"0.1016\" layer=\"2\"/>\n";

    }


#  Fix up the GND vias by looping over all of them (much slower since so many...).
    if ($CurrentSignal =~ /GND/ && $line =~ /<via/ && $line =~ /extent=\"2-15\"/)
    {
	my @lineValues = split(/"/,$line);
	push @Xgnd, $lineValues[1];
	push @Ygnd, $lineValues[3];
    }

    if ($CurrentSignal =~ /GND/ && $line =~ /<\/signal/)
    {
	my @Offsets = (461.2386, 522.1986);
	my @ROTATE = (-0.202458193, -0.151843645, -0.101229097, -0.050614548, 0, 0.050614548, 0.101229097, 0.151843645);
	my @ConnNames = ("Uy1", "Uy2", "Uy3", "Uy4", "Uy5", "Uy6", "Uy7", "Uy8", "Uy9", "Uy10", "Uy11", "Uy12", "Uy13", "Uy14", "Uy15", "Uy16");
	my @GndPad = ("Ay1",  "Ay2",  "Ay3",  "Ay4",  "Ay5",  "Ay6",  "Ay7",  "Ay8",  "Ay9",  "Ay10", 
		      "Ay11", "Ay12", "Ay13", "Ay14", "Ay15", "Ay16", "Ay17", "Ay18", "Ay19", "Ay20", 
		      "Ay21", "Ay22", "Ay23", "Ay24", "Ay25", "Ay26", "Ay27", "Ay28", "Ay29", 
		      "By1",  "By2",  "By9",  "By16", "By23", "By30", 
		      "Cy1",  "Cy2",  "Cy9",  "Cy16", "Cy23", "Cy30", 
		      "Dy1",  "Dy2",  "Dy9",  "Dy16", "Dy23", "Dy30", 
		      "Ey1",  "Ey30", 
		      "Fy1",  "Fy30" );
	

	for my $Connector (@ConnNames)
	{
	    my @ConnDetails = split(/y/,$Connector);
	    my $ConnIndex = $ConnDetails[1];
	    print "ConnIndex $ConnIndex\n";
	    my $iROT = int(($ConnIndex-1.0)/2.0);
	    my $iSAM = ($ConnIndex)%2;
	    
	    for my $PadInfo (@GndPad)
	    {
		my @PadDetails = split(/y/,$PadInfo);
		
		my @let  = ("F", "E", "D", "C", "B", "A");
		my %Rows = ("F" => "0", "E"=>"1", "D"=>"2", "C"=>"3", "B"=>"4", "A"=>"5");
		
		my $iROW = $Rows{$PadDetails[0]};;
		my $iCOL = $PadDetails[1]-1;
		
		print "$iROT  $iSAM  $CurrentSignal\n";
		#  We have now reached the point where we need to add a contactref line to the file...
		my $BESTDISTANCE = 999999;
		my $BESTWIRE  = -1;
		my $BESTROT   = -1;
		my $BESTSAM   = -1;
		my $BESTROW   = -1;
		my $BESTCOL   = -1;
		for (my $iWIRE=0; $iWIRE<@Xgnd; $iWIRE++)
		{
		    my $THETA = $ROTATE[$iROT];
		    my $Xprime = ( cos($THETA)*$Xgnd[$iWIRE] + sin($THETA)*$Ygnd[$iWIRE])*1000.0/25.4;
		    my $Yprime = (-sin($THETA)*$Xgnd[$iWIRE] + cos($THETA)*$Ygnd[$iWIRE])*1000.0/25.4;
		    #print "$Xgnd[$iWIRE] $Ygnd[$iWIRE] $iROT $THETA $Xprime $Yprime\n";
		    
		    my $Xoffset = 0.0;
		    my $Yoffset = $Offsets[$iSAM];
		    
		    my $Xcircle = $Xoffset + (-125.0+$iROW*50.0);
		    my $Ycircle = $Yoffset*1000.0/25.4 + (-(25.0 + 14.0*50.0) + $iCOL*50.0);

		    #  Shift the search region a little bit for accuracy of the search...
		    $Xcircle = $Xcircle - 20;
		    $Ycircle = $Ycircle + 20;

		    my $Distance = sqrt(($Xprime-$Xcircle)**2.0 + ($Yprime-$Ycircle)**2.0);
		    if ($Distance < $BESTDISTANCE)
		    {
			$BESTDISTANCE = $Distance;
			$BESTWIRE     = $iWIRE;
		    }
		}
		
		print "Distance = $BESTDISTANCE mils ";
		print "iWIRE = $BESTWIRE ";
		print "iROT  = $BESTROT ";
		print "iSAM  = $BESTSAM ";
		print "iROW  = $BESTROW ";
		print "iCOL  = $BESTCOL \n";
		
		if ($BESTDISTANCE > 10)
		{
		    print "FAILURE ON DISTANCE!\n";
		    print "$BESTDISTANCE \n";
		    print "iWIRE = $BESTWIRE ";
		    print "iROT  = $BESTROT ";
		    print "iSAM  = $BESTSAM ";
		    print "iROW  = $BESTROW ";
		    print "iCOL  = $BESTCOL ";
		    print "Connector  = $Connector ";
		    print "PadInfo    = $PadInfo ";
		    print "\n";
		    exit;
		}
		
		#  Put in one via and two new wires...good luck...
		my $THETA = -$ROTATE[$iROT];  # rotate the OTHER way...
		my $Xoffset = 0.0;
		my $Yoffset = $Offsets[$iSAM];	
		my $Xcircle = $Xoffset + (-125.0+$iROW*50.0);
		my $Ycircle = $Yoffset*1000.0/25.4 + (-(25.0 + 14.0*50.0) + $iCOL*50.0);
		my $x1 = ( cos($THETA)*$Xcircle + sin($THETA)*$Ycircle)*25.4/1000.0;   # location of the original pin...
		my $y1 = (-sin($THETA)*$Xcircle + cos($THETA)*$Ycircle)*25.4/1000.0;
		
		
		$Ycircle = $Ycircle + 25;
		my $x2 = ( cos($THETA)*$Xcircle + sin($THETA)*$Ycircle)*25.4/1000.0;   # location of the new via...
		my $y2 = (-sin($THETA)*$Xcircle + cos($THETA)*$Ycircle)*25.4/1000.0;
		
		my $x3 = $Xgnd[$BESTWIRE];  # location of the existing via...
		my $y3 = $Ygnd[$BESTWIRE];
		
		
		print NEW "<via x=\"$x2\" y=\"$y2\" extent=\"1-2\" drill=\"0.127\"/>\n";
		print NEW "<wire x1=\"$x1\" y1=\"$y1\" x2=\"$x2\" y2=\"$y2\" width=\"0.1016\" layer=\"1\"/>\n";
		print NEW "<wire x1=\"$x2\" y1=\"$y2\" x2=\"$x3\" y2=\"$y3\" width=\"0.1016\" layer=\"2\"/>\n";
	    }
	}
    }

    print NEW $line;
    
}
close (OLD);
close (NEW);
