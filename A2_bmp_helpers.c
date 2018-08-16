/* FILE: A2_bmp_helpers.c is where you will code your answers for Assignment 2.
 * 
 * Each of the functions below can be considered a start for you. 
 *
 * You should leave all of the code as is, except for what's surrounded
 * in comments like "REPLACE EVERTHING FROM HERE... TO HERE.
 *
 * The assignment document and the header A2_bmp_headers.h should help
 * to find out how to complete and test the functions. Good luck!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

int bmp_open( char* bmp_filename,        unsigned int *width, 
              unsigned int *height,      unsigned int *bits_per_pixel, 
              unsigned int *padding,     unsigned int *data_size, 
              unsigned int *data_offset, unsigned char** img_data ){

           
  FILE* file;
	if((file = fopen(bmp_filename, "rb")) == NULL){
		return -1;
	}

        //To hold the header information
	char header[56];
	fread(header,sizeof(char),56,file);

	//getting information from header
	*width=*(int *)&header[18];
	*height=*(int *)&header[22];
	*bits_per_pixel=*(int *)&header[28];
	*data_offset=*(int *)&header[10];
	*padding=(4-(((*bits_per_pixel)*(*width)/8)%4))==4?0:(4-(((*bits_per_pixel)*(*width)/8)%4));	
	*data_size=*(int *)&header[2];
	rewind(file);

	//Getting info from BMP file to img_data
	*img_data=(unsigned char*)malloc(*data_size);
	unsigned int now='n';
	int count=0;
	while(now!=EOF){
		now=fgetc(file);
		char* subst=(*img_data)+count;
		*subst=now;
		count++;
	}
	fclose(file);
  return 0;  
}

// We've implemented bmp_close for you. No need to modify this function
void bmp_close( unsigned char **img_data ){

  if( *img_data != NULL ){
    free( *img_data );
    *img_data = NULL;
  }
}

int bmp_mask( char* input_bmp_filename, char* output_bmp_filename, 
              unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned int y_max,
              unsigned char red, unsigned char green, unsigned char blue )
{
  unsigned int img_width;
  unsigned int img_height;
  unsigned int bits_per_pixel;
  unsigned int data_size;
  unsigned int padding;
  unsigned int data_offset;
  unsigned char* img_data    = NULL;

  int open_return_code = bmp_open( input_bmp_filename, &img_width, &img_height, &bits_per_pixel, &padding, &data_size, &data_offset, &img_data ); 
  
  if( open_return_code ){ printf( "bmp_open failed. Returning from bmp_mask without attempting changes.\n" ); return -1; }

  FILE* fp;
  fp=fopen(output_bmp_filename,"w");
  int row_length=(img_width*bits_per_pixel/8)+(padding);
  for(int j=y_min;j<=y_max;j++){
  int count=0;
  for(int i=data_offset+(j*row_length);i<data_offset+((j+1)*row_length);i=i+3){
	if(count>x_min&&count<x_max){
	img_data[i]=red;
	img_data[i+1]=green;
	img_data[i+2]=blue;}
	count++;
  }}

  fwrite(img_data,sizeof(unsigned char),data_size,fp);
  bmp_close( &img_data );
  
  return 0;
}         

int bmp_collage( char* bmp_input1, char* bmp_input2, char* bmp_result, int x_offset, int y_offset ){

  unsigned int img_width1;
  unsigned int img_height1;
  unsigned int bits_per_pixel1;
  unsigned int data_size1;
  unsigned int padding1;
  unsigned int data_offset1;
  unsigned char* img_data1    = NULL;
  
  int open_return_code = bmp_open( bmp_input1, &img_width1, &img_height1, &bits_per_pixel1, &padding1, &data_size1, &data_offset1, &img_data1 ); 
  
  if( open_return_code ){ printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input1 ); return -1; }
  
  unsigned int img_width2;
  unsigned int img_height2;
  unsigned int bits_per_pixel2;
  unsigned int data_size2;
  unsigned int padding2;
  unsigned int data_offset2;
  unsigned char* img_data2    = NULL;
  
  open_return_code = bmp_open( bmp_input2, &img_width2, &img_height2, &bits_per_pixel2, &padding2, &data_size2, &data_offset2, &img_data2 ); 
  
  if( open_return_code ){ printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input2 ); return -1; }
  

  FILE* fp;
  fp=fopen(bmp_result,"w");

  //Declaring values for third image
  if(bits_per_pixel1!=bits_per_pixel2){
   return -1;
}
  unsigned int img_width3;
  unsigned int img_height3;
  unsigned int bits_per_pixel3=bits_per_pixel2;
  unsigned int data_size3;
  unsigned int padding3;
  unsigned int data_offset3=data_offset2;//Header 2 is being used
  unsigned char* img_data3    = NULL;
  
  //Calculating new width and height and getting start values for img1 and img2
  int min_left;
  int max_right;
  int max_up;
  int min_down;
  int xStart1;
  int yStart1;
  int xStart2;
  int yStart2;

//min_left is the min of 0,X_offset
  if(x_offset>0){
  min_left=0;
  xStart1=0;
  xStart2=x_offset;
}else{
  min_left=x_offset;
  xStart1=-x_offset;
  xStart2=0;
}

//min_down is the min of 0,X_offset
  if(y_offset>0){
  min_down=0;
  yStart1=0;
  yStart2=y_offset;
}else{
  min_down=y_offset;
  yStart1=-y_offset;
  yStart2=0;
}

//max_right is the max of img_width1 AND x_offset+img_width2
if(img_width1>x_offset+img_width2){
  max_right=img_width1;
}else{
  max_right=x_offset+img_width2;
}

//max_up is the max of img_height1 AND y_offset+img_height2
if(img_height1>y_offset+img_height2){
  max_up=img_height1;
}else{
  max_up=y_offset+img_height2;
}
  img_width3=max_right-min_left;
  img_height3=max_up-min_down;

  //Calculating padding and data size and then writing to img_data3
  padding3=(4-(((bits_per_pixel3)*(img_width3)/8)%4))==4?0:(4-(((bits_per_pixel3)*(img_width3)/8)%4));
  data_size3=data_offset3+(((img_width3*bits_per_pixel3/8)+padding3)*img_height3);
  img_data3=(unsigned char*)malloc(data_size3);
  memcpy(img_data3,img_data2,data_offset2);
  memcpy(img_data3+2,(unsigned char*)&data_size3,4);
  memcpy(img_data3+18,(unsigned char*)&img_width3,4);
  memcpy(img_data3+22,(unsigned char*)&img_height3,4);

  
  int row_length1=(img_width1*bits_per_pixel1/8);
  int row_length2=(img_width2*bits_per_pixel2/8);
  int row_length3=(img_width3*bits_per_pixel3/8)+(padding3);
  int a=data_offset1;
  int no_of_rows=0;

  //Writing first image
  while(no_of_rows<img_height1){
  for(int i=data_offset3+(yStart1*row_length3)+(3*xStart1);i<data_offset3+(yStart1*row_length3)+(3*xStart1)+row_length1;i++){
	img_data3[i]=img_data1[a];
	a++;
  }
 a=a+padding1;
 yStart1++;
 no_of_rows++;
}

  //Writing second file
  a=data_offset2;
  no_of_rows=0;
  while(no_of_rows<img_height2){
  for(int i=data_offset3+(yStart2*row_length3)+(3*xStart2);i<data_offset3+(yStart2*row_length3)+(3*xStart2)+row_length2;i++){
	img_data3[i]=img_data2[a];
	a++;
  }
 a=a+padding2;
 yStart2++;
 no_of_rows++;
}
  //Writing to file and closing
  fwrite(img_data3,sizeof(unsigned char),data_size3,fp);
  bmp_close( &img_data1 );
  bmp_close( &img_data2 );
  bmp_close( &img_data3 );
  return 0;
}                  
