clear all; close all; clc; 


filename = "../results/micValues";

x = dlmread(filename);
x = x(2:end, :);
%disp(x);
f = x(:,1);
nMics = size(x, 2)-1; 

figure(1)
hold on; 

for iMic = 1:nMics
    plot(f, log10(x(:, iMic+1)));
end 