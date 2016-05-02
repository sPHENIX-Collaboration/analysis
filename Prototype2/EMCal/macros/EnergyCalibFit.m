
clear all
close all

%%

%   const double Es[] =
%     { 2,   3  ,  4,    8,   12 ,   16 };
%   const double runs[] =
%     { 2042,2040, 2039, 2038, 2067, 2063 };

RunList = [
    2,   3  ,  4,    8,   12 ,   16
    2042,2040, 2039, 2038, 2067, 2063
];
N_Runs = size(RunList, 2);

sim_const = 2.409/100;
sim_stat = 11.77/100;
Ndata = 20;

%% Readin
% global DataSet
DataSet =struct('run',{},'E',{},'DE',{},'data',{});

for i = 1:N_Runs
    
    DataSet(i).run = RunList(2,i);
    DataSet(i).E = RunList(1,i);
    DataSet(i).DE = sqrt( sim_const.^2 + sim_stat.^2./DataSet(i).E  )  *  DataSet(i).E;
    
    filename = sprintf('beam_0000%d-0000_DSTReader.root_DrawPrototype2EMCalTower_Prototype2_DSTReader.dat', DataSet(i).run );
    
    fprintf('processing %s\n', filename);
    
    data = textread(filename);
    data = data(:,1:Ndata);
    
    total_E = sum(data, 2);
    data = data(total_E>(DataSet(i).E * 0.5) & total_E<(DataSet(i).E * 1.5) , :);
    
    DataSet(i).data =  data;
    
end

%%
DrawDataSet(DataSet, ones(1, Ndata), 'Inputs');

SaveCanvas(['EnergyCalibFIt'],gcf);
%%

disp(object_function(ones(1, Ndata + N_Runs -1),DataSet, 10));
disp(object_function(ones(1, Ndata + N_Runs -1),DataSet, 2));
disp(object_function(ones(1, Ndata + N_Runs -1),DataSet, 1));

%%

options = optimset('Display','iter','TolFun',10000, 'MaxFunEvals', 100000,'MaxIter',100000,'PlotFcns',@optimplotfval );

x = fminsearch(@(x) object_function(x,DataSet, 10),ones(1, Ndata + N_Runs -1),...
    options);

%%
options = optimset('Display','iter','TolFun',1, 'MaxFunEvals', 100000,'MaxIter',100000,'PlotFcns',@optimplotfval );

x = fminsearch(@(x) object_function(x,DataSet,3),x,...
    options);

%%
options = optimset('Display','iter','TolFun',1e-4, 'MaxFunEvals', 100000,'MaxIter',100000,'PlotFcns',@optimplotfval );

x = fminsearch(@(x) object_function(x,DataSet,2),x,...
    options);

%% 

calib_const = abs(x(1:Ndata));

DrawDataSet(DataSet,calib_const,'Optimized');
SaveCanvas(['EnergyCalibFIt'],gcf);

%% Save out

for i = 1:N_Runs
    filename = sprintf('calirbated_%d.dat', DataSet(i).run );

    
    total_E = sum( DataSet(i).data, 2) ;
    calib_total_E = sum( DataSet(i).data* calib_const', 2) ;
    
    dlmwrite(filename,[total_E calib_total_E]);
end

save('fit.mat');
% save('goodfit.mat');
