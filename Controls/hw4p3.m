t = 0:.02:10;
figure(1); %Twistrod Length vs. Diameter
step([0 0 10],[1 2 10], t);
% title('Step Response');
% xlabel('amplitude');
% ylabel('time (seconds)');

figure(2);
%impulse(10, [1 2 10 0], t);


figure(3);
t=0:0.1:10;
alpha=1;
ramp=alpha*t;        
model=tf(10,[1 2 10]);   
[y,t]=lsim(model,ramp,t);
plot(t,y)
title('unit ramp respones');
xlabel('amplitude');
ylabel('time (seconds)');