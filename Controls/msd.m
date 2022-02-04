classdef msd
    % Mass Spring Damper Data Set Class
    properties
        m   % Mass [kg]
        c   % Damper coefficient [N/(m/s)]
        k   % Spring coefficient [N/m]
    end
    properties (Dependent)
        wn  % Undamped natural frequency [rad/s]
        wd  % Damped natural frequency [rad/s]
        zeta % Damping ratio
        s   % Roots of the characteristic equation
        TF  % Transfer function
    end
    methods
        % constructor
        function obj = msd(m, c ,k) 
            if nargin > 0
                obj.m = m;
                obj.c = c;
                obj.k = k;
            end
        end
        % dependent properties calculated on request
        function wn = get.wn(obj)
            wn = sqrt(obj.k/obj.m);    % undamped natural frequency [rad/s]
        end
        function zeta = get.zeta(obj)
             zeta = obj.c/(2*obj.m*obj.wn); % damping ratio
        end
        function wd = get.wd(obj)
            wd = obj.wn*sqrt(1-obj.zeta^2); % damped natural frequency [rad/s]
        end
        function s = get.s(obj)
            s = complex( [-obj.zeta*obj.wn + obj.wn*sqrt(obj.zeta^2-1)
                -obj.zeta*obj.wn - obj.wn*sqrt(obj.zeta^2-1)] );
            % root of the characteristic eq
        end
        function TransferFunction = get.TF(obj)
            TransferFunction = tf(1, [obj.m, obj.c, obj.k]); % transfer function
        end
        % functions that outputs time responses
        function [step_t, step_x] = Step(obj, t, step_value)
            % step_value - the magnitude of the input force [N]
            % for unit step input set it to 1
            % t - time vector at which the step response is calculated
            if obj.zeta < 1 % underdamped
                theta = atan(sqrt(1-obj.zeta^2)/obj.zeta);
                x = step_value/obj.k * (1  - exp(-obj.zeta*obj.wn.*t) .* sin(obj.wd.*t + theta) ./sqrt(1-obj.zeta^2));
 
            elseif obj.zeta == 1 % critically damped
                x = step_value/obj.k * (1- (1+obj.wn.*t).*exp(-obj.wn.*t));
 
            else
                alpha1 = obj.wn * (obj.zeta + sqrt(obj.zeta^2-1));
                alpha2 = obj.wn * (obj.zeta - sqrt(obj.zeta^2-1));
 
                x =  (step_value/obj.wn^2) * ...
                    (1 - (alpha1.*exp(-alpha2.*t) - alpha2.*exp(-alpha1.*t) )./(alpha1-alpha2) );
            end
            step_t = t;
            step_x = x;
        end
        function [impulse_t, impulse_x] = Impulse(obj, t, scale_value)
            % scale_value - the multiplier of the scaled impulse
            % for unit impulse set it to 1
            if obj.zeta < 1 % underdamped
                x = scale_value/ (obj.m * obj.wd) * exp(-obj.zeta.*obj.wn.*t) .* sin(obj.wd.*t);
 
            elseif obj.zeta == 1 % critically damped
                x = scale_value/obj.m .*t.*exp(-obj.wn.*t);
 
            else
                alpha1 = obj.wn * (obj.zeta + sqrt(obj.zeta^2-1));
                alpha2 = obj.wn * (obj.zeta - sqrt(obj.zeta^2-1));
 
                x =  (scale_value/(alpha1-alpha2)) * ...
                    ( exp(-alpha2.*t) - exp(-alpha1.*t) );
            end
            impulse_t = t;
            impulse_x = x;
        end
    end
end