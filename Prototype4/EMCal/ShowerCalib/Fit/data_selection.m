function Chi2 = data_selection(x, SigmaRej)

global Ndata;
global DataSet;

N_Runs = size(DataSet , 2);

calib_const = x(1:Ndata);
% E_scale = x((Ndata+1):(Ndata + N_Runs));
E_scale = ones(N_Runs);

for i = 1:N_Runs
    
    data =  DataSet(i).data;
    
    data = data * (calib_const');
    
    total_E = sum(data, 2);
    
    AChi2 = abs((total_E - DataSet(i).E.*E_scale(i))./(DataSet(i).DE * DataSet(i).E.*E_scale(i)));
    
%     disp(size(AChi2));
    
    DataSet(i).accept = AChi2<SigmaRej;
    
%     disp(size( DataSet(i).accept));
    
    fprintf('data_selection - %s / %.1f GeV - accept %d/%d events (%.3f)\n',...
        DataSet(i).FileID, DataSet(i).E,sum(DataSet(i).accept),size(DataSet(i).accept, 1),sum(DataSet(i).accept)/size(DataSet(i).accept, 1));

end

end