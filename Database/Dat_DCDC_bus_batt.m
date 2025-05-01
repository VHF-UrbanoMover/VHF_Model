%Database of parameters of the DC Converter

DCDC = struct();

% Parameters
DCDC.L = 8e-5;       % Inductance [H]
DCDC.C1 = 3e-4;      % Capacitance [F]
DCDC.fs = 10e3;      % switching frequency [Hz]
DCDC.Vin_min = 40;   % minimum input voltage
DCDC.Vin = 48;       % typical input voltage
DCDC.Vin_max = 58.4; % maximum input voltage
DCDC.Vout = 96;      % output voltage
DCDC.Imax = 75;      % maximum output current [A]

DCDC.rl = 2.2/75^2;  % inductor resistance [Ohm]
DCDC.rc1 = 0.01;      % capacitor resistance [Ohm]
DCDC.rc2 = 0.01;      % capacitor resistance [Ohm]
% ron = 0.014;    % igbt on resistance [Ohm]
DCDC.ron = 0.0317;    % igbt on resistance [Ohm]
DCDC.rd = 0.0103;    % diode forward resistance [Ohm]
DCDC.Vd = 1.55;        % diode forward voltage [V]
DCDC.rPre = 100;     % pre charge resistance [Ohm]

% Parameters for the DC converter of generator