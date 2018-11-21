/*
	Onion HTTP server library
	Copyright (C) 2010 David Moreno Montero

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <onion/http.h>
#include <onion/https.h>
#include <onion/log.h>
#include <onion/onion.h>
#include <onion/dict.h>
#include <onion/handler.h>
#include <onion/request.h>
#include <onion/response.h>
#include <signal.h>
#include <time.h>
#include <curl/curl.h>
#define URL_NAME "url"
#define CODE_LEN 128
#define IMG_TEMP_NAME "./image/temp.jpg"
/// 服务器返回数据错误码
typedef enum server_status
{
	SERVER_STATUS_OK = 0,  ///< 成功
	SERVER_STATUS_URL_ERROR = -1,  ///< url路径错误
	SERVER_STATUS_NO_PARAMETER = -2, ///< 没有参数
	SERVER_STATUS_NO_FOUND_FACE = -4,  ///< 没找到人脸
	SERVER_STATUS_INIT_DESERIALIZL = -3  ///< 初始化均衡化失败
}SERVER_STATUS;
/**
 * @short Just asks the user for the answer.
 */
void ImageDownloader(const char* image_url, const char* save_address)
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    FILE* fp = fopen(save_address,"wb");
	if(!fp)
	{
		ONION_INFO("open image fail\n");
	}
    res = curl_easy_setopt(curl, CURLOPT_URL, image_url);
    if(res != CURLE_OK)
    {
        curl_easy_cleanup(curl);
        return;
    }
    res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    res = curl_easy_perform(curl);
    fclose(fp);

    curl_easy_cleanup(curl);
}
onion_connection_status ask_handler(void *none, onion_request *req, onion_response *res){
	
	double Feature[513] = {0};
	char ImageName[100] = {0};
	char ImageData[2048] = {0};
	char resuImageData[2049] = {0};
	char UrlName[512] = {0};
	SERVER_STATUS status;
	status = SERVER_STATUS_NO_PARAMETER;
	const char *user_data=onion_request_get_query(req, URL_NAME);	
	if(user_data)
	{
		char *suffix = strrchr(user_data, '.');
		sprintf(ImageName, "../image/temp%s", suffix);
		sprintf(UrlName, "%s?x-oss-process=image/auto-orient,1",user_data);
		ImageDownloader( UrlName, ImageName );
		int start = clock();
		status = ToUrlgetFaceReg(ImageName, Feature, ImageData);
		int end = clock();
		ONION_INFO("detect time = %d", (end-start)/CLOCKS_PER_SEC);
	}
	else
	{
		return OCS_PROCESSED;
	}

	sprintf(resuImageData,"%d %s\n",status, ImageData);
	onion_response_write0(res, resuImageData);
	
	memset(ImageData, 0, sizeof(ImageData));
	memset(resuImageData, 0, sizeof(resuImageData));
	memset(ImageName, 0, sizeof(ImageName));
	memset(Feature, 0, sizeof(Feature));
	memset(UrlName, 0, sizeof(UrlName));
	return OCS_PROCESSED;
}

/**
 * @short In a simple loop asks the user for the answer of the query.
 */
int main(int argc, char **argv){
	onion *server;
	//onion_request *request;
	server=onion_new(O_POOL);
	//request = onion_request_new(server);
	onion_set_hostname(server, "192.168.100.162");
	
	//onion_add_listen_point(server, "localhost", "8080", onion_http_new());
	onion_set_root_handler(server, onion_handler_new((void*)ask_handler, NULL, NULL));
	//onion_set_root_handler(server, onion_handler_export_local_new("."));
	//onion_add_listen_point(server, "localhost", "8080", onion_http_new());

#ifdef HAVE_GNUTLS
	onion_add_listen_point(server, "localhost", "4443", onion_https_new(O_SSL_CERTIFICATE_KEY, "cert.pem", "cert.key"));
#else
	ONION_WARNING("HTTPS support is not enabled. Recompile with gnutls");
#endif
	//ONION_INFO("Listening at http://192.168.1:8080");

	//onion_set_port(server, "8081");
	
	onion_listen(server);
	
	onion_free(server);
	
	return 0;
}
