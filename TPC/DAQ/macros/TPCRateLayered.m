% bottom up calcualtion of TPC rate using MC approach
% Author - Jin Huang <jhuang@bnl.gov>

close all

clear all

myColorMap = colormap;
myColorMap(1,:) = [1 1 1];
colormap(myColorMap);
%% Inputs

% n = 1000;
% full_rate = 100e3;
% trig_rate = 15e3;
% trigger_window = 18e-6;

% n = 10000000;
n = 50000;
% n = 500000;
% full_rate = 250e3;
full_rate = 100e3;
% full_rate = 150e3;
% full_rate = 100e3;
% full_rate = 50e3;
trig_rate = 15e3;
% trigger_window = 4e-6;
trigger_window = 13e-6;
% trigger_window = 17.5e-6;
% trigger_window = 35e-6;

TargetEta = 1.1;
% TargetEta = 3;

nRing = 40;
% nRing = 48;

SaveName = sprintf('TPCRateLayered_%.0fHzCol_%.0fHzTrig_%.0fusDrift_nRing%d',full_rate,trig_rate,trigger_window*1e6, nRing);

%% Generic constants


minR = 30;
maxR = 78;
maxZ = 105;
dNdeta = 2 * 180 * 2; % Pre-CDR table 3.3 with effective factor x2 and two signs x2
bitPerHit =3*5*10 * 1.4;  % Pre-CDR table 3.3 with effective factor
DAMCompressionFactor = 1.02 * 0.5 * 0.6; % Repacking, clustering, compression

BCO = 10e6;
DataPlotRangeBCO =10000;

TimeSpan= n/full_rate;
TimeSpanBCO = int64(TimeSpan*BCO);


TriggerBCO  = poissrnd(trig_rate/BCO, TimeSpanBCO, 1)';
CollisionBCO =poissrnd((full_rate - trig_rate)/BCO ,TimeSpanBCO, 1)' + TriggerBCO;


%% Per triggger

PerTriggerBCO = int64(trigger_window * BCO);

RRing = repmat(linspace(minR, maxR, nRing )',1,PerTriggerBCO);
zBCO = repmat(linspace(maxZ, 0, PerTriggerBCO),nRing,1);

MinEtaRingBCO =   atanh( (zBCO - 10) ./ sqrt((zBCO - 10).^2 + RRing.^2)  );
% MinEtaRingBCO =   atanh( (zBCO - 0) ./ sqrt((zBCO - 0).^2 + RRing.^2)  );
dEtaRingBCO = atanh( (zBCO + maxZ/double(PerTriggerBCO) ) ./ sqrt((zBCO + maxZ/double(PerTriggerBCO) ).^2 + RRing.^2)  ) -  atanh( zBCO ./ sqrt(zBCO.^2 + RRing.^2)  );
dataBitRingBCO = bitPerHit .* dNdeta .* dEtaRingBCO;  % Pre-CDR table 3.3 with effective factor

TriggerMask = double(MinEtaRingBCO  <  TargetEta);
dataBitRingBCOAfterMask = dataBitRingBCO.*TriggerMask;

figure('name','PerEventData','PaperPositionMode','auto', ...
    'position',[100,0,2400,800]) ;

subplot(1,2,1)


imagesc(zBCO(1,:), RRing(:,1), dataBitRingBCO);
c = colorbar;

hold on;
plot([10 maxZ+10], [0 tan(acos(tanh(TargetEta))) * maxZ],'k--');

set(gca,'YDir','normal');
set(gca,'YLim',[0,80]);
set(gca,'XLim',[0,110])
box on
grid on
daspect([1 1 1])

xlabel('|z| position (cm)','FontSize',14);
ylabel('R position (cm)','FontSize',14);
c.Label.String = 'FEE data (bit) per MB collision per |z|-R bin (integrated over z-sign and azimuth)';
c.Label.FontSize = 14;

title(sprintf('FEE data (bit) per MB collision per |z|-R bin, total = %.1f Mbit', sum(sum(dataBitRingBCO))/1e6),'FontSize',16);


subplot(1,2,2)


imagesc(zBCO(1,:), RRing(:,1), dataBitRingBCOAfterMask);
c = colorbar;
hold on;
plot([10 maxZ+10], [0 tan(acos(tanh(TargetEta))) * maxZ],'k--');

set(gca,'YDir','normal');
set(gca,'YLim',[0,80]);
set(gca,'XLim',[0,110])
box on
grid on
daspect([1 1 1])

xlabel('|z| position (cm)','FontSize',14);
ylabel('R position','FontSize',14);
c.Label.String = 'FEE data (bit) per MB collision per |z|-R bin (integrated over z-sign and azimuth)';
c.Label.FontSize = 14;

title(sprintf('FEE data (bit) after DAM acceptance filtering, total = %.1f Mbit', sum(sum(dataBitRingBCOAfterMask))/1e6),'FontSize',16);




SaveCavas('TPCRateLayered',gcf);


%% Run the DAQ

DataBCO = conv2(CollisionBCO,dataBitRingBCO);
TriggerAcceptBCO = conv2(TriggerBCO,TriggerMask);
ThrottleAcceptBCO = double(TriggerAcceptBCO>0);

PlotDataBCO = DataBCO(:,1:DataPlotRangeBCO);
PlotThrottleDataBCO = DataBCO(:,1:DataPlotRangeBCO).*ThrottleAcceptBCO(:,1:DataPlotRangeBCO);

TotalDataRate = sum(sum(DataBCO))/TimeSpan;
TriggerDataRate = sum(sum(DataBCO.*TriggerAcceptBCO))/TimeSpan;
ThrottleDataRate = sum(sum(DataBCO.*ThrottleAcceptBCO))/TimeSpan;

%% Event stream display

figure('name','DataStream','PaperPositionMode','auto', ...
    'position',[100,0,2400,800]) ;

subplot(2,1,1)

imagesc( DataBCO(:,1:DataPlotRangeBCO) );
c = colorbar;
hold on;
set(gca,'YDir','normal');
set(gca,'YLim',[-4,nRing+1]);

xlabel('BCO bin (100 ns)','FontSize',14);
ylabel('Radial layer number','FontSize',14);
c.Label.String = 'FEE data (bit)';
c.Label.FontSize = 14;
title(sprintf('FEE data input to DAM. Rate = %.0f Gbps @ %.0f kHz Collision, %.0f kHz Trigger %.0f us Drift',...
    sum(sum(DataBCO))/TimeSpan/1e9,full_rate/1e3,trig_rate/1e3,trigger_window*1e6),'FontSize',16);

for i = 1:DataPlotRangeBCO
    
    if (TriggerBCO(i) >0)
        
        plot([i i], [-3 nRing],'r--','LineWidth',2);
        plot([i,i+PerTriggerBCO], [-3 -3], 'r-', 'LineWidth', 2)
        plot([i,i+PerTriggerBCO], [-1 -1], 'k-', 'LineWidth', 2)
        
    elseif (CollisionBCO(i) >0)
        
        plot([i i], [-1 nRing],'k--','LineWidth',2);
        plot([i,i+PerTriggerBCO], [-1 -1], 'k-', 'LineWidth', 2)
    end
    
    
end



subplot(2,1,2)

imagesc( PlotThrottleDataBCO );
c = colorbar;
hold on;
set(gca,'YDir','normal');
set(gca,'YLim',[-4,nRing+1]);

xlabel('BCO bin (100 ns)','FontSize',14);
ylabel('Radial layer number','FontSize',14);
c.Label.String = 'FEE data equavelent (bit)';
c.Label.FontSize = 14;
title(sprintf('DAM data output: Throttled data rate = %.0f Gbps, Triggered data rate = %.0f Gbps ',...
    ThrottleDataRate * DAMCompressionFactor/1e9 ,...
    TriggerDataRate * DAMCompressionFactor/1e9...
    ),'FontSize',16);

for i = 1:DataPlotRangeBCO
    
    if (TriggerBCO(i) >0)
        
        plot([i i], [-3 nRing],'r--','LineWidth',2);
        plot([i,i+PerTriggerBCO], [-3 -3], 'r-', 'LineWidth', 2)
        plot([i,i+PerTriggerBCO], [-1 -1], 'k-', 'LineWidth', 2)
        
    elseif (CollisionBCO(i) >0)
        
        plot([i i], [-1 nRing],'k--','LineWidth',2);
        plot([i,i+PerTriggerBCO], [-1 -1], 'k-', 'LineWidth', 2)
    end
    
    
end

SaveCavas(SaveName,gcf);


%% Event stream display - 1D

figure('name','DataStream1D','PaperPositionMode','auto', ...
    'position',[100,0,2400,800]) ;

subplot(2,1,1)

bar( PlotDataBCO(nRing,:) ,1);
% c = colorbar;
hold on;
set(gca,'YDir','normal');
set(gca, 'XLim',[0 DataPlotRangeBCO]);
ylim = get(gca,'YLim');
y_max = ylim(2);

xlabel('BCO bin (100 ns)','FontSize',14);
ylabel('Data in last TPC layer (bit/BCO)','FontSize',14);
% c.Label.String = 'FEE data (bit)';
% c.Label.FontSize = 14;
title(sprintf('FEE data input to DAM. Rate = %.0f Gbps @ %.0f kHz Collision, %.0f kHz Trigger %.0f us Drift',...
    sum(sum(DataBCO))/TimeSpan/1e9,full_rate/1e3,trig_rate/1e3,trigger_window*1e6),'FontSize',16);

for i = 1:DataPlotRangeBCO
    
    if (TriggerBCO(i) >0)
        
        plot([i i], [-.15 1].*y_max,'r--','LineWidth',1);
        plot([i,i+PerTriggerBCO], [-.15 -.15].*y_max, 'r-', 'LineWidth', 1)
        plot([i,i+PerTriggerBCO], [-.1 -.1].*y_max, 'k-', 'LineWidth', 1)
        
    elseif (CollisionBCO(i) >0)
        
        plot([i i], [-.1 1].*y_max,'k--','LineWidth',1);
        plot([i,i+PerTriggerBCO],  [-.1 -.1].*y_max, 'k-', 'LineWidth', 1)
    end
    
    
end



subplot(2,1,2)

bar( PlotThrottleDataBCO(nRing,:), 1 );
% c = colorbar;
hold on;
set(gca,'YDir','normal');
set(gca, 'XLim',[0 DataPlotRangeBCO]);
% set(gca,'YLim',[-4,nRing+1]);

xlabel('BCO bin (100 ns)','FontSize',14);
ylabel('Data in last TPC layer (bit/BCO)','FontSize',14);
% c.Label.String = 'FEE data equavelent (bit)';
% c.Label.FontSize = 14;
title(sprintf('DAM data output: Throttled data rate = %.0f Gbps, Triggered data rate = %.0f Gbps ',...
    ThrottleDataRate * DAMCompressionFactor/1e9 ,...
    TriggerDataRate * DAMCompressionFactor/1e9...
    ),'FontSize',16);

for i = 1:DataPlotRangeBCO
    
    if (TriggerBCO(i) >0)
        
        plot([i i], [-.15 1].*y_max,'r--','LineWidth',1);
        plot([i,i+PerTriggerBCO], [-.15 -.15].*y_max, 'r-', 'LineWidth', 1)
        plot([i,i+PerTriggerBCO], [-.1 -.1].*y_max, 'k-', 'LineWidth', 1)
        
    elseif (CollisionBCO(i) >0)
        
        plot([i i], [-.1 1].*y_max,'k--','LineWidth',1);
        plot([i,i+PerTriggerBCO],  [-.1 -.1].*y_max, 'k-', 'LineWidth', 1)
    end
    
    
end


SaveCavas(SaveName,gcf);

%% FEE Data Rate

DataRadialLayerRate = sum(DataBCO, 2) / TimeSpan;

if nRing == 40
    
    FEEDataRate = [sum( DataRadialLayerRate(1:8))/5;
        sum( DataRadialLayerRate((8+1):(8+16)))/8;
        sum( DataRadialLayerRate((8+1+16):(8+16+16)))/12]...
        /12/2;%12 sectors and two sides
elseif nRing == 48
    
    FEEDataRate = [sum( DataRadialLayerRate(1:16))/6;
        sum( DataRadialLayerRate((16+1):(16+16)))/8;
        sum( DataRadialLayerRate((16+1+16):(16+16+16)))/12]...
        /12/2;%12 sectors and two sides
    
else
    assert(0);
end


%% FEE Data Rate Plot

figure('name','FEEDataRate','PaperPositionMode','auto', ...
    'position',[100,0,800,400]) ;


bar( FEEDataRate/1e9 );
% c = colorbar;
hold on;
set(gca,'YDir','normal');
% set(gca, 'XLim',[0 DataPlotRangeBCO]);
ylim = get(gca,'YLim');
y_max = ylim(2);

xlabel('Module Number','FontSize',16);
ylabel('Average data rate per FEE (Gbps)','FontSize',16);
% c.Label.String = 'FEE data (bit)';
% c.Label.FontSize = 14;
title(sprintf('FEE data input to DAM. Total = %.0f Gbps @ %.0f kHz Collision, %.0f kHz Trigger %.0f us Drift',...
    sum(sum(DataBCO))/TimeSpan/1e9,full_rate/1e3,trig_rate/1e3,trigger_window*1e6),'FontSize',12);

SaveCavas(SaveName,gcf);

%% Collision pile up histogram

nPileUpTrig = zeros(  sum(TriggerBCO(PerTriggerBCO+1:(TimeSpanBCO-PerTriggerBCO)))  ,1)  ;
nPileUpRnd = zeros(  TimeSpanBCO - 2*PerTriggerBCO  ,1)  ;

iTrig = 1;
for i = PerTriggerBCO+1:(TimeSpanBCO-PerTriggerBCO)
    
    if (TriggerBCO(i));
        
        nPileUpTrig(iTrig) = sum( CollisionBCO((i-PerTriggerBCO):(i+PerTriggerBCO)) );
        
        iTrig = iTrig +1;
    end
    
    nPileUpRnd(i- PerTriggerBCO + 1) = sum( CollisionBCO((i-PerTriggerBCO):(i+PerTriggerBCO)) );
    
    
end
%%

figure('name','nPileUp','PaperPositionMode','auto', ...
    'position',[100,0,2000,800]) ;

subplot(1,2,1)

hist(nPileUpTrig,0:20);

set(gca,'XLim',[-1,11]);
xlabel('Number of collisions in TPC drift window','FontSize',14);
ylabel('Count per bin','FontSize',14);
title(sprintf('Collision trigger: <# collision in TPC window> = %.2f @ %.0f kHz Collision, %.0f us Drift',...
    mean(nPileUpTrig),...
    full_rate/1e3,trigger_window*1e6),'FontSize',16);

subplot(1,2,2)

hist(nPileUpRnd,0:20);

set(gca,'XLim',[-1,11]);
xlabel('Number of collisions in TPC drift window','FontSize',14);
ylabel('Count per bin','FontSize',14);
title(sprintf('Random trigger: <# collision in TPC window> = %.2f @ %.0f kHz Collision, %.0f us Drift',...
    mean(nPileUpRnd),...
    full_rate/1e3,trigger_window*1e6),'FontSize',16);

SaveCavas(SaveName,gcf);

%%

% fprintf('Throttled event / total = %.3f; Throttled data / total = %.3f; Triggered event / total = %.3f; Triggered data / total = = %.3f\n',...
%     n_recorded/n, recorded_data/n, n_triggered./n, triggered_data/n);
fprintf('------------\n');

fprintf('full_rate = %.0f kHz; trig_rate= %.0f kHz; trigger_window= %.1f us; Time span = %.3f s \n',...
    full_rate/1e3,trig_rate/1e3, trigger_window*1e6, TimeSpan );

fprintf('per event effective track = %.0f , per event FEE data = %.0f bits, total FEE data rate = %.0f Gbps \n',...
    mean(sum(dEtaRingBCO,2)*180*2), sum(sum(dataBitRingBCO)), sum(sum(DataBCO))/TimeSpan/1e9 );

fprintf('Trigger rate*drift window = %.2f;Full rate*drift window= %.2f;Trigger rate/full rate = %.2f (input)/%.2f(MC); \n',...
    trig_rate*trigger_window,full_rate*trigger_window,trig_rate/full_rate, sum(TriggerBCO)/sum(CollisionBCO)  );

fprintf('throttled data / total = %.3f; Triggered data / total = %.3f; throttled/trigger = %.3f; Triggered data/Per event data = %.3f \n',...
    ThrottleDataRate/TotalDataRate,...
    TriggerDataRate/TotalDataRate,...
    ThrottleDataRate/TriggerDataRate,...
    sum(sum(DataBCO.*TriggerAcceptBCO)) ./ (sum(sum(dataBitRingBCOAfterMask)).*sum(TriggerBCO))...
    );
%    sum(sum(DataBCO.*TriggerAcceptBCO)) ./ (sum(sum(dataBitRingBCOAfterMask)).*sum(CollisionBCO))...

fprintf('throttled data rate = %.0f Gbps; Triggered data rate = %.0f Gbps\n',...
    ThrottleDataRate * DAMCompressionFactor/1e9 ,...
    TriggerDataRate * DAMCompressionFactor/1e9);
fprintf('------------\n');

% fprintf('throttled event / total = %.3f; Triggered event / total = = %.3f;  throttled/trigger = %.3f\n',...
%     recorded_data/n, triggered_data/n, recorded_data/triggered_data );

