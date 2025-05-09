% -------------------------------------------------------------------
%  Generated by MATLAB on 26-Apr-2025 16:14:22
%  MATLAB version: 24.1.0.2689473 (R2024a) Update 6
% -------------------------------------------------------------------

% Parameters of the PIO algebraic estimator of tire forces
TC_OBSV_A = ...
  [1 0 0 0 -0.000295833333333333 0 0 0;
   0 1 0 0 0 -0.000295833333333333 0 0;
   0 0 1 0 0 0 -0.000289029106452269 0;
   0 0 0 1 0 0 0 -0.000289029106452269;
   0 0 0 0 1 0 0 0;
   0 0 0 0 0 1 0 0;
   0 0 0 0 0 0 1 0;
   0 0 0 0 0 0 0 1];

TF_OBSV_B = ...
  [0.001 0 0 0;
   0 0.001 0 0;
   0 0 0.001 0;
   0 0 0 0.001;
   0 0 0 0;
   0 0 0 0;
   0 0 0 0;
   0 0 0 0];

TF_OBSV_C = ...
  [1 0 0 0 0 0 0 0;
   0 1 0 0 0 0 0 0;
   0 0 1 0 0 0 0 0;
   0 0 0 1 0 0 0 0];

TF_OBSV_D = ...
  [0 0 0 0;
   0 0 0 0;
   0 0 0 0;
   0 0 0 0];

TF_OBSV_L = ...
  [1.25529346077614 8.04116153625104E-17 6.00475947507474E-17 -2.82123522639367E-18 ...
   ;
   2.00276564441579E-17 1.25529346077614 -1.21459165718105E-17 -4.71280127051238E-16 ...
   ;
   7.41393210777867E-17 -1.55285428203097E-17 1.25026270665115 -2.73214500271485E-17 ...
   ;
   -7.67661836159404E-18 -2.40483552218547E-16 -1.188967321757E-17 1.25026270665115 ...
   ;
   -862.96381107429 1.34485100686412E-15 6.45591712778729E-13 -6.97858976227793E-17 ...
   ;
   -3.75164864651043E-14 -862.963811074287 6.87461803097689E-14 1.63655029768099E-12 ...
   ;
   5.12295900694553E-13 9.38698109467505E-14 -865.87371674445 2.82368808538776E-14 ...
   ;
   -1.56315675833844E-13 1.11031727256951E-12 6.69794533554822E-13 -865.873716744449 ...
   ];

TF_OBSV_Psi = ...
  [-0.28902910645227 0 0 0 0 0 0 0;
   0 -0.28902910645227 0 0 0 0 0 0;
   0 0 -0.28902910645227 0 0 0 0 0;
   0 0 0 -0.28902910645227 0 0 0 0;
   0 0 0 0 2 0 0 0;
   0 0 0 0 0 2 0 0;
   0 0 0 0 0 0 2 0;
   0 0 0 0 0 0 0 2];

% Parameters of the extremum seeking algorithm to optimal slip estimation
TC_ESC_C = 39.8;
TC_ESC_D = 0.05;
TC_ESC_Lc = [1.184825089301992; -624.76086524617131];
TC_ESC_a = 0.015;
TC_ESC_esc_ic = 0.1;
TC_ESC_omega = 100;
TC_ESC_phi = -1.0471975511965976;
TC_ESC_re = 0.284;
TC_ESC_x0 = 200;
TC_ESC_x_c = 90;
TC_ESC_x_l = 10;

% Parameters of the traction controller using model-free
TC_MF_D = 0.006;
TC_MF_I = 0.04472;
TC_MF_Kp = 150;
TC_MF_Kpc = 250;
TC_MF_Kpp = 1600;
TC_MF_N = 17;
TC_MF_Nd = 10;
TC_MF_P = 5000;
TC_MF_Td = 0.005075;
TC_MF_Ti = 1.0E+6;
TC_MF_Ts = 0.005;
TC_MF_alpha = 0.9;