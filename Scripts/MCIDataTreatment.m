

% Load MCI data
DatMCI

throttle_angle = (0:1:90)';
rpm_speed = (RPM_idle:100:RPM_max)';
% Considering generator power
Imax = 120;
Ke = 0.0054;                % Constante de tensão [V/rpm]
rs = 0.001943;      % [ohm] Resistencia do estator 
Ls = 77e-6;

tdat=[11 15 30 50 70 90];
torque = 9.8*[polyval(p_Torque_11,rpm_speed),...
          polyval(p_Torque_15,rpm_speed),...
          polyval(p_Torque_30,rpm_speed),...
          polyval(p_Torque_50,rpm_speed),...
          polyval(p_Torque_70,rpm_speed),...
          polyval(p_Torque_90,rpm_speed)];

[rpmGrid,throttleGrid] = meshgrid(rpm_speed,throttle_angle);

[x,y,z] = prepareSurfaceData((RPM_idle:100:RPM_max)',tdat',torque);
tfit = fit([x,y],z,"cubicinterp",ExtrapolationMethod="linear");
torqueMCI = tfit(rpmGrid,throttleGrid);
powerMCI = torqueMCI.*rpm_speed'*pi/30/1000;

[x,y,z] = prepareSurfaceData(RPM,theta,Eff_exp);
efffit = fit([x,y],z,"cubicinterp",Normalize='on',ExtrapolationMethod="linear");
effMCI = efffit(rpmGrid,throttleGrid);

% Find target speeds for each power with the best efficiency
tspeed = RPM_idle:10:RPM_max;
[temprpmGrid,tempthrottleGrid] = meshgrid(tspeed,0:0.1:90);
temptorqueMCI = tfit(temprpmGrid,tempthrottleGrid);
tpowerMCI = temptorqueMCI.*tspeed*pi/30/1000;
tempeffMCI = efffit(temprpmGrid,tempthrottleGrid);

Vretmax = sqrt(6)*(Ke*tspeed - Imax/sqrt(2)*sqrt(rs^2+Ls^2*(tspeed*pi/30).^2))-2*Vd;
Idcdcmax = Vret/V_nom*Imax;
powerRetMax = Vretmax*Imax/1000;
powerMax = Imax*Ke*60/5*tspeed*pi/30/1000;

powerTarget=(0:0.1:6.8)';
effTarget = zeros(length(powerTarget),1);
speedTarget = 5000*ones(length(powerTarget),1);
thetaTarget = zeros(length(powerTarget),1);
for p = 2:length(powerTarget)
    idx = zeros(length(tspeed),1);
    dif = zeros(length(tspeed),1);
    teffspeed = zeros(length(tspeed),1);
    % Iterate over each speed to calculate to find the best efficiency
    % for i=find((powerTarget(p)*1000./(3*Ke*tspeed))<Imax)
    for i=find((powerMax>powerTarget(p))& abs(tspeed-speedTarget(p-1))<500)
        [dif(i),idx(i)]=min(abs(tpowerMCI(:,i)-powerTarget(p)));
        teffspeed(i) = tempeffMCI(idx(i),i);
        % if (powerTarget(p)>1.5 || tspeed(i)<4000)
        %     teffspeed(i) = tempeffMCI(idx(i),i);
        % end
    end
    [effTarget(p),id] = max(teffspeed);
    % if powerTarget(p) <0.5
    %     % The algorithm don't work so well for low power
    %     % Specifying speed manually
    %     % id = find(tspeed==(1800+300*powerTarget(p)));
    %     % effTarget(p) = tempeffMCI(idx(id),id);
    % elseif abs(temprpmGrid(1,id)-speedTarget(p-1)) > 50
    %     % Avoid high speed jumps
    %     id = find(tspeed==(speedTarget(p-1)+50));
    %     if isempty(id)
    %         id = find(tspeed==RPM_max);
    %     end
    %     effTarget(p) = tempeffMCI(idx(id),id);
    % end
    speedTarget(p) = temprpmGrid(1,id);
    thetaTarget(p) = tempthrottleGrid(id);
end

% Estimate generator current without loss
torqueTarget = 1000*powerTarget./(speedTarget*pi/30);
genRetCurrTarget = torqueTarget/(Ke*60/5);
vRetTarget = sqrt(6)*(Ke*speedTarget - genRetCurrTarget/sqrt(2).*sqrt(rs^2+Ls^2*(speedTarget*pi/30).^2));
dcdcCurrentTarget = vRetTarget/V_nom.*genRetCurrTarget;

targetsMCI = table(powerTarget,speedTarget,torqueTarget,thetaTarget,effTarget,genRetCurrTarget,vRetTarget,dcdcCurrentTarget,'VariableNames',["Power","Speed","Torque","Throttle","Efficiency","GenRetCurrent","GenRetVoltage","dcdcGenCurrent"]);

% clear tempthrottleGrid temprpmGrid tempeffMCI temptorqueMCI teffspeed tpowerMCI tspeed idx dif id p i
% clear powerTarget speedTarget genRetCurrTarget thetaTarget effTarget torqueTarget vRetTarget dcdcCurrentTarget

figure(1)
contourf(rpmGrid,powerMCI,throttleGrid,'ShowText','on')
xlabel('Rotação (rpm)')
zlabel('\theta (grau)')
ylabel('Power (kW)')

figure(2)
surf(rpmGrid,throttleGrid,powerMCI)
xlabel('Rotação (rpm)')
ylabel('\theta (grau)')
zlabel('Power (kW)')

figure(3)
contour(rpmGrid,torqueMCI,throttleGrid,'ShowText','on')
xlabel('Rotação (rpm)')
zlabel('\theta (grau)')
ylabel('Torque (Nm)')

figure(4)
surf(rpmGrid,throttleGrid,torqueMCI)
xlabel('Rotação (rpm)')
ylabel('\theta (grau)')
zlabel('Torque (Nm)')

figure(5)
contour(rpmGrid,effMCI,throttleGrid)
xlabel('Rotação (rpm)')
zlabel('\theta (grau)')
ylabel('Efficiency')

figure(6)
surf(rpmGrid,throttleGrid,effMCI)
xlabel('Rotação (rpm)')
ylabel('\theta (grau)')
zlabel('Efficiency')

figure(7)
contourf(rpmGrid,powerMCI,effMCI,'ShowText','on')
xlabel('Rotação (rpm)')
ylabel('Power (kW)')
zlabel('Efficiency')
hold on
plot(targetsMCI.Speed,targetsMCI.Power);
hold off

save("Database\targetsMCIS.mat","targetsMCI","effMCI","powerMCI","torqueMCI","rpm_speed","throttle_angle")

