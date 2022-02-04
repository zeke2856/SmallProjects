m = 1;%
k = 6;%p
c = 5;%d
A = [0 1; -k/m -c/m];
B = [0 ; 1/m];
C = [1 0];
D = [0];

% init condition
X0 = [2 -1];

%time step and step size
ts1 = 0.1;
t1 = 0:ts1:10;

%outside Force

F = ones(size(t1));
sys = ss(A,B,C,D);

%initial system parameters
initial(sys,X0);

%plotting
lsim(sys,F,t1,X0);