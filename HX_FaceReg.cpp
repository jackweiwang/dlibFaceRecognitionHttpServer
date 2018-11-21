#include"HX_FaceReg.h"
#include <string.h>

using namespace std;
using namespace dlib;

#define pfilepath0 "../model/shape_predictor_68_face_landmarks.dat";
#define pfilepath1 "../model/dlib_face_recognition_resnet_model_v1.dat";

HX_FACE_REG::HX_FACE_REG()
{
}

HX_FACE_REG::~HX_FACE_REG()
{
}

FR_STATUS HX_FACE_REG::HX_FaceRegInit()
{
    FR_STATUS frstatus = FR_STATUS_OK;

    faceDetector = get_frontal_face_detector();

    string landmark_filepath = pfilepath0;
	deserialize(landmark_filepath) >> landmarkDetector;

	string netfilepath = pfilepath1;
	pNet = new anet_type;

	deserialize(netfilepath) >> (*pNet);
	
	if(!pNet)
		return frstatus = FR_STATUS_INIT_DESERIALIZL;

	return frstatus;
}

FR_STATUS  HX_FACE_REG::HX_FaceRegFree()
{
	FR_STATUS frstatus = FR_STATUS_OK;

	if(pNet)
	{
        delete pNet;
        pNet = 0;
	}

	return frstatus;
}

FR_STATUS HX_FACE_REG::HX_FaceDetection(array2d<bgr_pixel> &img, rectangle *pRect)
{
    FR_STATUS frstatus = FR_STATUS_OK;

    std::vector<rectangle> dets = faceDetector(img);

	if(!dets.size())
		return frstatus = FR_STATUS_NO_FOUND_FACE;
    int nSize = 0;
	for (int i = 0; i < dets.size(); i++)
	{
		int nsum = dets[i].width() + dets[i].height();

		if (nsum > nSize)
		{
			nSize = nsum;
			*pRect = dets[i];
		}
	}

    return frstatus;
}

 FR_STATUS HX_FACE_REG::HX_FaceAlignment(array2d<bgr_pixel> &img, rectangle *pRect, matrix<rgb_pixel> &faceNormal)
 {
    FR_STATUS frstatus = FR_STATUS_OK;

    auto shape = landmarkDetector(img, *pRect);

	extract_image_chip(img, get_face_chip_details(shape, FACE_W, 0.25), faceNormal);

    return frstatus;
 }

 FR_STATUS HX_FACE_REG::HX_FaceGetFeature(matrix<rgb_pixel> &face, double *pFeature)
 {
    FR_STATUS frstatus = FR_STATUS_OK;

    matrix<float, 0, 1> face_descriptors = (*pNet)(face);

    for (int i = 0; i < CODE_LEN; i ++)
	{
		pFeature[i] = face_descriptors(i,0);
	}

    return frstatus;
 }

 FR_STATUS HX_FACE_REG::HX_FaceCodeCompare(double *pFeature1, double *pFeature2, double *pDist)
 {
    FR_STATUS frstatus = FR_STATUS_OK;

    double dDist = 0.0;
	for (int i = 0; i < CODE_LEN; i ++)
	{
		dDist += ((pFeature1[i] - pFeature2[i]) * (pFeature1[i] - pFeature2[i]));
	}

	*pDist = sqrtf(dDist);

    return frstatus;
 }
