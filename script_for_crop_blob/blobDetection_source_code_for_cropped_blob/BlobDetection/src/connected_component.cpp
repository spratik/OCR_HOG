/*
 * connected_component.cpp
 *
 *  Created on: May 27, 2016
 *      Author: pratik
 */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <enum.h>
using namespace std;
using namespace cv;

/*
 *  class
 */
Mat image;

class ConnectedComponent
{
public:

	/// declare the pointer for  the image pixels information
	int  *G_pixel_buffer;

	/// declare pointer which are connected to each other
	int  *G_connected_points;

	/// declare pointer for row stack buffer  which is used for storing
	/// the row element of image into the stack
	int  *G_row_buffer;

	/// declare pointer for col stack buffer which is used for storing
	/// the col element of the image into the stack
	int  *G_col_buffer;

	/// declare the double pointer for make the rectangular box of each element.
	int **G_bounding_box;

	/// declare int variable for counter purpose,
	/// how many element are present to make the bounding box
	int   G_box;

	/// declare pointer for histogram
	int  *G_histogram;

	/// declare the variable for common col stack index
	int   G_colStack;

	/// declare the variable for common row stack index
	int   G_rowStack;

	/// declare the int variable for assign each component
	/// to different values  this variable is useful for component leveling
	int   G_level;


	/* allocating 
	 *	memory
	 */

	int initialize (const int row, const int col)
	{
		/// find the size of the image
		int size = row * col ;

		/// allocate the memory for pixel bufferr
		G_pixel_buffer     = (int *)calloc(size, sizeof(int));

		/// Check for proper allocation
		if(G_pixel_buffer == NULL)      return MemoryNotAllocated;


		/// allocating memory for connected component buffer 
		G_connected_points = (int *)calloc(size ,sizeof(int));

		/// check proper allocation
		if(G_connected_points == NULL)  return MemoryNotAllocated;



		/// allocating memory for histogram
		G_histogram        = (int *)calloc(size ,sizeof(int));

		/// check proper allocation
		if(G_histogram == NULL)		return MemoryNotAllocated;


		/// allocating memoru for row stack buffer
		G_row_buffer       = (int *)calloc(size ,sizeof(int));

		/// check proper allocation     
		if(G_row_buffer == NULL) 	return MemoryNotAllocated;



		/// allocating memoru for col stack buffer
		G_col_buffer       = (int *)calloc(size ,sizeof(int));

		/// check proper allocation
		if(G_col_buffer == NULL) 	return MemoryNotAllocated;

		/// allocating memoru for bounding box 
		G_bounding_box     = (int **)calloc(size ,sizeof(int *));

		/// check proper allocation 
		if(G_bounding_box == NULL)      return MemoryNotAllocated;			

		/// initilize with zero because box variable is further useful for count how many rectangle box we need
		G_box              = 0;

		/// level is useful for component leveling purpose
		G_level            = 0;

		/// index of the column stack	
		G_colStack         =-1;

		/// index of the row stack	
		G_rowStack         =-1;

		///
		return Success;
	}

	/// here find the hsitogram,binaryimage,connected componet and bounding box
	void ProcessImage (Mat & ,int ,int ,int ,char *);

	/// display the original,connected component , bounding box image
	void DisplayImage (Mat & , char * ,char *, int , int , int );

	/// release the all the memory which are allocated

	void release()
	{
		// release pixel_buffer memory
		free(G_pixel_buffer);

		// release connected points memory
		free(G_connected_points);

		// release row buffer memory
		free(G_row_buffer);

		// release column buffer memory
		free(G_col_buffer);

		// release bounding box memory
		for(int i = 0; i < G_box ; i++){
			free(G_bounding_box[i]);
		}

		///
		free(G_bounding_box);

		/// release histogram memory
		free(G_histogram);

	}



};

/// create the bounding box
void BoundingBox (ConnectedComponent * ,const int ,const int );

/// dump the data into the file
void dumpDataIntoFile (ConnectedComponent * ,int ,int ,int ,char *);

// get the binary image
void getBinaryImage(ConnectedComponent *, int );

// find the 8 neighbour
void findEightNeighbour (ConnectedComponent *, int , int ,int &, int &, int &, int &, int );

/// main function start here
int main(int argc ,char **argv)
{
	if(argc != 3){

		cout << "Usage : EXE input_image Output_folder_path" << endl;
		return 0;

	}
	int rs = Success;
	//! read the image
	image = imread(argv[1], 1);
	//! find the rows values from the image
	int rows = image.rows;
	//! find the cols values from the image
	int cols = image.cols;

	//! find the size of the image
	int size = rows * cols;

	//! this is useful for create the window

	/*!
	 *   where flage = 1  is indicate that create the window according to size of the image
	 */
	//namedWindow("original_image",1);

	//! create the object
	ConnectedComponent obj;

	/// Allocate or initialize memory
	rs = obj.initialize (rows, cols);

	/// check proper allocation
	if(rs == MemoryNotAllocated)	return rs;

	char cmd[2048];
	sprintf(cmd, "mkdir %s", argv[2]);
	system(cmd);

	///  process the image like obtain the gray image buffer then obtain the binary image buffer from gray image then find the connected component then draw the reactangle box of each connected componenent
	obj.ProcessImage(image, size , rows , cols ,argv[2]);

	/// display the original , connected component image , bounding box image
	//obj.DisplayImage(image , argv[2] ,argv[3] ,cols ,rows , size);


	/// release the memory
	obj.release();


	return 0;

}


/* boundign box 
 * is draw the rectangle box of each connected component
 */
void BoundingBox (ConnectedComponent *obj ,const int cols ,const int rows ,char *bounding_box_file)
{
	int count = 0;
	/// store the MIN. value of row
	int Rmin;

	/// store the MIN. value of col
	int Cmin;

	/// store the WIDTH of the bounding box
	int width;

	/// store the height of the bounding box
	int height;

	/// store the index of the matrix
	int index;

	/// store the MAX. value of row
	int Rmax;

	/// store the MAX. value of col
	int Cmax;
	char buf[1024];

	int x = 0;
	int y = 0;

	/// scan the each connected component and draw the reactangle box
	for(int i = 0; i < obj-> G_box ; i++)
	{
		/// take the Cmin of the connected component
		Cmin   = obj-> G_bounding_box[i][0];

		/// take Rmin  of the connected component
		Rmin   = obj-> G_bounding_box[i][1];

		/// take width of the connected component
		width  = obj-> G_bounding_box[i][2];

		/// take height of the connected component
		height = obj-> G_bounding_box[i][3];


		if(Cmin - 2 >= 0 ){
			x = Cmin - 2;
		}else{
			x = Cmin;
		}

		if(Rmin - 2 >= 0 ){
			y = Rmin - 2;
		}else{
			y = Rmin;
		}

		if(width + 4 <= image.cols ){
			width = width + 4;
		}
		if(height + 4 <= image.rows ){
			height = height + 4;
		}



		Mat croppedImage(image,Rect(x,y,width , height ));
		if(!croppedImage.data){
			cout << "Image read error" << endl;
			return;
		}

		sprintf(buf, "%s/image_%d.png",bounding_box_file,count++);
		imwrite(buf,croppedImage);
		croppedImage.release();

	}


}

/* dump buffer data
 *  into the file
 */

void dumpDataIntoFile (ConnectedComponent *obj ,int size ,int cols ,int rows ,char *output)
{

	/// create the file for dumping the data
	ofstream myfile(output);

	/// dump P1 first into it for create the binary image
	myfile <<  "P3" << endl;

	/// dump the width and height of the image
	myfile <<  cols  <<  " " << rows << endl;


	/// scan the image for create the binary file	
	for(int i = 0; i < size ; i++ )
	{
		/// every row put one '\n' for new line for proper showing data
		if(i%cols == 0){
			myfile << endl;
		}

		/// check each index of the connected component buffer with not equal to zero if it is ture then dump one into file
		if(obj-> G_connected_points[i]!=0){
			myfile << "1" << " ";
			/// otherwise dump zero into the file
		}else{ 
			myfile << "0" << " ";
		}

	}

	/// close the file
	myfile.close();
}

void ConnectedComponent::ProcessImage (Mat &image ,int size ,int rows ,int cols ,char *bounding_box_file)
{


	/// create the 3 channel for red ,green,and blue image
	Mat  channel[3];

	//! then spilt the image into three differ channel
	split(image,channel);

	/// blue pointer hold the starting address of the blue image
	uchar *Blue  = channel[0].data;

	/// green pointer hold the starting address of the green image
	uchar *Green = channel[1].data;

	/// red pointer hold the starting address of the red image
	uchar *Red   = channel[2].data;


	/********************************************//**
	 *  .........gray-image............
	 ***********************************************/

	/*!
	 * gray image is average of the red,green,blue image channel.
	 *  so take the avg. of the three channel store each values into buffer
	 */


	for(int i = 0; i < size; i++){
		//! now G_pixel_buffer has all the gray image pixel values
		G_pixel_buffer[i]=  ((Red[i]) + (Green[i]) + (Blue[i])) / 3;
	}


	/********************************************//**
	 *  ............histogram...........
	 ***********************************************/

	/*!
	 * An image histogram is a graphical representation of the number of pixels in an image as a function of their intensity.
	 */

	for(int i = 0; i < size; i++) {
		/*! G_histogram has count values that how many time particular intensity value present in the image.
		 *   G_histogram is further helpful for finding the binary image.
		 */

		///   NOTE: image reperesent in 8 bit so range between 0 - 255
		G_histogram[G_pixel_buffer[i]]++;
	}


	/// get the binary image and store all the binary data into the G_pixel_buffer
	getBinaryImage(this ,size);


	/********************************************//**
	 *  ..........connected_component....
	 ***********************************************/


	/// declare the variables for finding the 8 neighbour

	///      declare variable for top left index
	int TL;

	///      declare variable for top index
	int TP;

	///      declare variable for top right index
	int TR;

	///      declare variable for Middle left index
	int ML;

	///      declare variable for Middle right index
	int MR;

	///      declare variable for bottom left index
	int BL;

	///      declare variable for bottom index
	int BT;

	///      declare variable for bottom right index
	int BR;

	/// declare variable for  store the bounding box data


	/// declare the varibales for store bouding box data

	///        declare the variable for store the colmn MIN. data  
	int col_min;

	///        declare the variable for store the colmn MAX. data  
	int col_max;

	///        declare the variable for store the row MIN. data  
	int row_min;

	///        declare the variable for store the row MIN. data  
	int row_max;


	///local variable for store the row data
	int row;

	///local variable for store the col data
	int col;

	/// declare the variable for store index of matrix
	int index;


	/// * Scan the image row wise for faster scaning.

	for(int r = 1; r < rows-1; r++)
	{
		for(int c = 1; c < cols-1 ;c++)
		{
			///       -get the index values as per the row and col vlaue
			index = r * cols + c;

			///       -check the index  of the G_pixel_buffer whether the pixel is object pixel or not for that every time check pixel value is one or not

			///       -if it is one then enter into the loop.
			if(G_pixel_buffer[index] == 1)
			{

				///           --make that index of the G_pixel_buffer to zero because next time scan at that we won't be consider
				G_pixel_buffer[index] = 0;

				///           --assign one G_level to that index of the connected_points (NOTE: for component leveling purpose each time when new element is found at that time assign different value)
				G_connected_points[index] = ++G_level;

				///            --here, find the all the connnected point present in image by using depth first search(DFS)


				///            --for DFS, increment the G_rowstack by one
				G_rowStack = G_rowStack + 1 ;

				///             --store the current row value into the G_row_buffer
				G_row_buffer[G_rowStack] = r;


				///             --same as column ,increment the G_colStack by one
				G_colStack = G_colStack + 1;

				///             --store the current col value into the G_col_buffer
				G_col_buffer[G_colStack] = c;


				///             * intilize bounding box variable data


				///               	--- assign the current column value into col_min and col_max
				col_min = col_max = c;

				///                     --- assign the current row value into row_min and row_max
				row_min = row_max = r;

				///         	* this while loop is useful for finding how many pixels are connected to each other for that we have to check the 8 neighbours of the above index

				///               - check the every time row_stack and col_stack are empty or not because we get the current current rowStack and colStack values and decrease the rowStack and colStack by one
				while(G_rowStack != -1 && G_colStack != -1)
				{

					///			--  pop row_buffer value store into the row variable
					row        = G_row_buffer[G_rowStack];

					///                     --  decrement G_rowStack counter by one
					G_rowStack = G_rowStack - 1;

					///                     -- pop col_buffer value store into the col variable
					col        = G_col_buffer[G_colStack];

					/// -- decrement  G_colStack counter by one
					G_colStack = G_colStack - 1;


					/// -- get the index value by using the current row and col value of the matrix
					index   = row * cols + col;

					/// --get TOP-LEFT index by using index and width of the image (NOTE: cols is nothing but width only)
					TL = index - cols -1;

					/// --get TOP index by using index and width of the image
					TP = index - cols;

					/// --get TOP index by using index and width of the image
					TR = index - cols + 1;

					/// --get TOP RIGHT index by using index and width of the image
					ML = index - 1;

					///                           --get MIDDLE LEFT index by using index and width of the image
					MR = index + 1;

					///                           --get MIDDLE RIGHT index by using index and width of the image
					BL = index + cols - 1;

					///                           --get BOTTOM LEFT index by using index and width of the image
					BT = index + cols ;

					///                           --get BOTTOM RIGHT index by using index and width of the image
					BR = index + cols + 1;




					///	check the TOP LEFT index value or not if it is one then again push row and col data into respective buffer
					if(row - 1 > 0 && col - 1 > 0){
						findEightNeighbour(this , TL ,cols ,col_min ,row_min ,col_max ,row_max , size);
					}
					///	-check the TOP index value is one or not if it is one then again push row and col data into respective buffer
					if(row - 1 > 0){
						findEightNeighbour(this , TP ,cols ,col_min ,row_min ,col_max ,row_max , size);
					}
					///-check the TOP RIGHT index value is one or not if it is one then again push row and col data into respective buffer
					if(row - 1 > 0 && col + 1 < cols - 1){
						findEightNeighbour(this ,TR ,cols ,col_min ,row_min ,col_max ,row_max ,size);
					}
					///	-check the MIDDLE LEFT index value is one or not if it is one then again push row and col data into respective buffer
					if(col - 1 > 0 ){
						findEightNeighbour(this ,ML ,cols ,col_min ,row_min ,col_max ,row_max ,size);
					}
					///	-check the MIDDLE RIGHT index value is one or not if it is one then again push row and col data into respective buffer
					if(col + 1 < cols - 1){
						findEightNeighbour(this ,MR ,cols ,col_min ,row_min ,col_max ,row_max ,size);
					}
					///					-check the BOTTOM LEFT index value is one or not if it is one then again push row and col data into respective buffer
					if(row + 1 < rows - 1 && col - 1 > 0){
						findEightNeighbour(this ,BL ,cols ,col_min ,row_min ,col_max ,row_max ,size);
					}
					///					-check BOTTOM  index value is one or not if it is one then again push row and col data into respective buffer
					if(row + 1 < rows -1 ) {
						findEightNeighbour(this ,BT ,cols ,col_min ,row_min ,col_max ,row_max ,size);
					}
					///					-check the BOTTOM RIGHT index value is one or not if it is one then again push row and col data into respective buffer
					if( row + 1 < rows - 1  && col + 1 < cols - 1 ){
						findEightNeighbour(this ,BR ,cols ,col_min ,row_min ,col_max ,row_max ,size) ;
					}

				}

				/// now store the rows,cols,width,height of the element into G_bounding_box buffer which is further useful for make rectangal box of element

				G_bounding_box[G_box] = (int *) calloc( 4, sizeof(int) );

				G_bounding_box[G_box][0]   = col_min;           // col
				G_bounding_box[G_box][1]   = row_min;           // row
				G_bounding_box[G_box][2]   = col_max - col_min; // width
				G_bounding_box[G_box++][3] = row_max - row_min; // height

			}


		}

	}

	/// follow above steps until get the all the connected component.

	/// at the end all the connected component values  into the G_connected_points

	/// now dump G_connected_points into the file, for display purpose
	//dumpDataIntoFile(this ,size ,cols ,rows ,connected_component_file);

	/// call the BoundingBox function for make the rectangle to each element
	BoundingBox (this ,cols ,rows ,bounding_box_file);
}



/* DISPLAYIMAGE  function
 *
 * is useful for display the orignal ,connected_component , bounding box image.
 *
 */

void ConnectedComponent :: DisplayImage(Mat &image , char *connected_component_file ,char *bounding_box_file ,int cols , int rows , int size)
{

	/// display the orignal image
	//imshow("original_image",image);

	/// * read the connected comonent file 
	//Mat CC = imread(connected_component_file);

	///    -create the window according to the size of the image for that put flag is 1
	//namedWindow("connected_points" ,1);

	///    - display the connected component image
	//imshow("connected_points", CC);

	/// * read the bounding box image 
	//Mat BB = imread(bounding_box_file);

	///     -create the window according to the size of the image for that put flag is 1
	//namedWindow("bounding_box" ,1);

	///    - display the bounding box image
	//imshow("bounding_box", BB);


	/// wait key is useful hold the displaied image on the screen if we press any key then that displaied image is vanished.
	//waitKey(0);

}

/* findEightNeighbour
 *
 *	is useful for check the 8 neighbour of the index and check the particular index.
 */

void findEightNeighbour (ConnectedComponent *obj, int index, int width ,int &col_min, int &row_min, int &col_max, int &row_max, int size)
{

	/// declare the local variable for the hold the row value
	int r1;

	/// declare the local variable for the hold the row value
	int c1;

	/// here, check the particular index value if it is true then eneter into the loop
	if(obj-> G_pixel_buffer[index] == 1)
	{

		///  - set that index value to zero
		obj-> G_pixel_buffer[index] = 0;


		///   - assign current G_level value into the current index of G_connected_points buffer.
		obj-> G_connected_points[index] = obj-> G_level;


		///    - get the row value from the index
		r1=(int)(index/width);

		///     -get the col value from the index
		c1=index-(r1*width);


		///     - increment the rowStack count value by one
		obj-> G_rowStack = obj-> G_rowStack + 1;

		///     - store the current row value into the G_row_buffer
		obj-> G_row_buffer[ obj-> G_rowStack ] = r1;

		///      - increment the colStack count value by one
		obj-> G_colStack = obj-> G_colStack + 1;

		///      -store the current col value into the G_col_buffer
		obj-> G_col_buffer[obj-> G_colStack] = c1;



		/// *find the area of the component and later according to the area draw the rectangle box

		///    -check evey time col_min grater than current column value if it is ture then assign the assign current column value to col_min ,so every time we get the minmum value of the colmn
		if(col_min > c1) col_min = c1;

		///    -check evey time row_min grater than current row value if it is ture then assign the assign current row value to row_min ,so every time we get the minmum value of the row.

		///    -check evey time col_max less than current column value if it is ture then assign the assign current column value to col_max ,so every time we get the maximum value of the colmn.
		if(col_max < c1) col_max = c1;

		///    -check evey time row_max current than current row value if it is ture then assign the assign current row value to row_max ,so every time we get maximum value of the row.
		if(row_max < r1) row_max = r1;
	}

}

/*! OTSU thresholding is used for get the binary image
 *   we have to follow some step to get the binary image
 */

void getBinaryImage(ConnectedComponent *obj, int size)
{


	/// intilize the total variable with size
	long long int total = size;

	/// intilize sum to zero
	long long int sum   = 0;

	/// intilize q1 to zero
	long long int q1    = 0;

	/// intilize q2 to zero
	long long int q2    = 0;

	/// intilize SUM to zero
	float SUM           = 0;

	/// intilize u1 to zero
	float u1            = 0;

	/// intilize u2 to zero
	float u2            = 0;

	/// intilize result to zero
	float result        = 0;

	/// intilize var_max to zero 
	float var_max       = 0;

	/// intitlize threshold to zero
	int threshold       = 0;



	/// auxiliary value for computing u2
	for(int i = 0; i < 256 ;i++)
		sum = sum + i * obj-> G_histogram[i];

	for(int t = 0; t < 256 ; t++)
	{	///update qi(t)
		q1 = q1 + obj-> G_histogram[t];
		/// according to the qi(t) ,get the qi(t) value
		q2 = total - q1;

		/// update ui(t)
		SUM  = SUM + t * obj-> G_histogram[t];

		u1 = SUM / q1 ;

		u2 = (sum - SUM) / q2;

		/// update the between-class variance
		result =  q1 * q2 * (u1 - u2) * (u1 - u2);

		/// update the threshold
		if(result > var_max)
		{
			threshold = t;
			var_max   = result;
		}


	}


	/// build the segmented image
	for(int i = 0; i < size; i++)
	{

		if(obj-> G_pixel_buffer[i] > threshold){
			obj-> G_pixel_buffer[i] = 0;
		}else{
			obj-> G_pixel_buffer[i] = 1;
		}

	}


}
/*
opencv_calib3d
opencv_contrib
opencv_core
opencv_features2d
opencv_flann
opencv_gpu
opencv_highgui
opencv_imgproc
opencv_legacy
opencv_ml
opencv_objdetect
opencv_ocl
opencv_photo
opencv_stitching
opencv_superres
opencv_ts
opencv_video
opencv_videostab
 */
