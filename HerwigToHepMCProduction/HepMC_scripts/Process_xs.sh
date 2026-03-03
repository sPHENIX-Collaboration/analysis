#! /bin/bash 
python Process_xs.py MB /sphenix/tg/tg01/jets/sgross/HerwigHepMC/Herwig_MB/cross_section_data/ >> Herwig_jet_crosssections.txt
for i in 5 12 20 30 40 50; do
	python Process_xs.py jet$i /sphenix/tg/tg01/jets/sgross/HerwigHepMC/Herwig_Jet$i/cross_section_data/ >> Herwig_jet_crosssections.txt
done

