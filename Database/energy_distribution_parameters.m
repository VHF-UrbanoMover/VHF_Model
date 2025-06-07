%%PARAMETERS

V_nom = 96;          %preciso determinar
V_gen_max = 0;      %preciso determinar
I_gen_max = 65;      %preciso determinar
I_gen_min = 0;      %preciso determinar
I_batt_max = 110;     %preciso determinar
I_batt_min = -110;     %preciso determinar
SoC_min = 0.2;        %preciso determinar
tau_gen = 3;
I_total_max = I_gen_max + I_gen_min;
NLF_alpha1 = 400;
Kp = 0.1;
Ki = 0.01;
