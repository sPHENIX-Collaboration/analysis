Data
CreateDstList.pl --build ana391 --cdb 2023p006 DST_CALO --list <run-list>
CreateDstList.pl --build ana395 --cdb 2023p007 DST_CALO --list <run-list>
CreateDstList.pl --build ana399 --cdb 2023p008 DST_CALO --list <run-list>
CreateDstList.pl --build ana402 --cdb 2023p009 DST_CALO --list <run-list>
CreateDstList.pl --build ana403 --cdb 2023p010 DST_CALO --list <run-list>
CreateDstList.pl --build ana403 --cdb 2023p011 DST_CALO --list <run-list>
CreateDstList.pl --build ana407 --cdb 2023p013 DST_CALO --list <run-list>
CreateDstList.pl --build ana410 --cdb 2023p014 DST_CALO_run1auau --list <run-list>
CreateDstList.pl --build ana412 --cdb 2023p015 DST_CALO_run1auau --list <run-list>

MC
CreateFileList.pl -nopileup -type 4 -run 10 DST_CALO_CLUSTER DST_GLOBAL DST_MBD_EPD G4Hits

Check to see what runs are in a particular production:
ls /sphenix/lustre01/sphnxpro/commissioning/DST_anaxxx_2023pxxx | cut -d- -f2 | uniq
