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
    parser.add_option("-p", "--printparam", action="store_true", default=False, help="Print parameters in config_combinechain.py")

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
    printparam = opt.printparam
    
    os.makedirs(config.softwarebasedir, exist_ok=True)
    
    if printparam:
        for name, value in vars(config).items():
            if not name.startswith('__') and not 'module' in str(getattr(config, name)):
                print('{:50} : {}'.format(name, value))
                

    if inttdst:
        print('Run INTT DST production')
        if not os.path.exists(config.macrodir):
            print('Directory {} does not exist'.format(config.macrodir))
            os.system('cd {} && git clone {}'.format(config.softwarebasedir, config.macrorepo))
        else:
            print('Directory {} exists. git pull to update'.format(config.macrodir))
            os.system('cd {} && git pull'.format(config.macrodir))
            
        os.chdir('{}/InttProduction'.format(config.macrodir))
        
        if config.inttdstproduction_runTrkrHits:
            replacetext('Fun4All_Intt_Combiner.C', 'bool runTrkrHits = false', 'bool runTrkrHits = true')
        if config.inttdstproduction_runTkrkClus:
            replacetext('Fun4All_Intt_Combiner.C', 'bool runTkrkClus = false', 'bool runTkrkClus = true')
        if config.inttdstproduction_stripRawHit:
            replacetext('Fun4All_Intt_Combiner.C', 'bool stripRawHit = false', 'bool stripRawHit = true')
        
        cmdlist = ['chmod 755 intt_makelist.sh',
                   'intt_makelist.sh {}'.format(config.runnumber), 
                   'root -l -b -q Fun4All_Intt_Combiner.C\({}\)'.format(config.inttdstproduction_InttUnpacker_nEvt)] 

        cmdstr = ' && '.join(cmdlist)
        os.system(cmdstr)
        
        print ('Done INTT DST production, move the file to the production directory and clean up')
        os.system('rm intt*.list && mv intt-{:08d}.root {}'.format(config.runnumber,config.productiondir))


    if inttntuple:
        print('Run INTT ntuple production')
        # Check if the intt dst exists in the production directory
        if not os.path.isfile('{}/intt-{:08d}.root'.format(config.productiondir,int(config.runnumber))):
            # try to copy from /gpfs/mnt/gpfs02/sphenix/user/cdean/software/macros/InttProduction/intt-00020869.root , otherwise exit
            if not os.path.isfile('/gpfs/mnt/gpfs02/sphenix/user/cdean/software/macros/InttProduction/intt-{:08d}.root'.format(int(config.runnumber))):
                print('Intt DST does not exist. Exit')
                sys.exit(1)
            else:
                print('Pre-generated Intt DST exists -> Copy it to the production directory')
                os.system('cp /gpfs/mnt/gpfs02/sphenix/user/cdean/software/macros/InttProduction/intt-{:08d}.root {}'.format(int(config.runnumber),config.productiondir))
        
        os.chdir('{}/condor/'.format(config.dndetamacrodir))
        cmdlist = ['chmod 755 runCondor.py',
                   'python runCondor.py --data --runInttData --runnumber {} --productiontag {} --generator none --eventPerJob {} --nJob {} --outputdir {} --softwareversion {} {}'.format(config.runnumber, 
                                                                                                                                                                                          config.inttntupleproduction_productionTag,
                                                                                                                                                                                          config.inttntupleproduction_eventPerJob, 
                                                                                                                                                                                          config.inttntupleproduction_nJob, 
                                                                                                                                                                                          config.productiondir+'/'+config.inttntupleproduction_InttNtupleDir, 
                                                                                                                                                                                          config.inttntupleproduction_softwareversion, 
                                                                                                                                                                                          '--submitcondor' if config.inttntupleproduction_submitcondor else '')]
        cmdstr = ' && '.join(cmdlist)
        os.system(cmdstr)
        

    if centmbntuple:
        print('Run Centrality&Minimum-bias ntuple production')
        os.chdir('{}'.format(config.dndetamacrodir))
        os.system('CreateDstList.pl --run {} --build {} --cdb {} DST_CALO'.format(config.runnumber, config.centntupleproduction_softwareversion.replace('.', ''), config.centntupleproduction_productionTag))
        os.chdir('{}/condor/'.format(config.dndetamacrodir))
        cmdlist = ['chmod 755 runCondor.py',
                   'python runCondor.py --data --runnumber {} --productiontag {} --generator none --eventPerJob {} --nJob {} --outputdir {} --softwareversion {} {}'.format(config.runnumber,
                                                                                                                                                                            config.centntupleproduction_productionTag,
                                                                                                                                                                            config.centntupleproduction_eventPerJob,
                                                                                                                                                                            config.centntupleproduction_nJob,
                                                                                                                                                                            config.productiondir+'/'+config.centntupleproduction_CentralityNtupleDir,
                                                                                                                                                                            config.centntupleproduction_softwareversion, 
                                                                                                                                                                            '--submitcondor' if config.centntupleproduction_submitcondor else '')]
        cmdstr = ' && '.join(cmdlist)
        os.system(cmdstr)


    if runcombiner:
        print('Run INTT event combiner')
        os.chdir('{}/{}'.format(config.productiondir,config.inttntupleproduction_InttNtupleDir))
        if not os.path.isfile('ntuple_merged.root'):
            os.system('hadd -f ntuple_merged.root ntuple_*.root')
            
        # cd to two directories up
        os.chdir('{}/INTT_MBD_evt_combiner'.format(os.path.abspath(os.path.join(config.productiondir, os.path.pardir, os.path.pardir))))
        os.system('python intt_mbd_evt_combiner_v1.py {}/{} {}/{}/ntuple_merged.root EventTree {}/{}/ntuple_00000.root EventTree'.format(config.productiondir,
                                                                                                                                         config.inttmbdcombine_combinedNtupleName,
                                                                                                                                         config.productiondir,
                                                                                                                                         config.inttntupleproduction_InttNtupleDir,
                                                                                                                                         config.productiondir,
                                                                                                                                         config.centntupleproduction_CentralityNtupleDir)) 
        
            
        

        