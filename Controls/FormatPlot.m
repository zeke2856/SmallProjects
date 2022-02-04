function[]= FormatPlot()
    set(findobj(gcf,'type','line'),'LineWidth', 2);
    set(findobj(gcf,'type','axes'),'FontSize',16, 'LineWidth', 0.5);
    grid on
end