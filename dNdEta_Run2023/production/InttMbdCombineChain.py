from optparse import OptionParser
import time
import os
import pwd
import sys
import re
import config_combinechain as config

def replacetext(filename, search_text, replace_text):
    with open(filename, 'r+') as f:
        file = f.read()
        file = re.sub(search_text, replace_text, file)
        f.seek(0)
        f.write(file)
        f.truncate()
        

if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -h]")
    parser.add_option("-i", "--inttdst", action="store_true", default=False, help="Run INTT DST production")
    parser.add_option("-n", "--inttntuple", action="store_true", default=False, help="Run INTT ntuple production") 
    parser.add_option("-m", "--centmbntuple", action="store_true", default=False, help="Run Centrality&MB ntuple production")
    parser.add_option("-c", "--runcombiner", action="store_true", default=False, help="Run INTT event combiner")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))

    #! Run one step at a time
    if len(args) > 1:
        parser.error("More than 1 steps to be run. Please run one step at a time")
        sys.exit(1)
        
    inttdst = opt.inttdst
    inttntuple = opt.inttntuple
    centmbntuple = opt.centmbntuple
    runcombiner = opt.runcombiner
    
    # get the current directory -> set as the analysisdir
    analysisdir = os.getcwd()
    print('analysisdir: {}'.format(analysisdir))

    username = pwd.getpwuid(os.getuid())[0]
    softwarebasedir = '/sphenix/user/{}/software'.format(username)
    macrodir = '/sphenix/user/{}/software/macros'.format(username)
    # analysisdir = '/sphenix/user/{}/software/analysis'.format(username)
    macrorepo = 'https://github.com/sPHENIX-Collaboration/macros.git'
    # analysisrepo = 'https://github.com/sPHENIX-Collaboration/analysis.git'

    os.makedirs(softwarebasedir, exist_ok=True)

    print (config.runnumber)

    if inttdst:
        print('Run INTT DST production')
        if not os.path.exists(macrodir):
            print('Directory {} does not exist'.format(macrodir))
            os.system('cd {} && git clone {}'.format(softwarebasedir, macrorepo))
        else:
            print('Directory {} exists. git pull to update'.format(macrodir))
            os.system('cd {} && git pull'.format(macrodir))
            
        os.chdir('{}/InttProduction'.format(macrodir))
        print(os.getcwd())
        
        if config.runTrkrHits:
            replacetext('Fun4All_Intt_Combiner.C', 'bool runTrkrHits = false', 'bool runTrkrHits = true')
        if config.runTkrkClus:
            replacetext('Fun4All_Intt_Combiner.C', 'bool runTkrkClus = false', 'bool runTkrkClus = true')
        if config.stripRawHit:
            replacetext('Fun4All_Intt_Combiner.C', 'bool stripRawHit = false', 'bool stripRawHit = true')
        
        cmdlist = ['chmod 755 intt_makelist.sh',
                   'intt_makelist.sh {}'.format(config.runnumber), 
                   'root -l -b -q Fun4All_Intt_Combiner.C'] 

        cmdstr = ' && '.join(cmdlist)
        os.system(cmdstr)
        
        print ('Done INTT DST production')
        os.makedirs('{}/production'.format(analysisdir), exist_ok=True)
        os.system('cp intt-{:08d}.root {}/production/'.format(config.runnumber,analysisdir))


    if inttntuple:
        print('Run INTT ntuple production')
        # Check if the intt dst exists in the production directory
        if not os.path.isfile('./production/intt-{:08d}.root'.format(int(config.runnumber))):
            # try to copy from /gpfs/mnt/gpfs02/sphenix/user/cdean/software/macros/InttProduction/intt-00020869.root , otherwise exit
            if not os.path.isfile('/gpfs/mnt/gpfs02/sphenix/user/cdean/software/macros/InttProduction/intt-{:08d}.root'.format(int(config.runnumber))):
                print('Intt DST does not exist. Exit')
                sys.exit(1)
            else:
                print('Pre-generated Intt DST exists -> Copy it to the production directory')
                os.system('cp /gpfs/mnt/gpfs02/sphenix/user/cdean/software/macros/InttProduction/intt-{:08d}.root ./production/'.format(int(config.runnumber),analysisdir))
        
        os.chdir('./condor/')
        cmdlist = ['chmod 755 runCondor.py',
                   'python runCondor.py --data --runInttData --generator none --eventPerJob 1000 --nJob 551 --outputdir {} --softwareversion {} {}'.format(config.InttNtupleDir, config.softwareversion, '--submitcondor' if config.submitcondor_InttNtupleProduction else '')]
        cmdstr = ' && '.join(cmdlist)
        os.system(cmdstr)
        

    if centmbntuple:
        print('Run Centrality&MB ntuple production')
        os.system('CreateDstList.pl --run 20869 --build ana403 --cdb 2023p011 DST_CALO')
        os.chdir('./condor/')
        cmdlist = ['chmod 755 runCondor.py',
                   'python runCondor.py --data --generator none --eventPerJob -1 --nJob 1 --outputdir {} --softwareversion {} {}'.format(config.CentralityNtupleDir, config.softwareversion, '--submitcondor' if config.submitcondor_CentralityNtupleProduction else '')]
        cmdstr = ' && '.join(cmdlist)
        os.system(cmdstr)

    if runcombiner:
        print('Run INTT event combiner')
        os.chdir('{}/production/{}'.format(analysisdir,config.InttNtupleDir))
        if not os.path.isfile('ntuple_merged.root'):
            os.system('hadd -f ntuple_merged.root ntuple_*.root')
        
        os.chdir('{}/production/{}'.format(analysisdir,config.CentralityNtupleDir))
        if not os.path.isfile('ntuple_merged.root'):
            os.system('hadd -f ntuple_merged.root ntuple_*.root')
            
        # cd to two directories up
        os.chdir('{}/INTT_MBD_evt_combiner'.format(os.path.abspath(os.path.join(analysisdir, os.path.pardir, os.path.pardir))))
        print(os.getcwd())
        os.system('python intt_mbd_evt_combiner_v1.py {}/production/{} {}/production/{}/ntuple_merged.root EventTree {}/production/{}/ntuple_merged.root EventTree'.format(analysisdir, 
                                                                                                                                                                           config.combinedNtupleName, 
                                                                                                                                                                           analysisdir,
                                                                                                                                                                           config.InttNtupleDir,
                                                                                                                                                                           analysisdir,
                                                                                                                                                                           config.CentralityNtupleDir)) 
        
            
        

        