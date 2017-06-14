% Plot the convergance rates for the first-order in space central scheme
% with Euler time stepping for the Soliton solution using Jordan model

clear all;
close all;
format long;

% load results
A = load('/home/jordan/Documents/PhD/project/data/postprocessing/ChrisDatn/solen/o1/E.dat');
B = load('/home/jordan/Documents/PhD/project/data/postprocessing/ChrisDatn/solen/o1/u.dat');
C = load('/home/jordan/Documents/PhD/project/data/postprocessing/ChrisDatn/solen/o1/h.dat');

n = max(size(A));

dx = A(:,1);
H_initial = A(:,2);
H_solution = B(:,2);
h = C(:,2);
u = B(:,2);

figure(1)
central = h(1:n);
x = dx(1:n);
loglog(x,central,'^r');
hold on
loglog([.001 .01 .01 .001],[0.6*10^-8 0.6*10^-8 0.6*10^-6 0.6*10^-8]);
text(0.003,2*10^-9,'1');
text(0.012,.5*10^-7,'2');
central = u(1:n);
loglog(x,central,'sr');
%axis([10^-4 10 10^-5 10]);
% str1(1) = {'Error norms using the first-order central scheme dambreak problem'};
% str1(2) = {'water depth and velocity t = 20s'};
% title(str1)
xlabel('$Log_{10}\Delta x$');
ylabel('$Log_{10}L_1$');
%LEGEND('Water Depth','First-order','Velocity',4)

figure(2)
central = H_initial(1:n);
loglog(x,central,'^r');
hold on
loglog([.001 .01 .01 .001],[0.4*10^-10 .4*10^-10 .4*10^-7 .4*10^-10]);
text(0.012,1*10^-9,'3');
text(0.003,1*10^-11,'1');
central = H_solution(1:n);
loglog(x,central,'sr');
%axis([10^-4 10 10^-5 10]);
xlabel('$Log_{10}\Delta x?????$');
ylabel('$Log_{10}L_1$');

set (0,'defaulttextinterpreter','none')

laprint
