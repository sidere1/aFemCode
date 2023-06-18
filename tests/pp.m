clear all; close all; clc; 


filename = "compEigenVsfMatrix";

x = dlmread(filename);
x = x(2:end, :);
disp(x);
n = x(:,1);


figure(1)
hold on; 

loglog((n), (x(:, 2)), '*-', 'DisplayName', 'fullPivLU');

loglog((n), (x(:, 2)), '*-', 'DisplayName', 'fullPivLU');
loglog((n), (x(:, 3)), '*-','DisplayName', 'partPivLU');
loglog((n), (x(:, 4)), '*-','DisplayName', 'QR');
loglog((n), (x(:, 5)), '*-','DisplayName', 'LDLt');
loglog((n), (x(:, 6)), '*-','DisplayName', 'fMatrix');
xlabel("matrix size");
ylabel("duration");
legend('show')
