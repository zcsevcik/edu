%%
%% solution.m
%%
%% Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
%% Date:           2013/12/16
%%
%% Tested with GNU Octave with octave-image library
%%
%% Bitmaps output size is different than original in octave.

%% ==================================================================== %%
img = imread('xsevci44.bmp');
[cy,cx] = size(img);

%% ====  S T E P 1  ( S H A R P E N )  ================================ %%
H = [ -0.5 -0.5 -0.5 ;
      -0.5  5.0 -0.5 ;
      -0.5 -0.5 -0.5 ];
img_step1 = imfilter(img, H);
imwrite(img_step1, 'step1.bmp');

%% ====  S T E P 2  ( F L I P )  ====================================== %%
img_step2 = flipdim(img_step1, 2);
imwrite(img_step2, 'step2.bmp');

%% ====  S T E P 3  ( M E D I A N )  ================================== %%
H = [ 5.0 5.0 ]; % 5-by-5 neighborhood

img_step3 = medfilt2(img_step2, H);
imwrite(img_step3, 'step3.bmp');

%% ====  S T E P 4  ( B L U R )  ====================================== %%
H = [ 1.0 1.0 1.0 1.0 1.0 ;
      1.0 3.0 3.0 3.0 1.0 ;
      1.0 3.0 9.0 3.0 1.0 ;
      1.0 3.0 3.0 3.0 1.0 ;
      1.0 1.0 1.0 1.0 1.0 ] / 49.0;

img_step4 = imfilter(img_step3, H);
imwrite(img_step4, 'step4.bmp');

%% ====  S T A N D A R D   E R R O R  ================================= %%
imgf_step4 = im2double(img_step4);
imgf_flip = im2double(flipdim(img, 2));
chyba = abs(imgf_flip - imgf_step4);
chyba = sum(sum(chyba));
chyba = chyba / (cx * cy) * 255.0 % 1px rescale

%% ====  S T E P 5  ( H I S T O G R A M   A D J U S T )  ============== %%
hist_l = min(min(imgf_step4));
hist_h = max(max(imgf_step4));

%imhist(imgf_step4);
imgf_step5 = imadjust(imgf_step4, [ hist_l hist_h ], []);
img_step5 = im2uint8(imgf_step5);
imwrite(img_step5, 'step5.bmp');

%% ====  M E A N  &  S T A N D A R D   D E V I A T I O N  ============= %%
mean_no_hist = mean2(imgf_step4) * 255.0
std_no_hist = std2(imgf_step4) * 255.0
mean_hist = mean2(imgf_step5) * 255.0
std_hist = std2(imgf_step5) * 255.0

%% ====  S T E P 6  ( Q U A N T I Z A T I O N )  ====================== %%
my_quantization = @(x, a, b, N) \
    round(((2^N)-1)*(x-a)/(b-a))*(b-a)/((2^N)-1) + a;

imgf_step6 = arrayfun(@(x) my_quantization(x, 0.0, 1.0, 2), imgf_step5);
img_step6 = im2uint8(imgf_step6);
imwrite(img_step6, 'step6.bmp');

%% ==================================================================== %%
%     OUTPUT GNU Octave:                                                   
% chyba = 9.0815
% mean_no_hist = 107.95
% std_no_hist = 28.772
% mean_hist = 148.80
% std_hist = 39.658

%% ==================================================================== %%
%     OUTPUT Matlab:
% chyba = 9.0644
% mean_no_hist = 107.7112
% std_no_hist = 28.7523
% mean_hist = 148.4668
% std_hist = 39.6315

%% ==================================================================== %%

