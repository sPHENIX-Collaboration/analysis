close all
clear all

% From Geant4 Material Scans on EMCal along without support and electronics, e.g. 
% 
% g4->ApplyCommand("/control/matScan/phi 1111 0 22.5 deg")
% g4->ApplyCommand("/control/matScan/theta 1111 1 70 deg")
% g4->ApplyCommand("/control/matScan/scan") ; > mat3.list


% file = 'mat2.list';

% file = 'mat.list';
% n_scan = 111;

file = 'E:\tmp\Transfer Buffer\mat3.list';
n_scan = 1111;



%%

Data = zeros(n_scan*n_scan,5);

i = 1;

fid = fopen(file);
tline = fgetl(fid);
while ischar(tline)
%     disp(tline)
    
    [A,n] = sscanf(tline,'%f');
    if n == 5 
%         disp(A);
        
        Data(i,:) =  A';
        i = i + 1;
        
    elseif (strncmpi(tline,' ave.',5))
        disp(tline)
    end
    
    
    tline = fgetl(fid);
    
end
fclose(fid);


assert(i-1 == n_scan*n_scan);
%%
Data = Data(Data(:,1)<54,:);
n_eta = size(Data,1)/n_scan;


%%

Theta = Data(:,1);
Phi_Deg = Data(:,2);
Length = Data(:,3);
x0 = Data(:,4);
lambda0 = Data(:,5);

%%

Theta = reshape(Theta,[],n_eta);
Phi_Deg = reshape(Phi_Deg,[],n_eta);
Length = reshape(Length,[],n_eta);
x0 = reshape(x0,[],n_eta);
lambda0 = reshape(lambda0,[],n_eta);

%%

Eta = -log(tan((90-Theta)/180*pi/2));
Phi_Rad = Phi_Deg/180*pi;

%%

figure('name','2D','PaperPositionMode','auto', ...
    'position',[100,0,1900,1000]) ;

subplot(1,2,1);

surf(Eta,Phi_Rad,x0);

xlabel('\eta','FontSize',18);
ylabel('\phi [rad]','FontSize',18);
zlabel('Radiatiaon Length','FontSize',18);


subplot(1,2,2);

surf(Eta,Phi_Rad,lambda0);

xlabel('\eta','FontSize',18);
ylabel('\phi [rad]','FontSize',18);
zlabel('Nuclear Interaction Length','FontSize',18);

%%

figure('name','1D-Projections','PaperPositionMode','auto', ...
    'position',[100,0,1900,1100]) ;

subplot(2,2,1);

plot(mean(Eta,1),mean(x0,1), 'LineWidth',2);

xlabel('\eta','FontSize',18);
ylabel('Radiatiaon Length','FontSize',18);
set(gca,'FontSize',16, 'XMinorTick','off', 'YMinorTick','off');
line(get(gca,'XLim'),[18,18],'Color',[.5 .5 .5]);
text(mean(get(gca,'XLim')),18,'18 X_0','VerticalAlignment','top','FontSize',18,'Color',[.5 .5 .5]);

set(gca,'YLim',[0,30]);
set(gca,'XLim',[0,1.2]);

text(mean(get(gca,'XLim')),mean(get(gca,'YLim'))*.2,...
    sprintf('sPHENIX EMCal, average %.1f X_0', mean(mean(x0))),...
    'HorizontalAlignment','center','FontSize',18);


subplot(2,2,2);

plot(mean(Phi_Rad,2),mean(x0,2), 'LineWidth',2);

xlim = get(gca,'XLim');

xlabel('\phi [rad]','FontSize',18);
ylabel('Radiatiaon Length','FontSize',18);
set(gca,'FontSize',16, 'XMinorTick','off', 'YMinorTick','off');
line((xlim),[18,18],'Color',[.5 .5 .5]);
text(mean(xlim)*1.2,18,'18 X_0','VerticalAlignment','top','FontSize',18,'Color',[.5 .5 .5]);


set(gca,'YLim',[0,30]);
set(gca,'XLim',xlim);

subplot(2,2,3);

plot(mean(Eta,1),mean(lambda0,1), 'LineWidth',2);

xlabel('\eta','FontSize',18);
ylabel('Nuclear Interaction Length','FontSize',18);
set(gca,'FontSize',16, 'XMinorTick','off', 'YMinorTick','off');

line(get(gca,'XLim'),[0.75,0.75],'Color',[.5 .5 .5]);
text(mean(get(gca,'XLim')),0.75 ,'0.75 \Lambda_0','VerticalAlignment','top','FontSize',18,'Color',[.5 .5 .5]);

set(gca,'YLim',[0,1.5]);
set(gca,'XLim',[0,1.2]);

text(mean(get(gca,'XLim')),mean(get(gca,'YLim'))*.2,...
    sprintf('sPHENIX EMCal, average %.1f \\Lambda_0', mean(mean(lambda0))),...
    'HorizontalAlignment','center','FontSize',18);

subplot(2,2,4);

plot(mean(Phi_Rad,2),mean(lambda0,2), 'LineWidth',2);

xlabel('\phi [rad]','FontSize',18);
ylabel('Nuclear Interaction Length','FontSize',18);
set(gca,'YLim',[0,1.5]);
set(gca,'XLim',xlim);
set(gca,'FontSize',16, 'XMinorTick','off', 'YMinorTick','off');

line((xlim),[.75,.75],'Color',[.5 .5 .5]);
text(mean(xlim)*1.2,0.75 ,'0.75 \Lambda_0','VerticalAlignment','top','FontSize',18,'Color',[.5 .5 .5]);


SaveCavas(file,gcf);


%%


figure('name','1D-Projections-RadiationLength','PaperPositionMode','auto', ...
    'position',[100,0,800,600]) ;


plot(mean(Eta,1),mean(x0,1), 'LineWidth',3);

xlabel('\eta','FontSize',18);
ylabel('Radiatiaon Length','FontSize',18);
set(gca,'FontSize',16, 'XMinorTick','off', 'YMinorTick','off');
line(get(gca,'XLim'),[18,18],'Color',[.5 .5 .5]);
text(mean(get(gca,'XLim')),18,'18 X_0','VerticalAlignment','top','FontSize',18,'Color',[.5 .5 .5]);

set(gca,'YLim',[0,30]);
set(gca,'XLim',[0,1.2]);

text(mean(get(gca,'XLim'))*.1,mean(get(gca,'YLim'))*.6,...
    sprintf('sPHENIX simulation\n2-D projective SPACAL EMCal\nThickness scan from vertex of (0,0,0) & averaged in \\phi\n\nAverage thickness = %.1f X_0, %.2f \\Lambda_0\nMin thickness = %.1f X_0, %.2f \\Lambda_0',...
    mean(mean(x0)),mean(mean(lambda0)),min(mean(x0,1)),min(mean(lambda0,1))),...
    'HorizontalAlignment','left','FontSize',18);


SaveCavas(file,gcf);
