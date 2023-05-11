function Chi2 = object_function(x, DataSet, SigmaRej)

N_Runs = size(DataSet , 2);
Ndata = 20;

Chi2 = 0;

% disp('object_function');
% disp(size(x));

calib_const = x(1:Ndata);
E_scale = x((Ndata+1):(Ndata + N_Runs-1));
E_scale = [E_scale 1]; % fix highest energy point

for i = 1:N_Runs
    
    data =  DataSet(i).data;
    
    data = data * calib_const';
    
    total_E = sum(data, 2);

    AChi2 = abs((total_E - DataSet(i).E.*E_scale(i))./DataSet(i).DE);
    AChi2 = AChi2(AChi2<SigmaRej);
    DChi2 = sum(AChi2.^2);
    
%     disp(DChi2);
    Chi2 = Chi2 + DChi2;
end

end