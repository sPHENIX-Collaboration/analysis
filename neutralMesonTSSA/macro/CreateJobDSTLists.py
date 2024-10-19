import glob
import re
import os

if __name__ == "__main__":
    # break up the full list of DSTs into smaller lists for condor jobs
    # write multiple per-job file lists with 10 DST files per job
    dstlistdir = 'filelists/RD/2024/Aug10/'
    joblistdir = 'condor/job_filelists/'
    maxjobs = 1000000 # in case we want a small test sample
    filesperjob = 10 # number of DSTs to process in one job
    jobnum = 0
    filesthisjob = 0
    totaldsts = 0

    dstlists = glob.glob(dstlistdir+'dst_calo*')
    print(f'Found {len(dstlists)} run-by-run DST lists')

    lines = []
    for dstlist in dstlists:
        # print(f'Opening new dst list, jobnum = {jobnum}')
        if jobnum > maxjobs:
            break
        with open(dstlist, 'r') as f:
            lines = f.readlines()
        numDSTsThisRun = len(lines)
        totaldsts += numDSTsThisRun
        jobfile = joblistdir + 'job-' + str(jobnum) + '.list'
        if os.path.exists(jobfile):
            os.remove(jobfile)
        jf = open(jobfile, 'a')
        for line in lines:
            jf.write(line)
            filesthisjob += 1
            if filesthisjob >= filesperjob:
                # move to the next job file
                filesthisjob = 0
                jobnum += 1
                jf.close()
                jobfile = joblistdir + 'job-' + str(jobnum) + '.list'
                if os.path.exists(jobfile):
                    os.remove(jobfile)
                jf = open(jobfile, 'a')
                if jobnum >= maxjobs:
                    break
        jf.close()
        jobnum += 1
    print(f'Found {totaldsts} DST files')
    print(f'Wrote {jobnum} per-job DST lists with <= {filesperjob} DSTs each')

