import os
import pwd

username = pwd.getpwuid(os.getuid())[0]
softwarebasedir = '/sphenix/user/{}/software'.format(username)
productiondir = os.path.dirname(os.path.abspath(__file__))
dndetamacrodir = '{}/macros'.format(os.path.abspath(os.path.join(productiondir, os.path.pardir)))
macrodir = '{}/macros'.format(softwarebasedir,username)
macrorepo = 'https://github.com/sPHENIX-Collaboration/macros.git'

runnumber = 20869

inttdstproduction_InttUnpacker_nEvt = -1
inttdstproduction_runTrkrHits = True
inttdstproduction_runTkrkClus = True
inttdstproduction_stripRawHit = True

inttntupleproduction_productionTag = '2023p011'
inttntupleproduction_InttNtupleDir = 'Data_NtupleIntt_Run{}'.format(runnumber)
inttntupleproduction_eventPerJob = 1000
inttntupleproduction_nJob = 551
inttntupleproduction_softwareversion = 'ana.405'
inttntupleproduction_submitcondor = False

centntupleproduction_softwareversion = 'ana.403'
centntupleproduction_productionTag = '2023p011'
centntupleproduction_CentralityNtupleDir = 'Data_NtupleCentrality_Run{}'.format(runnumber)
centntupleproduction_eventPerJob = -1
centntupleproduction_nJob = 1
centntupleproduction_submitcondor = True

inttmbdcombine_combinedNtupleName = 'Data_CombinedNtuple_Run{}.root'.format(runnumber)


