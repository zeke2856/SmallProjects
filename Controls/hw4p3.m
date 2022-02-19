sys = tf([0 0 1],[1 1 0]);

t = 0:.01:3;
figure(1); %Twistrod Length vs. Diameter
step(sys,t);
s = stepinfo(sys);
title('Step Response');
xlabel('amplitude');
ylabel('time (seconds)');

figure(2);
impulse([1], [1 1 0], t);

figure(3);
t=0:0.1:3;
alpha=1;
ramp=alpha*t;        
model=tf([1],[1 1 0]);   
[y,t]=lsim(model,ramp,t);
plot(t,y)
title('unit ramp respones');
xlabel('amplitude');
ylabel('time (seconds)');