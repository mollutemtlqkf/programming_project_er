#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

char reseach_player(char username[16])
{
    CURL* curl;       
    CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL); //curl 전역 초기화

	curl = curl_easy_init(); //curl 생성
    if (curl) {

        curl_easy_setopt(curl, CURLOPT_URL, "API주소"); //API 주소를 통해 접속

		res = curl_easy_perform(curl); //요청 수행

        if (res != CURLE_OK) { // 만약 제대로 접속이 안되고 오류가 발생하면
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));//오류 내용 출력
        }
		curl_easy_cleanup(curl); //curl 종료 free랑 같은 역할을 수행
    }
	curl_global_cleanup(); //curl 전역 종료
}
