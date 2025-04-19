% Set the paths, load databases and set the enviroment to test the models

% Set paths
curPath=pwd;
model_libs=[curPath,'\Models'];
addpath(genpath(model_libs),'-begin');
addpath(genpath([curPath,'\Models\Components']));
addpath(genpath([curPath,'\Scripts']));
addpath(genpath([curPath,'\Database']));


Path_m_temp=[curPath,'\Database'];
fp=findfiles(Path_m_temp,'m');
nump=size(fp,1);
for i=1:1:nump
   run(fp{i,1});
   disp('......');
   disp(fp{i,1});
end

clear curPath model_libs nump fp Path_m_temp