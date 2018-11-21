#ifndef HX_FACEREG_H_INCLUDED
#define HX_FACEREG_H_INCLUDED

#ifdef _cplusplus
#extern "C"{
#endif

#include <dlib/string.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#define FACE_W 150
#define FACE_H 150
#define CODE_LEN 128


using namespace dlib;
using namespace std;

//----------------------------------------------------------------------------------------------------------------------------------
template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N, BN, 1, tag1<SUBNET>>>;

template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2, 2, 2, 2, skip1<tag2<block<N, BN, 2, tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block = BN<con<N, 3, 3, 1, 1, relu<BN<con<N, 3, 3, stride, stride, SUBNET>>>>>;

template <int N, typename SUBNET> using ares = relu<residual<block, N, affine, SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block, N, affine, SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256, SUBNET>;
template <typename SUBNET> using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128, avg_pool_everything<
	alevel0<
	alevel1<
	alevel2<
	alevel3<
	alevel4<
	max_pool<3, 3, 2, 2, relu<affine<con<32, 7, 7, 2, 2,
	input_rgb_image_sized<150>
	>>>>>>>>>>>>;
//----------------------------------------------------------------------------------------------------------------------------------


/// 人脸识别状态码
typedef enum FR_STATUS
{
	FR_STATUS_OK = 0,  ///< 成功
	FR_STATUS_OPEN_IMAGE_FAIL = -6,  ///< 打开图片失败
	FR_STATUS_NO_FOUND_FACE = -4,  ///< 没找到人脸
	FR_STATUS_PARAMETER_INVALID = -1,  ///< 参数无效
	FR_STATUS_PARAMETER_OUT_OF_BOUND = -2, ///< 参数越界
	FR_STATUS_PARAMETER_NOT_ENOUGH_SYSTEM_MEMORY = -3,  ///< 内存不足
	FR_STATUS_INIT_DESERIALIZL = -5  ///< 初始化均衡化失败
};


class HX_FACE_REG
{
public:
	HX_FACE_REG();
	~HX_FACE_REG();

	FR_STATUS HX_FaceRegInit();

    FR_STATUS HX_FaceRegFree();

    FR_STATUS HX_FaceDetection(array2d<bgr_pixel> &img, rectangle *pRect);

    FR_STATUS HX_FaceAlignment(array2d<bgr_pixel> &img, rectangle *pRect, matrix<rgb_pixel> &faceNormal);

    FR_STATUS HX_FaceGetFeature(matrix<rgb_pixel> &face, double *pFeature);

    FR_STATUS  HX_FaceCodeCompare(double *pFeature1, double *pFeature2, double *pDist);

private:

	dlib::frontal_face_detector faceDetector;
	dlib::shape_predictor landmarkDetector;
	anet_type *pNet;
};


#ifdef _cplusplus
}
#endif

#endif // HX_FACEREG_H_INCLUDED
