function DrawDataSet(DataSet, calibconst, name)

    N_Runs = size(DataSet , 2);

    figure('name',['DrawDataSet_' name],'PaperPositionMode','auto', ...
        'position',[100,0,1800,1000]) ;
    
    for i = 1:N_Runs
        subplot(3,2,i);

        total_E = sum( DataSet(i).data* calibconst', 2) ;

    %     hist(total_E, 0:.1:25);
        histfit(total_E, 50,'normal');

    end

end