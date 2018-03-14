#define _USE_MATH_DEFINES

#include "include/image.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

/**
 * Image
 **/
Image::Image (int width_, int height_){

    assert(width_ > 0);
    assert(height_ > 0);

    width           = width_;
    height          = height_;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
	int b = 0; //which byte to write to
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			data.raw[b++] = 0;
			data.raw[b++] = 0;
			data.raw[b++] = 0;
			data.raw[b++] = 0;
		}
	}

    assert(data.raw != NULL);
}

Image::Image (const Image& src){
	
	width           = src.width;
    height          = src.height;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
    
    //memcpy(data.raw, src.data.raw, num_pixels);
    *data.raw = *src.data.raw;
}

Image::Image (char* fname){

	int numComponents; //(e.g., Y, YA, RGB, or RGBA)
	data.raw = stbi_load(fname, &width, &height, &numComponents, 4);
	
	if (data.raw == NULL){
		printf("Error loading image: %s", fname);
		exit(-1);
	}
	

	num_pixels = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;
	
}

Image::~Image (){
    delete data.raw;
    data.raw = NULL;
}

void Image::Write(char* fname){
	
	int lastc = strlen(fname);

	switch (fname[lastc-1]){
	   case 'g': //jpeg (or jpg) or png
	     if (fname[lastc-2] == 'p' || fname[lastc-2] == 'e') //jpeg or jpg
	        stbi_write_jpg(fname, width, height, 4, data.raw, 95);  //95% jpeg quality
	     else //png
	        stbi_write_png(fname, width, height, 4, data.raw, width*4);
	     break;
	   case 'a': //tga (targa)
	     stbi_write_tga(fname, width, height, 4, data.raw);
	     break;
	   case 'p': //bmp
	   default:
	     stbi_write_bmp(fname, width, height, 4, data.raw);
	}
}

void Image::AddNoise (double factor)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel rand_p = PixelRandom();
			Pixel noisy_p = (p*(1-factor))+(rand_p*factor);
			GetPixel(x, y) = noisy_p;
		}
	}
}

void Image::Brighten (double factor)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel scaled_p = p*factor;
			GetPixel(x, y) = scaled_p;
		}
	}
}

void Image::ChangeContrast (double factor)
{
	int x,y;
	float total_lum = 0;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			total_lum += GetPixel(x, y).Luminance();
		}
	}
	float mean_lum = total_lum / (x * y);
	Pixel gray_p = Pixel(mean_lum,mean_lum,mean_lum);
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			Pixel p = GetPixel(x, y);
			GetPixel(x, y) = PixelLerp(p, gray_p, (1-factor));
		}
	}
}


void Image::ChangeSaturation(double factor)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			Pixel p = GetPixel(x, y);
			float rgb_avg = (p.r + p.g + p.b)/3;
			Pixel grayscale_p = Pixel(rgb_avg, rgb_avg, rgb_avg);
			GetPixel(x, y) = PixelLerp(p, grayscale_p, (1-factor));
		}
	}
}


Image* Image::Crop(int xcrop, int ycrop, int wcrop, int hcrop)
{
	int x,y;
	Image * crop_img = new Image(wcrop, hcrop);
	for (x = 0 ; x < wcrop ; x++){
		for (y = 0 ; y < hcrop ; y++){
			crop_img->SetPixel(x, y, this->GetPixel(xcrop+x, ycrop+y));
		}
	}
	return crop_img;
}


void Image::ExtractChannel(int channel)
{
	int x,y;
	if (channel==1){
		for (x = 0 ; x < Width() ; x++){
			for (y = 0 ; y < Height() ; y++){
				Pixel p = GetPixel(x, y);
				Pixel extracted_p = Pixel(p.r,0,0);
				GetPixel(x, y) = extracted_p;
			}
		}
	} else if (channel==2){
		for (x = 0 ; x < Width() ; x++){
			for (y = 0 ; y < Height() ; y++){
				Pixel p = GetPixel(x, y);
				Pixel extracted_p = Pixel(0,p.g,0);
				GetPixel(x, y) = extracted_p;
			}
		}
	} else if (channel==3){
		for (x = 0 ; x < Width() ; x++){
			for (y = 0 ; y < Height() ; y++){
				Pixel p = GetPixel(x, y);
				Pixel extracted_p = Pixel(0,0,p.b);
				GetPixel(x, y) = extracted_p;
			}
		}
	}
}


void Image::Quantize (int nbits)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			Pixel p = GetPixel(x, y);
			GetPixel(x, y) = PixelQuant(p, nbits);
		}
	}
}

void Image::RandomDither (int nbits)
{
	int x,y;
	int shift = 8-nbits;
	float mult = 255/float(255 >> shift);
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			Pixel p = GetPixel(x, y);
			int new_r, new_g, new_b;
			new_r = ((p.r + (int)(ComponentRandom()/(pow(2,nbits)))) >> shift);
			new_g = ((p.g + (int)(ComponentRandom()/(pow(2,nbits)))) >> shift);
			new_b = ((p.b + (int)(ComponentRandom()/(pow(2,nbits)))) >> shift);
			Pixel new_p;
			new_p.SetClamp(new_r*mult, new_g*mult, new_b*mult);
			GetPixel(x, y) = new_p;
		}
	}
}

static int Bayer4[4][4] =
{
    {15,  7, 13,  5},
    { 3, 11,  1,  9},
    {12,  4, 14,  6},
    { 0,  8,  2, 10}
};


void Image::OrderedDither(int nbits)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			Pixel p = GetPixel(x, y);
			int i = x%4, j = y%4;
			float e_r, e_g, e_b;
			e_r = p.r/16;
			e_g = p.g/16;
			e_b = p.b/16;
			int new_r, new_g, new_b;
			int var = pow(2,9-nbits)-1;
			/* round rgb values up or down to the nearest threshhold depending
			   on whether e values are higher or lower than the Bayer4 value */
			if (e_r > Bayer4[i][j]){
				new_r = (p.r-(p.r%var)+var);
			} else {
				new_r = (p.r-(p.r%var));
			}
			if (e_g > Bayer4[i][j]){
				new_g = (p.g-(p.g%var)+var);
			} else {
				new_g = (p.g-(p.g%var));
			}
			if (e_b > Bayer4[i][j]){
				new_b = (p.b-(p.b%var)+var);
			} else {
				new_b = (p.b-(p.b%var));
			}
			Pixel new_p;
			new_p.SetClamp(new_r, new_g, new_b);
			GetPixel(x, y) = new_p;
		}
	}
}

/* Error-diffusion parameters */
const double
    ALPHA = 7.0 / 16.0,
    BETA  = 3.0 / 16.0,
    GAMMA = 5.0 / 16.0,
    DELTA = 1.0 / 16.0;

void Image::FloydSteinbergDither(int nbits)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			Pixel p = GetPixel(x, y);
			int new_r, new_g, new_b;
			int var = pow(2,9-nbits)-1;

			/* round rgb values to nearest threshhold */
			if (p.r%(var+1) > (var+1)/2){
				new_r = (p.r-(p.r%var)+var);
			} else {
				new_r = (p.r-(p.r%var));
			}
			if (p.g%(var+1) > (var+1)/2){
				new_g = (p.g-(p.g%var)+var);
			} else {
				new_g = (p.g-(p.g%var));
			}
			if (p.b%(var+1) > (var+1)/2){
				new_b = (p.b-(p.b%var)+var);
			} else {
				new_b = (p.b-(p.b%var));
			}

			/* set pixel at x,y with new value */
			Pixel new_p;
			new_p.SetClamp(new_r, new_g, new_b);
			
			/* diffuse error to nearby pixels!!! */
			int r_error = p.r - new_p.r;
			int g_error = p.g - new_p.g;
			int b_error = p.b - new_p.b;
			this->SetPixel(x, y, new_p);

			if (x+1 < Width()){
				this->SetPixel(x+1, y, Pixel(ComponentClamp(this->GetPixel(x+1,y).r+r_error*ALPHA),
					ComponentClamp(this->GetPixel(x+1,y).g+g_error*ALPHA),ComponentClamp(this->GetPixel(x+1,y).b+b_error*ALPHA))); 
			}
			if ((x-1 >= 0) && (y+1 < Height())){
				this->SetPixel(x-1, y+1, Pixel(ComponentClamp(this->GetPixel(x-1,y+1).r+r_error*BETA),
					ComponentClamp(this->GetPixel(x-1,y+1).g+g_error*BETA),ComponentClamp(this->GetPixel(x-1,y+1).b+b_error*BETA))); 
			}
			if (y+1 < Height()){
				this->SetPixel(x, y+1, Pixel(ComponentClamp(this->GetPixel(x,y+1).r+r_error*GAMMA),
					ComponentClamp(this->GetPixel(x,y+1).g+g_error*GAMMA),ComponentClamp(this->GetPixel(x,y+1).b+b_error*GAMMA))); 
			}
			if ((x+1 < Width()) && (y+1 < Height())){
				this->SetPixel(x+1, y+1, Pixel(ComponentClamp(this->GetPixel(x+1,y+1).r+r_error*DELTA),
					ComponentClamp(this->GetPixel(x+1,y+1).g+g_error*DELTA),ComponentClamp(this->GetPixel(x+1,y+1).b+b_error*DELTA))); 
			}
		}
	}

}

void Image::Blur(int n)
{
	int x,y,filter_x,filter_y;
	float d,gaussian;
	Image * temp_img = new Image(Width(), Height());
	
	/* Create an nxn gaussian filter! */
	float gaussian_filter[n][n];
	float gaussian_sum = 0;
	for (x = 0 ; x < n ; x++){
		for (y = 0 ; y < n ; y++){
			d = sqrt(pow(n/2-x,2)+pow(n/2-y,2));
			gaussian = 1/sqrt(2*M_PI)*exp(-1*pow(d,2)/2);
			gaussian_filter[x][y] = gaussian;
			gaussian_sum += gaussian;
		}
	}
	for (x = 0 ; x < n ; x++){
		for (y = 0 ; y < n ; y++){
			gaussian_filter[x][y] /= gaussian_sum;
		}
	}

	/* Fill temp_img with the result of applying the filter to
	   each pixel on the original image */
	int filter_r;
	if (n%2 == 0){
		filter_r = (n - 1)/2;
	} else {
		filter_r = (n - 1)/2 + 1;
	}
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			float red = 0, green = 0, blue = 0;
			for (filter_x = 0 ; filter_x < n ; filter_x++){
				for (filter_y = 0 ; filter_y < n ; filter_y++){
					int img_p_x = (x - filter_r + filter_x + Width()) % Width();
					int img_p_y = (y - filter_r + filter_y + Height()) % Height();
					red += this->GetPixel(img_p_x, img_p_y).r * gaussian_filter[filter_x][filter_y];
					green += this->GetPixel(img_p_x, img_p_y).g * gaussian_filter[filter_x][filter_y];
					blue += this->GetPixel(img_p_x, img_p_y).b * gaussian_filter[filter_x][filter_y];
				}
			}
			red = fmin(fmax(red, 0), 255);
			green = fmin(fmax(green, 0), 255);
			blue = fmin(fmax(blue, 0), 255);
			Pixel blurry_p = Pixel(red,green,blue);
			temp_img->SetPixel(x, y, blurry_p);
		}
	}

	/* Replace the image with the blurred image */
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			this->GetPixel(x, y) = temp_img->GetPixel(x, y);
		}
	}
}

void Image::Sharpen(int n)
{
	int x,y,filter_x,filter_y;
	float d,gaussian;
	Image * temp_img = new Image(Width(), Height());
	
	/* Create an nxn gaussian filter! */
	float gaussian_filter[n][n];
	float gaussian_sum = 0;
	for (x = 0 ; x < n ; x++){
		for (y = 0 ; y < n ; y++){
			d = sqrt(pow(n/2-x,2)+pow(n/2-y,2));
			gaussian = 1/sqrt(2*M_PI)*exp(-1*pow(d,2)/2);
			gaussian_filter[x][y] = gaussian;
			gaussian_sum += gaussian;
		}
	}
	for (x = 0 ; x < n ; x++){
		for (y = 0 ; y < n ; y++){
			gaussian_filter[x][y] /= gaussian_sum;
		}
	}

	/* Fill temp_img with the result of applying the filter to
	   each pixel on the original image */
	int filter_r;
	if (n%2 == 0){
		filter_r = (n - 1)/2;
	} else {
		filter_r = (n - 1)/2 + 1;
	}
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			float red = 0, green = 0, blue = 0;
			for (filter_x = 0 ; filter_x < n ; filter_x++){
				for (filter_y = 0 ; filter_y < n ; filter_y++){
					int img_p_x = (x - filter_r + filter_x + Width()) % Width();
					int img_p_y = (y - filter_r + filter_y + Height()) % Height();
					red += this->GetPixel(img_p_x, img_p_y).r * gaussian_filter[filter_x][filter_y];
					green += this->GetPixel(img_p_x, img_p_y).g * gaussian_filter[filter_x][filter_y];
					blue += this->GetPixel(img_p_x, img_p_y).b * gaussian_filter[filter_x][filter_y];
				}
			}
			red = fmin(fmax(red, 0), 255);
			green = fmin(fmax(green, 0), 255);
			blue = fmin(fmax(blue, 0), 255);
			Pixel blurry_p = Pixel(red,green,blue);
			temp_img->SetPixel(x, y, blurry_p);
		}
	}

	/* Replace each pixel with one extrapolated from the original and
	   blurred images */
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			this->SetPixel(x, y, PixelLerp(this->GetPixel(x, y), temp_img->GetPixel(x, y), -1));
		}
	}
}

void Image::EdgeDetect()
{
	int x,y,filter_x,filter_y;
	Image * temp_img = new Image(Width(), Height());
	
	/* Create an edge detect filter! */
	float edge_detect_filter[3][3] = {{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};

	/* Fill temp_img with the result of applying the filter to
	   each pixel on the original image */
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			float red = 0, green = 0, blue = 0;
			for (filter_x = 0 ; filter_x < 3 ; filter_x++){
				for (filter_y = 0 ; filter_y < 3 ; filter_y++){
					int img_p_x = (x - 1 + filter_x + Width()) % Width();
					int img_p_y = (y - 1 + filter_y + Height()) % Height();
					red += this->GetPixel(img_p_x, img_p_y).r * edge_detect_filter[filter_x][filter_y];
					green += this->GetPixel(img_p_x, img_p_y).g * edge_detect_filter[filter_x][filter_y];
					blue += this->GetPixel(img_p_x, img_p_y).b * edge_detect_filter[filter_x][filter_y];
				}
			}
			red = fmin(fmax(red, 0), 255);
			green = fmin(fmax(green, 0), 255);
			blue = fmin(fmax(blue, 0), 255);
			Pixel edgey_p = Pixel(red,green,blue);
			temp_img->SetPixel(x, y, edgey_p);
		}
	}

	/* Replace the image with the edge-detected image */
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			this->SetPixel(x, y, temp_img->GetPixel(x, y));
		}
	}
}

Image* Image::Scale(double sx, double sy)
{
	int x,y;
	Image * scaled_img = new Image(sx*this->Width(), sy*this->Height());

	//if (sampling_method == 1){ /* point sampling */

		for (x = 0 ; x < scaled_img->Width() ; x++){
			for (y = 0 ; y < scaled_img->Height() ; y++){
				int orig_x = trunc(x/sx);
				int orig_y = trunc(y/sy);
				scaled_img->SetPixel(x, y, this->GetPixel(orig_x,orig_y));
			}
		}

	//} else if (sampling_method == 2){ /* bilinear sampling */
	//
	//	for (x = 0 ; x < scaled_img->Width() ; x++){
	//		for (y = 0 ; y < scaled_img->Height() ; y++){
	//			
	//	 	}
	//	}
	//
	//} else if (sampling_method == 3){ /* gaussian sampling */
	//
	//}

	return scaled_img;
}

Image* Image::Rotate(double angle)
{
	/* fix angle because my code rotates the wrong direction, oops */
	angle = -angle;

	/* calculate radii of original image */
	float x_r = (Width()-1)/2, y_r = (Height()-1)/2;

	/* find corners of rotated image centered on origin */
	float corner1_x = (-x_r)*cos(angle)-(-y_r)*sin(angle);
	float corner1_y = (-x_r)*sin(angle)+(-y_r)*cos(angle);
	float corner2_x = x_r*cos(angle)-(-y_r)*sin(angle);
	float corner2_y = x_r*sin(angle)+(-y_r)*cos(angle);
	float corner3_x = x_r*cos(angle)-y_r*sin(angle);
	float corner3_y = x_r*sin(angle)+y_r*cos(angle);
	float corner4_x = (-x_r)*cos(angle)-y_r*sin(angle);
	float corner4_y = (-x_r)*sin(angle)+y_r*cos(angle);
	
	/* determine width and height of rotated image canvas */
	int new_width = fmax(abs(corner1_x-corner3_x),abs(corner2_x-corner4_x));
	int new_height = fmax(abs(corner1_y-corner3_y),abs(corner2_y-corner4_y));

	/* create new image of appropriate width/height for rotated image to
	   be copied onto! */
	Image * rotated_img = new Image(new_width,new_height);

	/* calculate radii of new image image */
	float new_x_r = (new_width)/2, new_y_r = (new_height)/2;

	//if (sampling_method == 1){ /* point sampling */
	int x,y;
	for (x = 0 ; x < new_width ; x++){
		for (y = 0 ; y < new_height ; y++){
			/* center on origin */
			float new_x, new_y;
			new_x = x-new_x_r;
			new_y = y-new_y_r;
			/* un-rotate values: note, reverse rotation matrix is [ cos  sin]
																  [-sin  cos] */
			int unrotated_x = (int)(new_x*cos(angle)+new_y*sin(angle));
			int unrotated_y = (int)((-new_x)*sin(angle)+new_y*cos(angle));
			/* if value isn't on image, set to black. if it is, copy it to its
			   rotated position on the new canvas! */
			if ((unrotated_x < (-fabs(x_r))) || (unrotated_x > fabs(x_r)) || (unrotated_y < (-fabs(y_r))) || (unrotated_y > fabs(y_r))){
				rotated_img->SetPixel(x,y,Pixel(0,0,0,0));
			} else {
				rotated_img->SetPixel(x,y,this->GetPixel(unrotated_x+x_r,unrotated_y+y_r));
			}
		}
	}

	//} else if (sampling_method == 2){ /* bilinear sampling */

	//} else if (sampling_method == 3){ /* gaussian sampling */

	//}

	return rotated_img;
}

void Image::Fun() /* Zoom/pinch filter! */
{
	int x,y;
	float x_r = (Width()-1)/2, y_r = (Height()-1)/2;
	Image * pinch_img = new Image(Width(), Height());
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			/* restrain to [-1,1]x[-1,1]*/
			float new_x, new_y;
			int get_x, get_y;
			new_x = (x-x_r)/x_r;
			new_y = (y-y_r)/y_r;
			/* get polar coord */
			float r = sqrt(pow(new_x,2)+pow(new_y,2));
			float angle = atan2(new_y,new_x);
			/* get new r based on r' = r^2 (reverse-mapping means we need r) */
			float reverse_mapped_r = sqrt(r);
			/* revert reverse-mapped coords to x,y , outside [-1,1]x[-1,1] */
			float reverse_mapped_x = reverse_mapped_r*cos(angle);
			float reverse_mapped_y = reverse_mapped_r*sin(angle);
			get_x = (int)((reverse_mapped_x*x_r)+x_r)%Width();
			get_y = (int)((reverse_mapped_y*y_r)+y_r)%Height();
			pinch_img->SetPixel(x, y, this->GetPixel(get_x, get_y));
		}
	}

	/* Copy new image to original image */
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			this->SetPixel(x, y, pinch_img->GetPixel(x, y));
		}
	}

}

/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method)
{
    assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
    sampling_method = method;
}


Pixel Image::Sample (double u, double v){
    /* WORK HERE */
	return Pixel();
}