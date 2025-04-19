%% Parâmetros do Motor VHF - Baseado em Ensaio 07_2024

%% 1. Parâmetros Mecânicos
motor.braco_balanca = 0.8;                % [m] Comprimento do braço do dinamômetro
motor.massa_maxima = 22.17;               % [kgf] Máxima carga aplicada nos ensaios
motor.rpm_range = [200 400 600 800 1000]; % Faixa de velocidades testadas [rpm]

%% 3. Coeficientes do Modelo de Corrente DC
% IDC = A*tau + B*omega + C*(tau/omega) + D
motor.coeficientes.IDC = [0.873;        % A: Coef. torque [A/Nm]
                         0.0125;        % B: Coef. velocidade [A/rpm]
                         1.342;         % C: Coef. interação [A/(Nm/rpm)]
                         2.718];        % D: Offset [A]

%% 4. Lookup Table de Eficiência (η)
% Eixos: omega_rpm x torque_Nm
motor.lookup.omega = [200 400 600 800 1000]; % [rpm]
motor.lookup.torque = [0 50 100 150];        % [Nm]

% Matriz de eficiência [%] (omega x torque)
motor.lookup.eficiencia = [92 89 85 80;      % 200 rpm
                          88 84 79 75;      % 400 rpm
                          85 80 75 70;      % 600 rpm
                          82 77 72 67;      % 800 rpm
                          78 73 68 63];     % 1000 rpm

%% 5. Dados Térmicos
motor.temp_ref = 25;                     % [°C] Temperatura de referência
motor.coef_temp = 0.015;                 % [%/°C] Coef. de correção térmica