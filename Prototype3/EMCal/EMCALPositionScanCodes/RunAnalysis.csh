mkdir Plotfiles 

root -b -l -q PlotEnergy.C

cd Plotfiles

hadd ACX.root ACX*

root -b -l -q MakePlots.C

cd ..

root -b -l -q MakeFile.C

root -b -l -q ReadHis3.root

root -b -l -q Interpolation0Degree.C


