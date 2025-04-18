%% 1. Parâmetros do Gerador Síncrono (Ligação Estrela)
P = 10;                     % Número de polos
Ra = 0.00194;               % Resistência de armadura [Ω]
Ls = 77e-6;                 % Indutância síncrona [H]
Ke = 0.0054;                % Constante de tensão [V/(rad/s)]

%% 2. Parâmetros do Retificador Ativo
eta_rect = 0.95;            % Eficiência do retificador
V_drop = 0.5;               % Queda de tensão por chave [V]
R_rect = 0.005;             % Resistência equivalente do retificador [Ω]

%% 3. Parâmetros do Conversor DC-DC Boost
eta_conv = 0.9;             % Eficiência do conversor
L_boost = 1e-3;             % Indutância do Boost [H]
C_bus = 1e-3;               % Capacitância do barramento [F]
V_bus_ref = 98;             % Tensão de referência do barramento [V]
R_boost = 0.01;             % Resistência série do conversor [Ω]

%% 4. Parâmetros do Controlador PI
Kp = 0.05;                  % Ganho proporcional
Ki = 0.1;                   % Ganho integral
D_min = 0.1;                % Duty cycle mínimo
D_max = 0.9;                % Duty cycle máximo

%% 5. Parâmetros da Carga
I_load_min = 5;             % Corrente mínima da carga [A]
I_load_max = 130;           % Corrente máxima da carga [A]
