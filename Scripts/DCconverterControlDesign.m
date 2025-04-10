%Script to evaluate the performance of the boost converter

% Parameters
L = 8e-5;       % Inductance [H]
C = 3e-4;       %Capacitance [F]
fs = 10e3;      % switching frequency [Hz]
Vin_min = 40;   % minimum input voltage
Vin = 48;       % typical input voltage
Vin_max = 58.4; % maximum input voltage
Vout = 96;      % output voltage
Il = 75;      % maximum output current [A]

rl = 2.2/75^2;  % inductor resistance [Ohm]
% ron = 0.014;    % igbt on resistance [Ohm]
ron = 0.0317;    % igbt on resistance [Ohm]
rd = 0.0103;    % diode forward resistance [Ohm]
Vd = 1.55;        % diode forward voltage [V]

R = 96/225;     % load resistance [Ohm]

% Estimate efficiency
d = (0:0.01:1)';       % Duty cycle
% p = (1 + (rl + d*ron + (1-d)*rd)./((1-d).^2*R));
% e = 3*p./(1-d) - 3*Vd/Vin_min*p;

figure(1); clf
hold on
grid on

figure(2); clf
hold on
grid on

for i=1:10
    R = 10/i*96/225;
    r = (rl + d*ron + (1-d)*rd + 3*(1-d).^2*R);
    g = 3*R*(1-d)./r - 3*(1-d).^2*R*Vd/Vin_min./r;
    eff = (1-d).*g;
    
    figure(1)
    plot(d,g,'DisplayName',strcat('R=',num2str(R)))
    figure(2)
    plot(d,eff,'DisplayName',strcat('R=',num2str(R)))
end
legend
figure(1)
legend


fun = @(dl)(3*R*dl - 3*dl.^2*R*Vd/Vin_min)/(rl + (1-dl)*ron + dl*rd + 3*dl.^2*R)-2;
Dl = fsolve(fun,0.5);

D = (1-Dl);
Rp = rl+D*ron+Dl*rd;

Gd = -3*tf([L*Il,rl*Il-Dl*Vout],[L*C,Rp*C+L,Rp/R+3*Dl^2]);

figure(3)
bode(Gd)
margin(Gd)

%% Controller design

s = tf('s');
Kp = 0.0001373;
Ki = 0.51176;
Kd = 9.2084e-09;

Cd = Kp + Ki/s +Kd*s;

figure(4)
margin(Cd*Gd)