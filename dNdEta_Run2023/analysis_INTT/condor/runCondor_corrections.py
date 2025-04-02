from optparse import OptionParser
import time
import os
import pwd
import sys
import re

def dir_empty(dir_path):
    return not any((True for _ in os.scandir(dir_path)))

if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -h]")
    parser.add_option("--corrfiletag", dest="corrfiletag", default='HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0', help="Correction file tag")
    parser.add_option("--outfilepathbaseline", dest="outfilepathbaseline", default='HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0', help="Outfile path for baseline")
    parser.add_option("--outfilepathclosure", dest="outfilepathclosure", default='HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0', help="Outfile path for closure")
    parser.add_option("--outfilepathdata", dest="outfilepathdata", default='Data_Run20869_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0', help="Outfile path for data")
    parser.add_option("--hijingbaselinedir", dest="hijingbaselinedir", default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0', help="Hijing baseline directory for tracklet minitrees")
    parser.add_option("--hijingclosuredir", dest="hijingclosuredir", default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/minitree/TrackletMinitree_Sim_Ntuple_HIJING_new_20240424/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0', help="Hijing closure directory for tracklet minitrees")
    parser.add_option("--datadir", dest="datadir", default='/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/minitree/TrackletMinitree_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0', help="Data directory for tracklet minitrees")
    parser.add_option("--submitcondor", dest="submitcondor", action="store_true", default=False, help="Submit condor jobs")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt)) 

    corrfiletag = opt.corrfiletag
    outfilepathbaseline = opt.outfilepathbaseline
    outfilepathclosure = opt.outfilepathclosure
    outfilepathdata = opt.outfilepathdata
    hijingbaselinedir = opt.hijingbaselinedir
    hijingclosuredir = opt.hijingclosuredir
    datadir = opt.datadir
    submitcondor = opt.submitcondor
    
    os.makedirs('./log_corrections/', exist_ok=True)
    # if not dir_empty('./log_corrections/'):
    #     os.system('rm ./log_corrections/*')
        
    parentdir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir))
        
    filedesc = corrfiletag.replace('HIJING_Baseline_', '')
        
    condorFileName = "submitCondor_corrections_{}.job".format(filedesc)
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe             = vanilla\n")
    condorFile.write("InitialDir           = {}\n".format(parentdir))
    condorFile.write("Executable           = $(InitialDir)/runCorrections.sh\n")
    condorFile.write("PeriodicHold         = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory       = 3GB\n")
    condorFile.write("Priority             = 20\n")
    condorFile.write("job_lease_duration   = 3600\n")
    condorFile.write("corrfiletag          = {}\n".format(corrfiletag))
    condorFile.write("outfilepath_baseline = {}\n".format(outfilepathbaseline))
    condorFile.write("outfilepath_closure  = {}\n".format(outfilepathclosure))
    condorFile.write("outfilepath_data     = {}\n".format(outfilepathdata))
    condorFile.write("hijingbaselinedir    = {}\n".format(hijingbaselinedir))
    condorFile.write("hijingclosuredir     = {}\n".format(hijingclosuredir))
    condorFile.write("datadir              = {}\n".format(datadir))
    condorFile.write("Output               = $(Initialdir)/condor/log_corrections/condorlog_{}.out\n".format(filedesc))
    condorFile.write("Error                = $(Initialdir)/condor/log_corrections/condorlog_{}.err\n".format(filedesc))
    # condorFile.write("Log                  = $(Initialdir)/condor/log_corrections/condorlog_{}.log\n".format(filedesc))
    condorFile.write("Log                = /tmp/condorlog_hjheng_corrections_{}.log\n".format(filedesc))
    condorFile.write("Arguments            = \"$(corrfiletag) $(outfilepath_baseline) $(outfilepath_closure) $(outfilepath_data) $(hijingbaselinedir) $(hijingclosuredir) $(datadir)\"\n")
    condorFile.write("Queue 1")
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)