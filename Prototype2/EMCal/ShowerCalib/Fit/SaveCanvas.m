function prefix = SaveCanvas(savename,fig)

if nargin <= 1
    fig = gcf  ;  
end

savename = [savename '.' get(fig,'Name')];


set(fig,'Units','points')
set(fig,'paperunits','points')
pos =  get(fig,'position');
set(fig, 'PaperPosition', [0 0 pos(3) pos(4)]);
set(fig, 'PaperSize', [pos(3) pos(4)]);


print(fig, '-dpng','-r100',[savename '.png']);
% print(fig, '-dpdf',[savename '.pdf']);
% print(fig, '-depsc',[savename '.eps']);

saveas(fig, [savename '.pdf']);
% saveas(fig, [savename '.fig']);

disp(['Updated Save canvas: Saved to ' savename '.png/.eps/.fig']);

prefix = savename;

end

