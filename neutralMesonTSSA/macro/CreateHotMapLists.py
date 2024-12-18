import glob
import re
import numpy as np

def makejoblists():
    goldenrunlist = 'filelists/RD/2024/Nov7/GoldenFEMrunList.txt'
    joblistdir = 'condor/job_filelists/hotmapcheck/'
    runlistdir = 'filelists/RD/2024/Nov7/'

    dstlists = glob.glob(runlistdir+'dst*')

    for i, runlist in enumerate(dstlists):
        joblist = joblistdir + f'job-{i}.list'
        with open(joblist, 'w') as f:
            with open(runlist, 'r') as h:
                line = h.readline()
                f.write(line)

    print(f'Wrote {len(dstlists)} job files with 1 DST each')

def makegoodrunlist():
    logdir = 'condor/out/hotmaps/'
    logfiles = glob.glob(logdir+'job*.out')
    runnum = np.zeros(len(logfiles), dtype=int)
    hasmap = np.zeros(len(logfiles), dtype=bool)
    for i, logfile in enumerate(logfiles):
        with open(logfile,'r') as f:
            for line in f:
                # get the run number
                m1 = re.search('run (\d{5})', line)
                # print(m1)
                if m1:
                    runnum[i] = m1.group(1)
                # check if analysis finished (exited early = no hot map)
                # m2 = re.search('Analysis Completed', line)
                m2 = re.search('Global Tag: ProdA_2024, domain: CEMC_BadTowerMap, timestamp: \d{5}... reply: /cvmfs', line)
                if m2:
                    hasmap[i] = 1

    outfile = 'emcal_withmaps.txt'
    with open(outfile, 'w') as h:
        for i in range(len(runnum)):
            if hasmap[i]:
                h.write(str(runnum[i])+'\n')
    print(f'Wrote {hasmap.sum()} runs with hot maps to {outfile}')

if __name__ == '__main__':
    makejoblists()
    makegoodrunlist()

