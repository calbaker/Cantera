function setMaxTimeStep(r, maxstep)
% SETMAXTIMESTEP - Set the maximum time step.
% 
% The integrator chooses a step size based on the desired error
% tolerances and the rate at which the solution is changing. In
% some cases, the solution changes very slowly at first, then very
% rapidly (ignition problems). In such cases, the integrator may
% choose a timestep that is too large, which leads to numerical
% problems later. Use this method to set an upper bound on the
% timestep.
%
reactornetmethods(6, reactornet_hndl(r), maxstep);
