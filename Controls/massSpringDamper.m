m = 1;
k = 6;
c = 5;
F = 1;
Ts = 0.25;

A = [0 1; -k/m -c/m];
B = [0; F/m];
C = [1 0];
D = [0];

sys = ss(A,B,C,D,Ts);

clf
grid, axis([0 10 -1 1])
step(sys)

