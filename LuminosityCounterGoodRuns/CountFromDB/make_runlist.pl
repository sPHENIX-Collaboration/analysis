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

my $sql = qq{
    SELECT DISTINCT runnumber
    FROM filelist
    WHERE runnumber BETWEEN 47289 AND 53880
      AND hostname = 'gl1daq'
      AND filename LIKE '%physics%'
    ORDER BY runnumber;
};

my $sth = $dbh->prepare($sql);
$sth->execute();

open my $out, ">", "gl1_physics_runlist.txt" or die $!;

while (my ($run) = $sth->fetchrow_array) {
    print $out "$run\n";
}

close $out;
$sth->finish;
$dbh->disconnect;

print "Done.. created runlist: gl1_physics_runlist.txt\n";
