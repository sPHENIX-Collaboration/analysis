#!/usr/bin/env python3
# Make corral_m job lists from a segment_entries.txt (written by count_entries.C):
#   one list = contiguous segments of ONE run, in segment order, ~TARGET events.
#   Per run: nlists = max(1, round(run_events/TARGET)); cut the run into nlists contiguous groups
#   of nearly equal event counts. Lists are numbered 0..N-1 across runs (run order, then segment
#   order) = the SLURM array index. Writes <outdir>/list_NN.txt (one file path per line),
#   <outdir>/lists_summary.txt, and <outdir>/manifest.txt.
#   Usage: make_lists.py <outdir> <target_events_per_list> <collect_dir>
import os, sys, collections, datetime
outdir, TARGET, cdir = sys.argv[1], int(sys.argv[2]), sys.argv[3]
rows   = [l.split() for l in open(os.path.join(outdir, "segment_entries.txt")) if l.strip()]
bad    = [r for r in rows if int(r[2]) <= 0]
if bad:
	print("unreadable/empty files (left out of the lists):"); print("\n".join(r[3] for r in bad))
runs   = collections.OrderedDict()
for run, seg, ne, path in rows:
	if int(ne) <= 0: continue
	runs.setdefault(int(run), []).append((int(seg), int(ne), path))
for r in runs: runs[r].sort()
ilist, summary = 0, []
for r, segs in runs.items():
	tot    = sum(s[1] for s in segs)
	nl     = max(1, int(round(tot / TARGET)))
	cuts   = [tot * (k + 1) / nl for k in range(nl)]	# cumulative-event targets for the end of each list
	groups, cur, acc, k = [], [], 0, 0
	for s in segs:
		cur.append(s); acc += s[1]
		if k < nl - 1 and acc >= cuts[k]:
			groups.append(cur); cur = []; k += 1
	if cur: groups.append(cur)
	for g in groups:
		with open(os.path.join(outdir, "list_%02d.txt" % ilist), "w") as f:
			for s in g: f.write(s[2] + "\n")
		summary.append("%02d  run %d  segments %4d-%4d  nseg %3d  events %8d" % (ilist, r, g[0][0], g[-1][0], len(g), sum(s[1] for s in g)))
		ilist += 1
with open(os.path.join(outdir, "lists_summary.txt"), "w") as f:
	f.write("\n".join(summary) + "\n")
nev = sum(int(r[2]) for r in rows if int(r[2]) > 0)
with open(os.path.join(outdir, "manifest.txt"), "w") as f:
	f.write("collect_dir  %s\ntarget       %d\nnlists       %d\nnfiles       %d\nnevents      %d\nmade         %s\n"
	        % (cdir, TARGET, ilist, sum(len(v) for v in runs.values()), nev, datetime.datetime.now().isoformat(timespec="seconds")))
print("\n".join(summary))
print("lists: %d   files: %d   events: %d   (unreadable files: %d)" % (ilist, sum(len(v) for v in runs.values()), nev, len(bad)))
