#!/usr/bin/env perl
use strict;
use warnings;
use DBI;

my $dbname = "daq";
my $host   = "sphnxdaqdbreplica";

my $dbh = DBI->connect("dbi:Pg:dbname=$dbname;host=$host",
                       undef, undef,
                       { RaiseError => 1, AutoCommit => 1 })
          or die $DBI::errstr;

if (@ARGV < 1) {
    print "ERROR :: Need an input run list\n";
    die "Usage: $0 runlist.txt\n";
}

my $runlist_file = $ARGV[0];
open my $fh, "<", $runlist_file or die "Cannot open $runlist_file: $!";
my @runs;
while (<$fh>) {
    chomp;
    next if /^\s*$/;
    push @runs, $_;
}
close $fh;

my $sth_counts = $dbh->prepare("
    SELECT SUM(raw) AS raw_sum, SUM(live) AS live_sum
    FROM gl1_scalers
    WHERE runnumber = ? AND index = 10
");

my $sth_time = $dbh->prepare("
    SELECT EXTRACT(EPOCH FROM brtimestamp), EXTRACT(EPOCH FROM ertimestamp)
    FROM run
    WHERE runnumber = ?
");

my ($tot_raw, $tot_live, $tot_raw_corr) = (0, 0, 0);
my $collfreq = 111 * 78e3;  # 8.658e6

foreach my $run (@runs) {
    $sth_counts->execute($run);
    my ($raw10, $live10) = $sth_counts->fetchrow_array();
    $raw10  ||= 0;
    $live10 ||= 0;

    # get times
    $sth_time->execute($run);
    my ($tstart, $tend) = $sth_time->fetchrow_array();
    next unless ($tstart && $tend);
    my $dt = $tend - $tstart;
    next if $dt <= 0;

    $tot_raw  += $raw10;
    $tot_live += $live10;

    my $rate = $raw10 / ($dt * $collfreq);
    next if $rate <= 0.0 || $rate >= 1.0;

    my $mu = -log(1.0 - $rate);
    my $corr = $mu * $dt * $collfreq;

    $tot_raw_corr  += $corr;
}

$sth_counts->finish;
$sth_time->finish;
$dbh->disconnect;

my $tot_live_corr = 0;
if ($tot_raw > 0) {
    my $livefrac = $tot_live / $tot_raw;
    $tot_live_corr = $tot_raw_corr * $livefrac;
}

my ($lumi_live, $lumi_raw, $lumi_live_corr, $lumi_raw_corr) = (0, 0, 0, 0);
my $mbd_cross = 25.2;
my $norm = 1e-09;
$lumi_live = $tot_live / $mbd_cross * $norm;
$lumi_raw = $tot_raw / $mbd_cross * $norm;
$lumi_live_corr = $tot_live_corr / $mbd_cross * $norm;
$lumi_raw_corr = $tot_raw_corr / $mbd_cross *$norm;

print "Total raw (uncorrected): $tot_raw / lumi: $lumi_raw\n";
print "Total live (uncorrected): $tot_live / lumi: $lumi_live\n";
print "Total raw (pileup-corrected): $tot_raw_corr / lumi: $lumi_raw_corr\n";
print "Total live (pileup-corrected): $tot_live_corr / lumi: $lumi_live_corr\n";
