#include "std_include.h"

using namespace cv;
using namespace std;

TIFF* tiff;
int active_frame=0;

void displayFrame(int, void*);
void preprocess(Mat in, Mat &out);
void dynamicMinMax(Mat in, Mat &out, int xf, int yf);

int main(int argc, char** argv) {
        
    tiff = TIFFOpen(argv[1], "r");

    cout<<"\nREADING FILE...\n\n";

    int dircount = 0;
    if (tiff) {
	do {
	    dircount++;
	} while (TIFFReadDirectory(tiff));
    }
    cout<<dircount<<" frames in image"<<endl<<endl;

    cout<<"Starting visualization..."<<endl;

    namedWindow("Image", 1);

    string trackname("Frame");

    createTrackbar(trackname, "Image", &active_frame, dircount, displayFrame);

    displayFrame(active_frame, 0);

    waitKey(0);
    return 0;

}

void displayFrame(int, void*) {

    Mat img;
    uint32 c, r;
    size_t npixels;
    uint32* raster;
    
    TIFFSetDirectory(tiff, active_frame);
    
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &c);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &r);
    npixels = r * c;
    raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if (raster != NULL) {
        if (TIFFReadRGBAImageOriented(tiff, c, r, raster, ORIENTATION_TOPLEFT, 0)) {
            img.create(r, c, CV_32F);
            for (int i=0; i<r; i++) {
                for (int j=0; j<c; j++) {
                    img.at<float>(i,j) = TIFFGetR(raster[i*c+j])/255.0;
                }
            }
        }
        _TIFFfree(raster);
    }
    
    Mat out;
    //preprocess(img, out);
    imshow("Image", img);
    
}

void preprocess(Mat in, Mat &out) {
   
    threshold(in, in, 20, 0, THRESH_TOZERO);
    //qimshow(in);

    Mat im2;
    dynamicMinMax(in, im2, 40, 40); 
    //qimshow(im2);

    GaussianBlur(im2, im2, Size(3,3), 1.0);
    //qimshow(im3);

    Mat im3;
    dynamicMinMax(im2, im3, 40, 40);
    //qimshow(out);

    threshold(im3, im3, 100, 0, THRESH_TOZERO);

    Mat im4;
    dynamicMinMax(im3, out, 40, 40);

}

void dynamicMinMax(Mat in, Mat &out, int xf, int yf) {

    int xs = in.cols/xf;
    int ys = in.rows/yf;

    if (xs*xf != in.cols || ys*yf != in.rows)
        cout<<endl<<"Wrong divide factor. Does not lead to integer window sizes!"<<endl;

    out.create(in.rows, in.cols, CV_8U);

    for (int i=0; i<xf; i++) {
        for (int j=0; j<yf; j++) {
            
            Mat submat = in(Rect(i*xs,j*ys,xs,ys)).clone();
            Mat subf; submat.convertTo(subf, CV_32F);
            SparseMat spsubf(subf);

            double min, max;            
            minMaxLoc(spsubf, &min, &max, NULL, NULL);
            min--;
            if (min>255.0) min = 0;
            subf -+ min; subf /= max; subf *= 255;
            subf.convertTo(submat, CV_8U);

            submat.copyTo(out(Rect(i*xs,j*ys,xs,ys)));

        }
    }

}
