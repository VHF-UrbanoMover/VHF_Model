%Database of parameters of the DC Converter

% Parameters
L = 8e-5;       % Inductance [H]
C = 3e-4;       %Capacitance [F]
fs = 10e3;      % switching frequency [Hz]
Vin_min = 40;   % minimum input voltage
Vin = 48;       % typical input voltage
Vin_max = 58.4; % maximum input voltage
Vout = 96;      % output voltage
Imax = 75;      % maximum output current [A]

rl = 2.2/75^2;  % inductor resistance [Ohm]
rc = 0.01;      % capacitor resistance [Ohm]
% ron = 0.014;    % igbt on resistance [Ohm]
ron = 0.0317;    % igbt on resistance [Ohm]
rd = 0.0103;    % diode forward resistance [Ohm]
Vd = 1.55;        % diode forward voltage [V]