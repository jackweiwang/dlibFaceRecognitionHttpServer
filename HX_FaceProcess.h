#include<stdio.h>

extern "C"
{
	__attribute ((visibility("default"))) int ToUrlgetFaceReg(char *svrUrl, double *dFeature, char *dstFeatureChar);
}
/*
typedef int (*FUN)(char *svrUrl, double *dFeature);

FUN GetFun(char *svrUrl, double *dFeature);
*/
