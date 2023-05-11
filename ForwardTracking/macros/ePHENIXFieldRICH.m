close all
clear all

%% RICH input

p_RICH_Ref = 10;
% n_index = 1.00137; % C4F10
% n_index = 1.00054; % CF4
n_index = 1.00062; % CF4
Theta_max = acos(1./n_index)*1000; % mrad


MaxLength = 350;
etas = [1.02:.05:1.3,1.4:.2:3, 3:.5:4]';
N_step = 1000;


%% open field map

file1 = 'DrawFieldMap_hfield_sPHENIX.2d.root.data';
% file2 = 'DrawFieldMap_hfield_fsPHENIX.2d.root.data';

fileID = fopen(file1);
data1 = fscanf(fileID,'%f%f%f%f\n', [4 Inf])';
fclose(fileID);

%% resampling

bc1 = griddata(data1(:,2),data1(:,1),data1(:,4),0,0,'natural');
data1_scale = 1.4/1.5;

[Z,R]= meshgrid(-400:2:400,0:2:300);
% [Z,R]= meshgrid(-200:2:200,0:2:200);

br1 = griddata(data1(:,2),data1(:,1),data1(:,3),Z,R,'natural') .* data1_scale;
bz1 = griddata(data1(:,2),data1(:,1),data1(:,4),Z,R,'natural') .* data1_scale;
b1 = sqrt(br1.^2 + bz1.^2);

%% RICH trajectory sampling

thetas = (2. .* atan( exp(-etas))) * ones(1, N_step);

trajectory_step = ones(size(etas)) * linspace(0,MaxLength,N_step);
step = MaxLength ./ (N_step-1); 

trajectory_r = sin(thetas) .* trajectory_step;
trajectory_z = cos(thetas) .* trajectory_step;


%% Field mapping 

trajectory_br = griddata(data1(:,2),data1(:,1),data1(:,3),trajectory_z,trajectory_r,'natural') .* data1_scale;
trajectory_bz = griddata(data1(:,2),data1(:,1),data1(:,4),trajectory_z,trajectory_r,'natural') .* data1_scale;

% bending B
trajectory_bt = trajectory_bz.*cos(thetas + pi ./ 2) + trajectory_br.*sin(thetas + pi ./ 2);
% longitudinal B
trajectory_bl = trajectory_bz.*cos(thetas) + trajectory_br.*sin(thetas);



%% angular kicks

%BdL in T*m
trajectory_bdL = 0.01 .* step.* cumsum(trajectory_bt,2);
trajectory_angle_kick = 0.3 .* trajectory_bdL ./ p_RICH_Ref;

%% ring spread

RICH_volumne_cut = IsInCurvedRich(trajectory_z, etas * ones(1, N_step));

std_RICH = zeros(size(etas));
for trajectory = 1:size(etas)
    std_RICH(trajectory) = std(trajectory_angle_kick(trajectory,RICH_volumne_cut(trajectory,:)))  ;
end


%% ring plot

ylim =  8.01e-2*Theta_max; % max mrad
font_size = 17;

m2014 = matfile('BABAR_V11_StationCuts_Z5.0cm_300.0cm.mat');

figure('name',sprintf('Curved_RICH_Dispersion'),'PaperPositionMode','auto', ...
    'position',[100,0,800,600]) ;



std_RICH_draw = std_RICH* 1e3; % rad -> mrad

hold on;



plot(m2014.etas_fine, interp1(m2014.etas, m2014.std_RICH_draw, m2014.etas_fine,'spline'),'-',...
    'LineWidth',2, 'Color',[0,0,.5]);


plot(etas, std_RICH_draw,'-',...
    'LineWidth',2, 'Color',[0,.5,.5]);
RICH_fit = polyfit(etas, std_RICH_draw,3);
std_RICH_fit = polyval(RICH_fit,etas);
% plot(etas, std_RICH_fit,'--');
disp(RICH_fit./sqrt(2));

set(gca,'XLim',[.5,4.5],'FontSize',font_size);
set(gca,'YLim',[0, ylim],'FontSize',font_size);
% title(sprintf('RICH Ring RMS Dispersion for track of p = %.1f GeV/c due to Field Bending', p_RICH_Ref ),'FontSize',15);
ylabel('RICH Ring Dispersion, \Delta\phi (mrad)','FontSize',font_size);
xlabel(['\eta'],'FontSize',font_size);

box on;

legend('2014 Concept: arXiv:1402.1209','2018 Update: sPH-cQCD-2018-001',...
    'Location','East');

% grid on;

% text(3,2,...
%     'PID error \delta R = \Delta\phi / \sqrt{2N_{\gamma}}(10 GeV/c)/p',...
%     'Interpreter','Latex'...
%     ,'FontSize',font_size...
% );
text(2.8,2.3,...
    'RICH ring error $\delta R = \Delta\phi / \sqrt{2N_{\gamma}} \times (10 GeV/c)/p$',...
    'Interpreter','Latex'...
    ,'FontSize',font_size...
    ,'HorizontalAlignment','center'...
    );


ax1 = gca;

ax2 = axes('Position',get(ax1,'Position'),...
    'XAxisLocation','bottom',...
    'YAxisLocation','right',...
    'Color','none',...
    'LineWidth',1, ...
    'YColor','b');
set(ax2,'YLim',[0,ylim/Theta_max*100],'FontSize',font_size);
set(get(ax2,'YLabel'),...
    'String','RICH Ring Dispersion, \Delta\phi (percentage of \theta_{MAX})',...
    'FontSize',font_size);
set(ax2,'XTick',[]);

SaveCavas('ePHENIXFieldRICH');



%% Geometry Check

downsample = 4;



figure('name',['DrawField'],'PaperPositionMode','auto', ...
    'position',[100,0,2000,800]) ;

% subplot(2,1,1);


contourf(Z,R,b1,0:.1:2.5,'ShowText','on')
daspect([1 1 1])
% colormap cool
hold on
quiver(Z(1:downsample:end,1:downsample:end),R(1:downsample:end,1:downsample:end),...
    bz1(1:downsample:end,1:downsample:end),br1(1:downsample:end,1:downsample:end)...
    )

caxis([0, 2.5])
box on;
grid off;


h = colorbar;
set(get(h,'title'),'string','Field Strength [T]','FontSize',18);
xlabel('Z [cm]','FontSize',20);
ylabel('R [cm]','FontSize',20);
title('Magnetic field strength and vector in sPHENIX inner detector region','FontSize',20);
set(gca,'FontSize',18)
% text(0,80,'Central Tracking Volume','VerticalAlignment','bottom','HorizontalAlignment','center','FontSize',20,'Color',[.3 .3 .3]*.1)
% text(-110,80,'e-going Tracker','VerticalAlignment','bottom','HorizontalAlignment','right','FontSize',20,'Color',[.3 .3 .3]*.1)
% text(160,80,'h-going Tracker','VerticalAlignment','bottom','HorizontalAlignment','left','FontSize',20,'Color',[.3 .3 .3]*.1)



h = rectangle('Position',[-102 0  +2*102  78-0]);
set(h,'EdgeColor',[.3 .3 .3]);
set(h,'LineWidth',4);

% E-GEM
h = line([-1.1,-1.1].*100, [.1, 0.8].*100 );
set(h,'Color',[.3 .3 .3]);
set(h,'LineWidth',4,'LineStyle','-');

h = line([-1.35,-1.35].*100, [.1, 0.8].*100 );
set(h,'Color',[.3 .3 .3]);
set(h,'LineWidth',4,'LineStyle','-');


% H-GEM
h = line([1.2,1.2].*100, [.1, 0.8].*100 );
set(h,'Color',[.3 .3 .3]);
set(h,'LineWidth',4,'LineStyle','-');

h = line([1.5,1.5].*100, [.1, 1].*100 );
set(h,'Color',[.3 .3 .3]);
set(h,'LineWidth',4,'LineStyle','-');

% h = line([2.7,2.7,2.5].*100, [.1, .8, 1.3].*100 );
% set(h,'Color',[.3 .3 .3]);
% set(h,'LineWidth',4,'LineStyle','-');

for trajectory = 1:size(etas)
    h = plot(trajectory_z(trajectory,:), trajectory_r(trajectory,:),'-','Color',[1,1,1]*.5 );
    h = plot(trajectory_z(trajectory,RICH_volumne_cut(trajectory,:)), trajectory_r(trajectory,RICH_volumne_cut(trajectory,:)),...
        '-','Color',[1,1,1]*.2,'LineWidth',2);
end

SaveCavas('ePHENIXFieldRICH');



