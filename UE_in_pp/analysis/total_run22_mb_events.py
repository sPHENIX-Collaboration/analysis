import ROOT

total_entries = 0
corrupted_files = 0
for i in range(0, 10000):
    fname = f"/sphenix/tg/tg01/jets/egm2153/UEinppOutput/sim_run22_mb_total_events_zvtx_lt_30_output_{i}.root"
    try:
        f = ROOT.TFile.Open(fname)
        if not f or f.IsZombie():
            print(f"Warning: Could not open or file is corrupted: {fname}")
            corrupted_files += 1
            continue
    except Exception as e:
        print(f"Exception while opening file {fname}: {e}")
        continue
    
    tree = f.Get("T")
    if not tree:
        print(f"TTree 'T' not found in {fname}")
        f.Close()
        continue
    entries = tree.GetEntries()
    print(f"{fname}: {entries} entries")
    total_entries += entries
    f.Close()

print(f"\nTotal number of entries across all files: {total_entries}")
print(f"\nTotal number of corrupted files: {corrupted_files}")
