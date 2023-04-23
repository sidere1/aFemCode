clear all; close all; clc ; 

A = reshape(1:9,3,3)'
b = reshape(10:15,2,3)'

%[L, U] = lu(A)

U = [1 2 3 ; 0 5 6 ; 0 0 -21.6]
L = [1 0 0 ; 4 1 0 ; 7 2 1]
U = [1 2 3 ; 0 -3 -6 ; 0 0 0]

L*U

tic 
N = 10000;
A = rand(N);
b = rand(N, 1);
A\b;
toc 