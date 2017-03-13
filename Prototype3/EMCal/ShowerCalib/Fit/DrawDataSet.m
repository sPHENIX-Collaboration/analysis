function DrawDataSet(DataSet, calibconst, name)

    N_Runs = size(DataSet , 2);

    figure('name',['DrawDataSet_' name],'PaperPositionMode','auto', ...
        'position',[100,0,1800,1000]) ;
    
    for i = 1:N_Runs
        subplot(1,1,i);

        total_E = sum( DataSet(i).data* calibconst', 2) ;

    %     hist(total_E, 0:.1:25);
        histfit(total_E, 50,'normal');
        [mu1, sigma1] = normfit(total_E);
        
        [mu2, sigma2] = normfit(total_E((total_E >(mu1 - 2*sigma1)) & (total_E <(mu1 + 2*sigma1))));
        
        title(sprintf('%s, E = %.1f GeV, dE/E = %.1f%%', DataSet(i).FileID, DataSet(i).E, sigma2/mu2*100 ));
        xlabel('Measrued Energy (GeV)');
        set(gca,'XLim',[DataSet(i).E/2 DataSet(i).E*1.5])
    end

end