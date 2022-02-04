clear all
%% inputs
% specification of the mass spring dampers.  m[kg], c[Ns/m], k[N/m]
mcks = [1 10 6];
 
t = [0:0.05:10].'; % time vector for plot of time response
 
step_value = 9; % the magnitude of the input force [N] after step event
%% create msd instances and calc time responses
num_msds = size(mcks,1); % the number of systems
 
%initialise data store arrays
msds(num_msds,1) = msd; % initialise the object array to store instances
step_ts = cell([num_msds, 1]);
step_xs = cell([num_msds, 1]);
 
for i = 1:num_msds
    msds(i) = msd(mcks(i,1), mcks(i,2), mcks(i,3));
    %create instances of mass-spring-damper systems using 'msd' class
    [step_ts{i}, step_xs{i}] = Step(msds(i),t, step_value);
    %calculate the response to step input f = step_value [N]
end
clear i
 
legend_zetas = strings(num_msds,1); % prepare string array for plot legend
zeta_values = [msds.zeta];
for i = 1:num_msds
    legend_zetas(i) = sprintf('zeta = %.2f', zeta_values(i));
end
clear zeta_values
 
%% plots
figure(1), clf, hold on
plot([msds.s], 'x', 'MarkerSize', 8)
%legend(legend_zetas, 'Location', 'SouthEast')
FormatPlot()
xlim([0 10]), ylim([-5 5])
axis equal
title('The roots s of the characteristic equation')
xlabel('Real'), ylabel('Imaginary')
 
figure(2), clf %step response using the msd class
hold on
for i = 1:num_msds
    plot(step_ts{i}, step_xs{i})
end
clear i
%legend(legend_zetas, 'Location', 'SouthEast')
grid on
title('Step response')
xlabel('Time t [s]'), ylabel('Displacement x [m]')
FormatPlot()
 
figure(3), clf, hold on % plot step response using Control System Toolbox
opt = stepDataOptions('StepAmplitude',step_value); % alter the step amplitude
for i = 1:num_msds
    step(msds(i).TF, t, opt) % plot time response using built-in function
end
%legend(legend_zetas, 'Location', 'SouthEast')
FormatPlot()