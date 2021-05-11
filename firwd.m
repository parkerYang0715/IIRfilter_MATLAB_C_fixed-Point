function B=firwd(N,Ftype,WnL,WnH,Wtype)
% B = firwd(N,Ftype,WnL,WnH,Wtype)
% FIR filter design using the window function method.
% Input parameters:
% N: the number of the FIR filter taps.
% Note: It must be odd number.

% Ftype: the filter type
% 1. Lowpass filter
% 2. Highpass filter
% 3. Bandpass filter
% 4. Bandstop filter
% WnL: lower cutoff frequency in radians. Set WnL=0 for the highpass filter.
% WnH: upper cutoff frequency in radians. Set WnH=0 for the lowpass filter.
% Wtypw: window function type
% 1. Rectangular window
% 2. Triangular window
% 3. Hanning window
% 4. Hamming window
% 5. Balckman window
% Output:
% B: FIR filter coefficients.
M = (N-1)/2;
hH=sin(WnH*[-M:1:-1])./([-M:1:-1]*pi);
hH(M+1)=WnH/pi;
hH(M+2:1:N)=hH(M:-1:1);
hL=sin(WnL*[-M:1:-1])./([-M:1:-1]*pi);
hL(M+1)=WnL/pi;
hL(M+2:1:N)=hL(M:-1:1);
if Ftype == 1
    h(1:N)=hL(1:N);
end
if Ftype == 2
    h(1:N)=-hH(1:N);
    h(M+1)=1+h(M+1);
end
if Ftype ==3
    h(1:N)=hH(1:N)-hL(1:N);
end
if Ftype == 4
    h(1:N)=hL(1:N)-hH(1:N);
    h(M+1)=1+h(M+1);
end
% Window functions
if Wtype ==1
    w(1:N)=ones(1,N);
end
if Wtype ==2
    w=1-abs([-M:1:M])/M;
end
if Wtype == 3
    w= 0.5+0.5*cos([-M:1:M]*pi/M);
    %w= 0.5-0.5*cos(2*[0:1:N-1]*pi/(N-1)); % MATLAB hanning is different
end
if Wtype == 4
    w=0.54+0.46*cos([-M:1:M]*pi/M);
end
if Wtype == 5
    w=0.42+0.5*cos([-M:1:M]*pi/M)+0.08*cos(2*[-M:1:M]*pi/M);
end
B = h .* w;

end