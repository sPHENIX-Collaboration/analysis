import ROOT
import glob

# Pattern matching your files
file_pattern = "/sphenix/tg/tg01/jets/egm2153/UEinppOutput/sim_run22_jet20_total_events_zvtx_lt_30_output_*.root"
file_list = glob.glob(file_pattern)

total_entries = 0

for fname in file_list:
    f = ROOT.TFile.Open(fname)
    if not f or f.IsZombie():
        print(f"Could not open {fname}")
        continue
    tree = f.Get("T")
    if not tree:
        print(f"TTree 'T' not found in {fname}")
        continue
    entries = tree.GetEntries()
    print(f"{fname}: {entries} entries")
    total_entries += entries
    f.Close()

print(f"\nTotal number of entries across all files: {total_entries}")
