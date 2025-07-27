#ifndef DSBS_H_INCLUDED
#define DSBS_H_INCLUDED

#include <vector>
#include <cmath>
#include <iostream>

//helper insertion sort
///NEED TO MANUALLY INKREMENT INDEX FROM OUTSIDE
void Insert(unsigned char val, unsigned char* median ,int index)
{
    median[index] = val;
    for(int i=index; i>0; i--)
    {
        if(median[i-1] > median[i])
        {
            std::swap(median[i-1],median[i]);
        }
    }
}

unsigned char* median_filter(unsigned char* img, int channels, int img_height, int img_width, int kernal_length)
{
    unsigned char* result = new unsigned char[img_height * img_width * channels];

    //used to sort values and to get the median
    unsigned char median[kernal_length*kernal_length];

    for(int img_r=0; img_r < img_height; img_r++)
    {
        for(int img_c=0; img_c < img_width * channels; img_c += channels)
        {
            for(int chan=0; chan < channels; chan++)
            {
                int img_index = (img_c + chan) + (img_r * (img_width * channels));
                int index = 0;

                for(int H_r =0; H_r < kernal_length; H_r++)
                {
                    for(int H_c =0; H_c < kernal_length * channels; H_c += channels)
                    {
                        int imgCol =  H_c + img_c;
                        int imgRow = H_r + img_r;

                        ///clamping of image when going out of bound
                        int imgInd;
                        if(imgCol >= img_width * channels && imgRow >= img_height)
                            /// outside last row and last col
                            imgInd = (img_width - 1)  + chan + (img_height - 1) * img_width * channels;

                        else if(imgCol >= img_width * channels)
                            ///outside last col only
                            imgInd = (img_width - 1) + chan + imgRow * img_width * channels;

                        else if(imgRow >= img_height)
                            ///outside last row only
                            imgInd = imgCol + chan + (img_height - 1) * img_width * channels;

                        else
                            ///within bounds
                            imgInd = imgCol + chan + imgRow * img_width * channels;

                        Insert(img[imgInd] , median , index++);
                    }
                }
                //the median
                result[img_index] = median[ (kernal_length * kernal_length) / 2];
            }
        }
    }

    return result;

}

double gauss(double input, double sigma)
{
    return std::exp( (-1.0 * input * input) / (2 * sigma * sigma) );
}

unsigned char* bilateral_filter(unsigned char* img, int channels, int img_height, int img_width, int kernal_length
                             , double sigma_spatial, double sigma_intensity)
{
    unsigned char* result = new unsigned char[img_height * img_width * channels];

    int mid = (kernal_length - 1) / 2;

    for(int img_r=0; img_r < img_height; img_r++)
    {
        for(int img_c=0; img_c < img_width * channels; img_c += channels)
        {

                int img_index = (img_c) + (img_r * (img_width * channels));
                std::vector<double> sum(channels,0);
                double Wp = 0;

                for(int H_r =0; H_r < kernal_length; H_r++)
                {
                    for(int H_c =0; H_c < kernal_length * channels; H_c += channels)
                    {
                        int imgCol =  H_c + img_c;
                        int imgRow = H_r + img_r;
                        int y = mid - H_r;
                        int x = (H_c / channels) - mid;

                        ///clamping of image when going out of bound
                        int imgInd;
                        if(imgCol >= img_width * channels && imgRow >= img_height)
                            /// outside last row and last col
                            imgInd = (img_width - 1) + (img_height - 1) * img_width * channels;

                        else if(imgCol >= img_width * channels)
                            ///outside last col only
                            imgInd = (img_width - 1) + imgRow * img_width * channels;

                        else if(imgRow >= img_height)
                            ///outside last row only
                            imgInd = imgCol + (img_height - 1) * img_width * channels;

                        else
                            ///within bounds
                            imgInd = imgCol + imgRow * img_width * channels;

                        double RGBsum = 0;
                        for(int chan=0; chan < channels; chan++)
                            RGBsum += std::pow( img[imgInd + chan] - img[img_index + chan], 2 );

                        double Fs = gauss( std::sqrt(x*x + y*y), sigma_spatial );
                        double Fr = gauss( std::sqrt(RGBsum), sigma_intensity);
                        Wp += Fs * Fr;

                        for(int I=0; I<channels; I++)
                            sum[I] += img[imgInd + I] * Fs * Fr;
                    }
            }
            for(int chan=0; chan < channels; chan++)
            {
                result[img_index + chan] = (1.0/Wp) * sum[chan];
            }
        }
    }

    return result;
}

///sharpening using median filter blurring
unsigned char* Sharpen(unsigned char* img, int channels, int img_height, int img_width, int kernal_length, double alpha)
{
    if(alpha > 1)
        alpha = 1;
    else if(alpha < 0)
        alpha = 0;

    unsigned char* blurred = median_filter(img,channels,img_height,img_width,kernal_length);
    unsigned char* result = new unsigned char[img_height * img_width * channels];

    for(int i = 0; i < img_height * img_width * channels; ++i)
    {
        int sharpened = static_cast<int>(img[i] + alpha * (img[i] - blurred[i]));

        // clamp to [0, 255]
        if(sharpened < 0) sharpened = 0;
        else if(sharpened > 255) sharpened = 255;

        result[i] = static_cast<unsigned char>(sharpened);
    }


    delete[] blurred;
    return result;
}

#endif // DSBS_H_INCLUDED

/*
    for(int i=0;i<index;i++)
        std::cout<<static_cast<int>(median[i])<<" ";
    std::cout<<"channel is : "<<chan<<" "
    <<"pixel is now : "<<static_cast<int>(result[img_index])<<"\n";
*/
