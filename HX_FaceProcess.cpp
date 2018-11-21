#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<sstream>
#include<vector>
#include"HX_FaceReg.h"
#include"HX_FaceProcess.h"

using namespace std;

#define ZOOM_IMAGE 320
extern "C"
{

int ToUrlgetFaceReg(char *svrUrl, double *dFeature, char *dstFeatureChar)
{
	cv::Mat srcimage;
	FR_STATUS ret_status = FR_STATUS_OK;
	srcimage = cv::imread(svrUrl);
    if(!srcimage.data)
    {
	printf("open image fail = %s\n",svrUrl);
    	return ret_status = FR_STATUS_OPEN_IMAGE_FAIL;
    }
    

	int nImgWidth = srcimage.cols;  //获取图像宽度
        int nImgHeight = srcimage.rows;  // 获取图像高度

	double zoom = (double)(ZOOM_IMAGE) / (double)(nImgWidth);
	cv::Mat image = srcimage.clone();
	cv::resize(srcimage, image, cv::Size(), zoom, zoom, cv::INTER_LINEAR);
  	HX_FACE_REG hx_FaceReg;
	printf("dst ==image width =%d--image height = %d\n", image.cols, image.rows);


	ret_status = hx_FaceReg.HX_FaceRegInit();
    if( ret_status != 0 )
    {

		hx_FaceReg.HX_FaceRegFree();	
		return ret_status = FR_STATUS_INIT_DESERIALIZL;
	}
		
	dlib::array2d<bgr_pixel> img;
   	dlib::rectangle rect;
	dlib::assign_image(img, dlib::cv_image<bgr_pixel>(image));

   	ret_status = hx_FaceReg.HX_FaceDetection(img, &rect);
	if(ret_status != 0)
	{
	    hx_FaceReg.HX_FaceRegFree();	
	    return ret_status = FR_STATUS_NO_FOUND_FACE;
	}

	int x = rect.left()/ zoom;
	int y = rect.top() / zoom;
	int width= rect.width() / zoom;
	int height= rect.height() / zoom;
	printf("x = %d,y=%d, width= %d, height = %d\n", x, y, width, height);
    //-----------------------------------------------------------------------

    ///---------------------  face alignment --------------------------------
    matrix<rgb_pixel> faceNormal(FACE_H, FACE_W);

    ret_status = hx_FaceReg.HX_FaceAlignment(img, &rect, faceNormal);
    if( ret_status != 0 )
    {
		return ret_status = FR_STATUS_NO_FOUND_FACE;
	}
    //-----------------------------------------------------------------------

    ///---------------------  face get feature ------------------------------

    ret_status = hx_FaceReg.HX_FaceGetFeature(faceNormal, dFeature);
    if( ret_status != 0 )
    {
		return ret_status;
	}


	hx_FaceReg.HX_FaceRegFree();	
		
    int i = 0;
    char DstData[128][20] = {0};
	char RectData[20] = {0};

    std::string p_data;

	sprintf(RectData, "%d %d %d %d ", x, y, width, height);	
	p_data = reinterpret_cast<char *>(RectData);
    for(i = 0; i < CODE_LEN; i++)
    {	
    	sprintf(DstData[i], "%lf ", dFeature[i]);    
        p_data += reinterpret_cast<char*>(DstData[i]);  
    }
    
	int length = p_data.length();
	//cout<<"dst  name  ==-------------" <<  p_data << length << endl;
	strncpy(dstFeatureChar, p_data.c_str(), length);
	return 0;
}

}
