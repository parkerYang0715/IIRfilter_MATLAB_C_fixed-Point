clear;clc;
% Design of the digital highpass filter
% Specs.: 0.1dB ripple on the passband at a freq. of 2000 Hz
% 40dB attenuation at the freq. of 1kHz
% Sampling freq. at 8000 Hz
f_dp = 2000; % Hz  for passband
f_ds = 1000; % Hz  for stopband
omega_dp=2*pi*f_dp;
omega_ds=2*pi*f_ds;
Fs = 8000; % Hz
T=1/Fs;
As = 40;  % (dB) stopband attenuation
Ap = 0.1;  % (dB) passband ripple
omega_ap = 2/T*tan(omega_dp*T/2);  % => H(s) = omega_a/(s+omega_a)
omega_as = 2/T*tan(omega_ds*T/2);
nu_s = omega_ap/omega_as;

%compute smallest order for the filter for chebyshev
epsilon_sq=10^(0.1*Ap)-1;
tmp =(10^(0.1*As)-1 )/epsilon_sq;
tmp = sqrt(tmp);
N=round( log10( tmp +sqrt(tmp^2-1)) / log10( nu_s +sqrt(nu_s^2-1))  );

%[B,A]=lp2hp([0.3579],[1 1.1974 1.7169 1.0255 0.3791],omega_ap); % 4th-order
[B,A]=lp2hp([0.1789],[1 1.1725 1.9374 1.3096 0.7525 0.1789],omega_ap); %5th
[b,a]=bilinear(B,A,Fs)
%freqz(b,a,512,Fs); axis([0 Fs/2 -40 3])
[h_IIR,w_IIR]=freqz(b,a,512,Fs);

%compute smallest order for the filter
epsilon_sq=10^(0.1*Ap)-1;
N_butter=round(0.5 * log10( (10^(0.1*As)-1)/epsilon_sq )/log10(nu_s)  )
[B_butter,A_butter]=lp2hp([1],[1 4.494 10.0978 14.5918 14.5918 10.0978 4.494 1],omega_ap); 
[b_butter,a_butter]=bilinear(B_butter,A_butter,Fs)
[h_butter,w_butter]=freqz(b_butter,a_butter,512,Fs);


%comparison with FIR
% stopband 0~1000 Hz
% passband 2000~4000 Hz
% stopband attenuation 40dB
As = 40;  % (dB) stopband attenuation
Ap = 0.1;  % (dB) passband ripple
omega_c = 0.5*(f_dp+f_ds) * 2*pi /Fs % cutoff freq.
df = (abs(f_dp-f_ds))/Fs % normalized transition band
%consider hanning window
tap = ceil(3.1/df)
if (mod(tap,2)==0)
    tap = tap+1
end

M = (tap-1)/2;
b_d=sin(omega_c*[-M:1:-1])./([-M:1:-1]*pi);
b_d(M+1)=omega_c/pi;
b_d(M+2:1:tap)=b_d(M:-1:1);
b_d(1:tap)=-b_d(1:tap);
b_d(M+1)=1+b_d(M+1);
bw=b_d'.*hanning(tap);
%[h_FIR,w_FIR]=freqz(bw,1,512,Fs);

Ftype = 2; % highpass filter
WnL = 0; WnH = omega_c;Wtype=3;%hanning
B_han=firwd(tap,Ftype,WnL,WnH,Wtype);
[h_FIR,w_FIR]=freqz(B_han,1,512,Fs);



phi_FIR = 180*unwrap(angle(h_FIR))/pi;
phi_butter = 180*unwrap(angle(h_butter))/pi;
phi_IIR = 180*unwrap(angle(h_IIR))/pi;
subplot(2,1,1);
plot(w_FIR,20*log10(abs(h_FIR)),'k','LineWidth',1.2);grid;
hold on; plot(w_butter,20*log10(abs(h_butter)),'r','LineWidth',1.2);
plot(w_IIR,20*log10(abs(h_IIR)),'--','LineWidth',1.2)
axis([0 Fs/2 -250 5])
xlabel('Frequency (radians)');
ylabel('Magnitude (dB)');
legend({'25-taps FIR filter with Hanning window','7-th order IIR filter (Butterworth)','5-th order IIR filter (Chebyshev)'},'FontSize',16,'Location','Southeast')
subplot(2,1,2);
plot(w_FIR,phi_FIR,'k','LineWidth',1.2);grid;hold on;
plot(w_butter,phi_butter,'r','LineWidth',1.2);
plot(w_IIR,phi_IIR,'--','LineWidth',1.2);
xlabel('Frequency (radians)');
ylabel('Phase (degrees)');