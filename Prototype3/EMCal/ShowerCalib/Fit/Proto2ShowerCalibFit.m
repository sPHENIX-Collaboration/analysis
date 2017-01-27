
clear all
close all

global Ndata;
global DataSet;
global low_tower_IDs;

%%

DataFolder = 'E:/tmp/ShowerCalib/';

% FileID = {'Rot45','THP','UIUC18','UpTilt5', 'ShowerDepth'};
FileID = {'Rot45','THP','UIUC18','UIUC21','Tilt0', 'ShowerDepth'};
FileList = FileID;

sim_const = 3/100;
sim_stat = 12/100;
Ndata = 64;
InitConst = [ones(1, 64) ];

zero_sup = -1000; %zero suppression disabled

%%

for i =  1:size(FileList,2)
    
    FileList{i} = [DataFolder FileList{i} '.lst_EMCalCalib.root.dat'];
    
end

%%

% N_Runs = size(RunList, 2);

%% Readin
% global DataSet
DataSet =struct('FileID',{},'E',{},'DE',{},'data',{}, 'accept',{});

N_Runs = 0;
% for i = 1:1
for i = 1:size(FileList,2)
    filename = FileList{i};
    fprintf('processing %s\n', filename);
    
    data = textread(filename);
    %     disp(size(data));
    
    energys = data(:,1);
    energy = unique(energys);
    data = data(:,2:(1+Ndata));
    
    energy = energy(energy<=8 & energy>2);
    
    for j=1:size(energy, 1)
        
        N_Runs = N_Runs +1;
        fprintf('processing %s - %.0f GeV @ %d\n', filename,energy(j), N_Runs );
        
        DataSet(N_Runs).FileID =  FileID{i};
        DataSet(N_Runs).E = energy(j);
        DataSet(N_Runs).DE = sqrt( sim_const.^2 + sim_stat.^2./DataSet(i).E  )  ;
        
        DataSet(N_Runs).data = data(energys == energy(j),:);
        DataSet(N_Runs).data = DataSet(N_Runs).data .* (DataSet(N_Runs).data>zero_sup);
        
        total_E = sum( DataSet(N_Runs).data* InitConst', 2) ;
        
        DataSet(N_Runs).data = DataSet(N_Runs).data(total_E > DataSet(N_Runs).E/2, :);
        DataSet(N_Runs).accept = ones(size(data, 1), 1);
        
    end
    
end

%%
DrawDataSet(DataSet, InitConst, 'Inputs');

SaveCanvas([DataFolder 'EnergyCalibFIt'],gcf);



%%

figure('name',['DrawDataSet_EnergyFraction'],'PaperPositionMode','auto', ...
    'position',[100,0,1800,1000]) ;

SumFraction = [];

for i = 1:N_Runs
    subplot(4,4,i);
    total_E = ones(size(DataSet(i).data)) .*DataSet(i).E;
    Fraction = DataSet(i).data ./ total_E;
    MeanFraction = mean(Fraction, 1);
    MeanFraction = reshape(MeanFraction, 8, 8);
    
    SumFraction = [SumFraction; Fraction];
    
    imagesc(0:7, 0:7, MeanFraction);
    colorbar
    set(gca,'YDir','normal')
    
    title(sprintf('%s, E = %.1f GeV', DataSet(i).FileID, DataSet(i).E));
    xlabel('Column ID');
    ylabel('Row ID');
end

SaveCanvas([DataFolder 'EnergyCalibFIt'],gcf);
%%

figure('name',['DrawDataSet_EnergyFractionSum'],'PaperPositionMode','auto', ...
    'position',[100,0,1900,400]) ;

subplot(1,3,1);

MeanFraction = mean(SumFraction, 1);
MeanFraction = reshape(MeanFraction, 8, 8);

imagesc(0:7, 0:7, MeanFraction);
colorbar
set(gca,'YDir','normal');

title(sprintf('Sum all data'));
xlabel('Column ID');
ylabel('Row ID');

subplot(1,3,2);

hist(reshape(MeanFraction,size(MeanFraction,1) * size(MeanFraction,2),1),100);
title('fraction of shower energy carried by each tower');
xlabel('MeanFraction');

subplot(1,3,3);

low_tower_IDs =reshape( MeanFraction<0.004, 1, Ndata);

imagesc(0:7, 0:7, reshape(low_tower_IDs, 8, 8));
colorbar
set(gca,'YDir','normal');

title(sprintf('Low hit towers'));
xlabel('Column ID');
ylabel('Row ID');

SaveCanvas([DataFolder 'EnergyCalibFIt'],gcf);

% return
%%

InitConst_RunScale = [InitConst ones(1,  N_Runs)];
% InitConst_RunScale = [InitConst ];

data_selection(InitConst_RunScale,10);
disp(object_function(InitConst_RunScale));

% disp(object_function(InitConst_RunScale, 2));
% disp(object_function(InitConst_RunScale, 1));

%%

x = InitConst_RunScale;

options = optimset('Display','iter','TolX',1, 'MaxFunEvals', 100000,'MaxIter',40000,'PlotFcns',@optimplotfval );

data_selection(x,8);
disp(object_function(x));

x = fminsearch(@(x) object_function(x), x,...
    options);

%%
options = optimset('Display','iter','TolX',1, 'MaxFunEvals', 100000,'MaxIter',40000,'PlotFcns',@optimplotfval );
%
data_selection(x,4);
disp(object_function(x));

x = fminsearch(@(x) object_function(x), x,...
    options);


%%
options = optimset('Display','iter','TolX',1, 'MaxFunEvals', 100000,'MaxIter',40000,'PlotFcns',@optimplotfval );
%
data_selection(x,2);
disp(object_function(x));

x = fminsearch(@(x) object_function(x), x,...
    options);


%%

data_selection(x,2);

calib_const = x(1:Ndata);
E_scale = x((Ndata+1):(Ndata + N_Runs));


figure('name',['CalibConst'],'PaperPositionMode','auto', ...
    'position',[100,0,1800,400]) ;

subplot(1,3,1);


plot(calib_const);
title(sprintf('Calibration constant'));
xlabel('Col * 8 + Row');
ylabel('Calibration New / Old');

subplot(1,3,2);

imagesc(0:7, 0:7, reshape(calib_const, 8, 8));
colorbar
set(gca,'YDir','normal')

title(sprintf('Calibration constant, New / Old'));
xlabel('Column ID');
ylabel('Row ID');


subplot(1,3,3);

plot(E_scale);
title(sprintf('Energy scale constant, mean = %.1f', mean(E_scale)));
xlabel('Run ID');
ylabel('Energy scale New / Old');


SaveCanvas([DataFolder 'EnergyCalibFIt'],gcf);
%%

DrawDataSet(DataSet,calib_const,'Optimized');
SaveCanvas([DataFolder 'EnergyCalibFIt'],gcf);

%% Save out
%
% for i = 1:N_Runs
%     filename = sprintf('%s/calirbated_%d_%.0f.dat',DataFolder, DataSet(i).FileID, DataSet(i).E );
%
%
%     total_E = sum( DataSet(i).data*InitConst', 2) ;
%     calib_total_E = sum( DataSet(i).data* calib_const', 2) ;
%
%     dlmwrite(filename,[total_E calib_total_E]);
% end

save([DataFolder 'fit.mat']);
% save('goodfit.mat');

%%

[calib_const_col, calib_const_row]= meshgrid(0:7,0:7);

A = [reshape(calib_const_col,1,64); reshape(calib_const_row,1,64); calib_const];

fileID = fopen([DataFolder 'ShowerCalibFit_CablibConst.dat'],'w');
% fprintf(fileID,'%d - %d \n',);

fprintf(fileID,'%d\t%d\t%f\n',A);


fclose(fileID);




