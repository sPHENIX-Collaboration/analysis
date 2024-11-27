import re
from glob import glob
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import sys
# import seaborn as sns

# sns.set(style='whitegrid')

class LumiPolarAna:
    matches = []
    scalers = []
    runtimes = []
    errors = []
    has_scalers = False
    runinfo = {}
    logdir_nm = 'condor/out/NM'
    logdir_tssa = 'condor/out/tssahists'
    df = pd.DataFrame()

    # def __init__(self):

    def ScanFile(self, filename):
        hasmatch = False
        jobnum = ''
        # print(filename)
        m0 = re.search(r'job_(\d+)', filename)
        if m0:
            jobnum = m0.group(1)
            # print(f'Jobnum = {jobnum}')
            if int(jobnum) > 6892:
                return

        with open(filename, 'r') as f:
            lines = []
            error = ''
            for line in f:
                lines.append(line)
                # print(line, end='')
                m = re.search(r'^\d{5},.*$', line) # runnum, pol, luminosity
                if m:
                    # print('Found match in ', filename, ': ', m.group(), sep='')
                    self.matches.append(m.group())
                    hasmatch = True
                m2 = re.search(r'MBDNS GL1p scalers: \[(\d.*)\]$', line) # GL1p scalers
                if m2:
                    # print('Found GL1P scalers in ', filename, ': ', m2.group(1), sep='')
                    self.scalers.append(m2.group(1))
                m3 = re.search(r'real\s+(\d+)m(\d+\.\d+)s', line) # job execution time
                if m3:
                    time_min = int(m3.group(1))
                    time_sec = float(m3.group(2))
                    # print(f'Found time {time_min + (time_sec/60)}')
                    self.runtimes.append(time_min + (time_sec/60))
                m4 = re.search(r'^error:.*$', line)
                if m4:
                    self.errors.append(jobnum + ', ' + m4.group(0))
            if not hasmatch:
                print(f'No matches found in log {filename}')
        return

    def ScanAllFiles(self):
        # files = glob(self.logdir_nm + '/job_0.out')
        files = glob(self.logdir_nm + '/*.out')
        files = sorted(files)
        print(f'Reading {len(files)} log files')
        # print('Reading the following log files:')
        # print(files)
        for i, file in enumerate(files):
            if i % 1000 == 0:
                print(f'Reading file {i}')
            # if i > 6892:
            #     break
            self.ScanFile(file)
        with open('NMerrors.txt', 'w') as outfile:
            outfile.writelines([i+'\n' for i in self.errors])
        return

    def ScanTSSALogs(self):
        tssa_runtimes = []
        files = glob(self.logdir_tssa + '/*.out')
        for file in files:
            with open(file, 'r') as f:
                for line in f:
                    m = re.search(r'real\s+(\d+)m(\d+\.\d+)s', line) # job execution time
                    if m:
                        time_min = int(m.group(1))
                        time_sec = float(m.group(2))
                        # print(f'Found time {time_min + (time_sec/60)}')
                        tssa_runtimes.append((time_min*60) + time_sec)
        return tssa_runtimes

    def MakeDataFrame(self):
        print('Found', len(self.matches), 'matches,', len(self.scalers), 'scalers')
        for count, line in enumerate(self.matches):
            strvalues = line.split(',')
            runno = int(strvalues[0])
            bpol = float(strvalues[1])
            blumiup = int(float(strvalues[2]))
            blumidown = int(float(strvalues[3]))
            ypol = float(strvalues[4])
            ylumiup = int(float(strvalues[5]))
            ylumidown = int(float(strvalues[6]))
            values = [runno, bpol, blumiup, blumidown, ypol, ylumiup, ylumidown]
            if len(self.matches) == len(self.scalers):
                self.has_scalers = True
                # scaler = np.fromstring(self.scalers[count], dtype=int, sep=', ')
                # print(scaler)
                # values.append(scaler)
                values.append(self.scalers[count])

            if runno in self.runinfo:
                # print('Found existing runno', runno)
                oldvalues = self.runinfo[runno]
                if (oldvalues[1] != values[1]) or (oldvalues[4] != values[4]):
                    print('Run #', runno, ': Got different polarization values!', sep='')
                    return
            else:
                # print('Found new runno', runno)
                self.runinfo[runno] = values
        # print(self.runinfo)
        if self.has_scalers:
            self.df = pd.DataFrame(columns=['RunNum', 'BluePol', 'BlueLumiUp', 'BlueLumiDown', 'YellowPol', 'YellowLumiUp', 'YellowLumiDown', 'GL1P'])
        else:
            self.df = pd.DataFrame(columns=['RunNum', 'BluePol', 'BlueLumiUp', 'BlueLumiDown', 'YellowPol', 'YellowLumiUp', 'YellowLumiDown'])
        for vals in self.runinfo.values():
            self.df.loc[len(self.df.index)] = vals
        print(self.df)
        print(self.df.describe())
        return

    def ReadRunTimes(self, filename):
        with open(filename, 'r') as f:
            lines = f.readlines()
            self.runtimes = []
            for line in lines:
                self.runtimes.append(float(line[:-2]))

    def GetDataFrame(self, filename):
        try:
            self.df = pd.read_csv(filename)
        except:
            return False
        print(self.df)
        # print(self.df.describe())
        return True

    def WriteRunTimes(self, filename):
        with open(filename, 'w') as f:
            for time in self.runtimes:
                f.write(str(time)+'\n')

    def WriteDataFrame(self, filename):
        self.df.to_csv(filename)
        return

    def GetRelLumi(self):
        blue_up_total = self.df['BlueLumiUp'].sum()
        blue_down_total = self.df['BlueLumiDown'].sum()
        blue_rel = blue_up_total / blue_down_total
        yellow_up_total = self.df['YellowLumiUp'].sum()
        yellow_down_total = self.df['YellowLumiDown'].sum()
        yellow_rel = yellow_up_total / yellow_down_total
        print(f'Naive method blue relative luminosity = {blue_rel}')
        print(f'Naive method yellow relative luminosity = {yellow_rel}')

        blue_runbyrun_total = self.df['BlueLumiUp'] + self.df['BlueLumiDown']
        blue_runbyrun_rel = self.df['BlueLumiUp'] / self.df['BlueLumiDown']
        yellow_runbyrun_total = self.df['YellowLumiUp'] + self.df['YellowLumiDown']
        yellow_runbyrun_rel = self.df['YellowLumiUp'] / self.df['YellowLumiDown']
        blue_rel = ((blue_runbyrun_total * blue_runbyrun_rel).sum()) / (blue_runbyrun_total.sum())
        yellow_rel = ((yellow_runbyrun_total * yellow_runbyrun_rel).sum()) / (yellow_runbyrun_total.sum())
        print(f'Correct method blue relative luminosity = {blue_rel}')
        print(f'Correct method yellow relative luminosity = {yellow_rel}')

    def GetPol(self):
        blue_runbyrun_total = self.df['BlueLumiUp'] + self.df['BlueLumiDown']
        blue_runbyrun_pol = self.df['BluePol']
        yellow_runbyrun_total = self.df['YellowLumiUp'] + self.df['YellowLumiDown']
        yellow_runbyrun_pol = self.df['YellowPol']
        blue_pol = ((blue_runbyrun_total * blue_runbyrun_pol).sum()) / (blue_runbyrun_total.sum())
        yellow_pol = ((yellow_runbyrun_total * yellow_runbyrun_pol).sum()) / (yellow_runbyrun_total.sum())
        print(f'Correct method blue polarization = {blue_pol}')
        print(f'Correct method yellow polarization = {yellow_pol}')

    def PlotRunTimes(self, outprefix='python_plots/'):
        x = np.array(self.runtimes)
        fig, ax = plt.subplots(nrows=1, ncols=1, figsize=(18,12))
        ax.hist(x, 100)
        ax.set_title('neutralMesonTSSA Job Run Times')
        ax.set_xlabel('Run Time (min)')
        ax.set_ylabel('Counts')
        plt.savefig(outprefix + 'runtimes.png')

    def PlotTSSATimes(self, outprefix='python_plots/'):
        tssa_runtimes = self.ScanTSSALogs()
        x = np.array(tssa_runtimes)
        fig, ax = plt.subplots(nrows=1, ncols=1, figsize=(18,12))
        ax.hist(x, 20)
        ax.set_title('TSSAhistmaker Job Run Times')
        ax.set_xlabel('Run Time (sec)')
        ax.set_ylabel('Counts')
        plt.savefig(outprefix + 'tssa_runtimes.png')

    def PlotPol(self, outprefix='python_plots/pol_'):
        x = self.df['RunNum']
        bpol = self.df['BluePol']
        ypol = self.df['YellowPol']

        fig, (ax1, ax2, ax3) = plt.subplots(nrows=1, ncols=3, figsize=(18,6))
        ax1.scatter(x, bpol, color='tab:blue', s=2.0)
        ax1.set_xlabel('Run Number')
        ax1.tick_params(axis='x', labelrotation=60)
        # ax1.ticklabel_format(useMathText=True)
        ax1.set_ylabel('Blue Beam Polarization (%)')

        ax2.scatter(x, ypol, color='orange', s=2.0)
        ax2.set_xlabel('Run Number')
        ax2.tick_params(axis='x', labelrotation=60)
        ax2.set_ylabel('Yellow Beam Polarization (%)')

        ax3.scatter(x, bpol, color='tab:blue', s=2.0, label='Blue Beam')
        ax3.scatter(x, ypol, color='orange', s=2.0, label='Yellow Beam')
        ax3.set_xlabel('Run Number')
        ax3.tick_params(axis='x', labelrotation=60)
        ax3.set_ylabel('Beam Polarization (%)')

        fig.suptitle('Beam Polarization Across Runs')
        fig.legend()
        plt.savefig(outprefix + 'runbyrun.png')
        # plt.show()


        fig, (ax1, ax2) = plt.subplots(nrows=1, ncols=2, figsize=(12,6))

        ax1.hist(bpol, bins=20, color='tab:blue', label='Blue Beam')
        ax1.set_xlabel('Blue Beam Polarization (%)')
        ax1.set_ylabel('Counts')
        #    ax1.ticklabel_format(useMathText=True)
        ax2.hist(ypol, bins=20, color='orange', label='Yellow Beam')
        ax2.set_xlabel('Yellow Beam Polarization (%)')
        ax2.set_ylabel('Counts')
        fig.suptitle('Beam Polarization Distributions')
        fig.legend()
        plt.savefig(outprefix + 'dist.png')
        # plt.show()

    def PlotRel(self, outprefix='python_plots/rel_'):
        x = self.df['RunNum']
        brel = self.df['BlueLumiUp']/self.df['BlueLumiDown']
        yrel = self.df['YellowLumiUp']/self.df['YellowLumiDown']

        fig, (ax1, ax2, ax3) = plt.subplots(nrows=1, ncols=3, figsize=(18,6))

        ax1.scatter(x, brel, color='tab:blue', s=2.0)
        ax1.set_xlabel('Run Number')
        ax1.tick_params(axis='x', labelrotation=60)
        #    ax1.ticklabel_format(useMathText=True)
        ax1.set_ylabel('Blue Beam Relative Luminosity')

        ax2.scatter(x, yrel, color='orange', s=2.0)
        ax2.set_xlabel('Run Number')
        ax2.tick_params(axis='x', labelrotation=60)
        ax2.set_ylabel('Yellow Beam Relative Luminosity')

        ax3.scatter(x, brel, color='tab:blue', s=2.0, label='Blue Beam')
        ax3.scatter(x, yrel, color='orange', s=2.0, label='Yellow Beam')
        ax3.set_xlabel('Run Number')
        ax3.tick_params(axis='x', labelrotation=60)
        ax3.set_ylabel('Relative Luminosity')

        fig.suptitle('Relative Luminosity Across Runs')
        fig.legend()
        plt.savefig(outprefix + 'runbyrun.png')
        # plt.show()


        fig, (ax1, ax2) = plt.subplots(nrows=1, ncols=2, figsize=(12,6))

        ax1.hist(brel, bins=20, color='tab:blue', label='Blue Beam')
        ax1.set_xlabel('Blue Beam Relative Luminosity')
        ax1.set_ylabel('Counts')
        #    ax1.ticklabel_format(useMathText=True)
        ax2.hist(yrel, bins=20, color='orange', label='Yellow Beam')
        ax2.set_xlabel('Yellow Beam Relative Luminosity')
        ax2.set_ylabel('Counts')
        fig.suptitle('Relative Luminosity Distributions')
        fig.legend()
        plt.savefig(outprefix + 'dist.png')
        # plt.show()

if __name__ == "__main__":
    rewrite_files = False
    if len(sys.argv) > 1 and sys.argv[1] == '--rewrite':
        rewrite_files = True
    lpa = LumiPolarAna()
    csvfile = 'lumipol.csv'
    csvisgood = lpa.GetDataFrame(csvfile)
    runtimefile = 'runtimes.csv'
    if rewrite_files or not csvisgood:
        lpa.ScanAllFiles()
        lpa.WriteRunTimes(runtimefile)
        lpa.MakeDataFrame()
        lpa.WriteDataFrame(csvfile)
    if not rewrite_files:
        lpa.ReadRunTimes(runtimefile)
    lpa.PlotRunTimes()
    lpa.PlotTSSATimes()
    lpa.GetRelLumi()
    lpa.GetPol()
    # lpa.PlotPol()
    # lpa.PlotRel()
