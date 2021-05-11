clear;clc;
% Design of the digital Lowpass filter
% Specs.: 0.1dB ripple on the passband at a freq. of 1.5 kHz
% 21dB attenuation at the freq. of 2.5 kHz
% Sampling freq. at 8000 Hz
f_dp = 1500; % Hz  for passband
f_ds = 2500; % Hz  for stopband
omega_dp=2*pi*f_dp;
omega_ds=2*pi*f_ds;
Fs = 8000; % Hz
T=1/Fs;
As = 25;  % (dB) stopband attenuation
Ap = 0.1;  % (dB) passband ripple
omega_ap = 2/T*tan(omega_dp*T/2);  % => H(s) = omega_a/(s+omega_a)
omega_as = 2/T*tan(omega_ds*T/2);
nu_s = omega_as/omega_ap;

%compute smallest order for the filter
epsilon_sq=10^(0.1*Ap)-1;
N_butter=round(0.5 * log10( (10^(0.1*As)-1)/epsilon_sq )/log10(nu_s)  )
[B_butter,A_butter]=lp2lp([1],[1 3.8637 7.4641 9.1416 7.4641 3.8637 1],omega_ap);
[b_butter,a_butter]=bilinear(B_butter,A_butter,Fs);
[h_butter,w_butter]=freqz(b_butter,a_butter,512,Fs);


%comparison with FIR
% stopband attenuation 25dB
%omega_c = 0.5*(f_dp+f_ds) * 2*pi /Fs % cutoff freq.
omega_c = f_dp * 2*pi /Fs % cutoff freq.
df = (abs(f_dp-f_ds))/Fs; % normalized transition band
%consider hanning window
%tap = ceil(3.1/df);
%if (mod(tap,2)==0)
%    tap = tap+1;
%end
tap = 17;

Ftype = 1; % highpass filter
WnL = omega_c; WnH = 0;Wtype=3;%hanning
B_han=firwd(tap,Ftype,WnL,WnH,Wtype);
[h_FIR,w_FIR]=freqz(B_han,1,512,Fs);



phi_FIR = 180*unwrap(angle(h_FIR))/pi;
phi_butter = 180*unwrap(angle(h_butter))/pi;

subplot(2,1,1);
plot(w_FIR,20*log10(abs(h_FIR)),'k','LineWidth',1.2);grid;
hold on; plot(w_butter,20*log10(abs(h_butter)),'r','LineWidth',1.2);
axis([0 Fs/2 -250 5])
xlabel('Frequency (radians)');
ylabel('Magnitude (dB)');
legend({'17-taps FIR filter with Hanning window','6-th order IIR filter (Butterworth)'},'FontSize',16,'Location','Southeast')
subplot(2,1,2);
plot(w_FIR,phi_FIR,'k','LineWidth',1.2);grid;hold on;
plot(w_butter,phi_butter,'r','LineWidth',1.2);
xlabel('Frequency (radians)');
ylabel('Phase (degrees)');
dataLength = 199;
in=0.7*sin(0.15*pi*[0:1:dataLength])+0.1*sin(0.95*pi*[0:1:dataLength])+0.2*sin(0.77*pi*[0:1:dataLength]);
FIR_out = filter(B_han,1,in);
IIR_out = filter(b_butter,a_butter,in);
figure();
plot(in,'k','LineWidth',1.2);hold on;
plot(FIR_out);plot(IIR_out,'r');

%output fixed point coefficients
FIR_fixedPoint_b = round(B_han*2^14)
IIR_fixedPoint_b = round(b_butter*2^14)
IIR_fixedPoint_a = round(a_butter*2^14)

target_fs=Fs;
wavfilename = 'sp01.wav';
[inputSignal,speech_Fs] = audioread(wavfilename);
originalSound = resample(inputSignal(speech_Fs*0.2:speech_Fs*5.6,1),target_fs,speech_Fs);
IIR_filterSound = filter(b_butter,a_butter,originalSound);
sound(IIR_filterSound , target_fs);
sizeFile = size(wavfilename,2);
outFile = strcat(wavfilename(1:sizeFile-4) ,'_LP_IIR_filtered.wav');
audiowrite(outFile,IIR_filterSound,target_fs);

FIR_filterSound = filter(B_han,1,originalSound);
sound(FIR_filterSound , target_fs);
FIR_outFile = strcat(wavfilename(1:sizeFile-4) ,'_LP_FIR_filtered.wav');
audiowrite(FIR_outFile,FIR_filterSound,target_fs);