
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <process.h> //used for thread

/**
 * UTF-16 character type
 */
//#ifndef __STDC_UTF_16__
#    ifndef CHAR16_T
#        define CHAR16_T unsigned short
typedef CHAR16_T char16_t;
#    endif
//#endif

#include "mex.h"
#include "cv.h"
#include "highgui.h"

void cdecl webcam_Thread(LPVOID pVoid);

//######################################################
//##############--main mex function--########################
//######################################################
void mexFunction(int nlhs, mxArray *plhs,
        int nrhs, mxArray *prhs[]) {
    int ThreadNr; //needed for the thread
    
//start your thread
    _beginthread( webcam_Thread, 0, &ThreadNr );
    
//----for testing----
//Sleep(20000); //will make the mex function stay around for 20 seconds so you can see the mexPrintfs of the thread if you want
}

//################################################
//##########--Thread Code--#########################
//################################################
void cdecl webcam_Thread(LPVOID pVoid) {
    
//declare your variables/pointers
    int x=0; //used to adjust # of while loops
    mxArray *myarray = 0; //used to help extract your matlab variable's value#
    mxArray * locked_matlab = 0;
    mxArray * stopped_matlab = 0;
    const mxArray *locked_c;
    
    //double *TempPointer; //used to help extract your matlab variable's value
    
    locked_matlab = mxCreateDoubleMatrix(1, 1, mxREAL);
    stopped_matlab = mxCreateDoubleMatrix(1, 1, mxREAL);
    
    double * locked = (double*)mxCalloc(1, sizeof(double));
    locked[0] = 0.0;
    mxSetData(locked_matlab, locked);
    mexPutVariable("base", "locked", locked_matlab);

    int dims[] = {240, 320, 3};
    char16_t * image = (char16_t*)mxCalloc(320*240*3, sizeof(char16_t));
    
//     double * ptr = (double*)mxCalloc(1, sizeof(double));
//     ptr[0] = 2.0F;
//
//     test = mxCreateDoubleMatrix(1, 1, mxREAL);
//     mxSetData(test, ptr);
//     mexPutVariable("base", "test", test);
    
            //Allocate memory and assign output pointer
            
            myarray = mxCreateCharArray(3, (const int*)dims); //mxChar is our data-type
            mxSetData(myarray, image);   
            
    CvCapture *capture = 0;
    IplImage  *frame = 0;
    int       key = 0;
    
//----Setup your pointers before entering the thread's loop, in this way we will not have to talk
//----to matlab at all once we enter the thread (we will be simply accessing the matlab
//----variables memory location). This is useful because a lot of really fast communication with
//----matlab can crash it.
//get a pointer to a pointer to the hullo variable (which you should initialize in matlab before running this prog)
//myarray = mexGetVariablePtr("base", "hullo"); //can be base caller or global
//get the hullo variable's value from the pointers, store it in Val
//TempPointer = mxGetPr(myarray);
    
    /* initialize camera */
    capture = cvCaptureFromCAM( 0 );
    
    /* always check */
    if ( !capture ) {
        fprintf( stderr, "Cannot open initialize webcam!\n" );
        return;
    }
    
    /* create a window for the video */
    cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );
    
    while( key != 'q' ) {
        /* get a frame */
        frame = cvQueryFrame( capture );
        
        /* always check */
        if( !frame ) break;
        
        int height = frame->height;
        int width  = frame->width;
        
        //mexPutVariable("base", "locked", locked_matlab);
        
        for( int px = 0; px < width; px++) {
            for( int py = 0; py < height; py++) {
                image[py + px*height + 2*width*height] =((uchar*)(frame->imageData + frame->widthStep*py))[px*3];
                image[py + px*height + width*height] =((uchar*)(frame->imageData + frame->widthStep*py))[px*3+1];
                image[py + px*height] =((uchar*)(frame->imageData + frame->widthStep*py))[px*3+2];
            }
        }
        locked_c = mexGetVariablePtr("base", "locked"); //can be base caller or global
        //get the hullo variable's value from the pointers, store it in Val
        locked = mxGetPr(locked_c);
        mxSetData(locked_matlab, locked);
        if ( locked[0] < 0.5) {
            
            mexPutVariable("base", "image", myarray);
            
            locked[0] = 1.0;
            mexPutVariable("base", "locked", locked_matlab);
        }
        /* display current frame */
        cvShowImage( "result", frame );
        
        /* exit if user press 'q' */
        key = cvWaitKey( 1 );
    }
    
    double * stopped = (double*)mxCalloc(1, sizeof(double));
    stopped[0] = 1.0;
    mxSetData(stopped_matlab, stopped);
    mexPutVariable("base", "stopped", stopped_matlab);
    
    //mxFree(image);
    //mxFree(stopped);
    //mxFree(locked);
    
    //mxDestroyArray(locked_matlab);
    //mxDestroyArray(stopped_matlab);
    //mxDestroyArray(myarray);
    cvDestroyWindow( "result" );
    cvReleaseCapture( &capture );
    
    /*while(x<10)
     * {
     * x++;
     *
     * //here we are accessing the matlab variable's memory location without having to talk to matlab
     * Val = *TempPointer;
     * //TempPointer[0] = 2.0F;
     *
     * //play a sound
     * PlaySound("alleluia.wav", NULL, SND_FILENAME | SND_SYNC);
     * //wait in between sounds for Val seconds
     * Sleep(1000*Val);
     *
     * //----for testing----
     * //mexPrintf(" Val:%f", Val);
     */
    
}
