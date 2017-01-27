function Chi2 = object_function(x)

global Ndata;
global DataSet;
global low_tower_IDs;

N_Runs = size(DataSet , 2);
Chi2 = 0;
Nevent = 0;
% disp('object_function');
% disp(size(x));

calib_const = x(1:Ndata);
E_scale = x((Ndata+1):(Ndata + N_Runs));
E_scale_low_tower = mean(calib_const);

% E_scale = [E_scale N_Runs - sum(E_scale)]; % fix mean energy point
% % E_scale = x((Ndata+1):(Ndata + N_Runs-1));
% E_scale = ones(N_Runs); % fix mean energy point


for i = 1:N_Runs
    
    data =  DataSet(i).data;
    
    %     disp(size(data));
    %     disp(size(calib_const));
    data = data * (calib_const');
    
    total_E = sum(data, 2);
    
    AChi2 = abs((total_E - DataSet(i).E.*E_scale(i))./(DataSet(i).DE * DataSet(i).E.*E_scale(i)));
    %     AChi2 = AChi2(AChi2<SigmaRej);
    
    %     disp(size(AChi2));
    %     disp(size(DataSet(i).accept));
    AChi2 = AChi2.* DataSet(i).accept;
    
    DChi2 = sum(AChi2.^2);
    
    %     disp(DChi2);
    Nevent = Nevent + sum(AChi2.* DataSet(i).accept);
    Chi2 = Chi2 + DChi2;
end

Chi2 =  Chi2 + ...
    (sum(E_scale) - N_Runs)^2 * Nevent  * 10;

calibration_diff_lt = (calib_const - E_scale_low_tower).*low_tower_IDs;

Chi2 =  Chi2 + ...
    sum(calibration_diff_lt.^2) ...
    * Nevent * 10;

end