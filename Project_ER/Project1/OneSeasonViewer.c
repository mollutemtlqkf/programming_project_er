#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "OneSeasonViewer.h"
#define API_KEY  "G8qG8ACBH5RHr8j2H0kt866wdH8PSUV8kUdg4Ns3"
#define BUFFER_SIZE 800000 //최대치까지 담을수 있도록 일단 많은 메모리 할당 근데 왜 800000이 되고 8884844가 안되지?


int oneseasonview() {
	printf("현제는 시즌9(마츠리) 입니다.\n조회할려는 시즌을 입력해주세요 : ");
	int season;
	scanf("%d", &season); 
	season = season * 2 + 17; //시즌 -> API용 시즌 코드 변환
    SetConsoleOutputCP(65001); //API 결과 출력을 위해 UTF-8로 인코딩 변경

	FILE* ppopen; //출력 스트림용 파이프


	size_t maxDataSize = BUFFER_SIZE; //최대치까지 담을수 있도록 일단 많은 메모리 할당
	char* jsonData = (char*)malloc(maxDataSize); //JSON 데이터를 담을수 있도록 유동적인 메모리 할당
	char buffer[BUFFER_SIZE]; //임시 버퍼

    if (jsonData == NULL) { // 아무 응답도 받지 못했을떄
        printf("Data getting failed\n");
		SetConsoleOutputCP(949); //원래 인코딩으로 복구
        return 1;
    }


    char command[200000]; //일단 넣을수 있도록 용량을 크게 한 빈 껍데기 준비
    snprintf(command, sizeof(command), "curl -s -X GET \"https://open-api.bser.io/v1/rank/top/%d/3\" -H \"accept: application/json\" -H \"x-api-key: %s\" 2>&1", season,API_KEY); //빈껍데기에 템플릿을 바탕으로 시즌 데이터 넣기
	printf("Connecting to API...\n"); //API 연결 시도
	fflush(stdout); //버퍼 비우기

	ppopen = _popen(command, "r"); //명령어 실행
	if (ppopen == NULL) { //명령어 실행 실패시
		perror("popen failed"); //오류 출력
		free(jsonData); //메모리 해제
		SetConsoleOutputCP(949); //원래 인코딩으로 복구
        return 1;
    }

	int location = 0; //현재까지 읽은 데이터 길이
	while (fgets(buffer, sizeof(buffer), ppopen) != NULL) { //출력 스트림에서 한 줄씩 읽기
		int bufferlength = strlen(buffer); //읽은 데이터 길이

		if (location + bufferlength >= maxDataSize - 1) {//데이터가 최대치보다 커질경우
			printf("\nData Overflow\n"); //오류 출력
            break;
        }

		memcpy(jsonData + location, buffer, bufferlength); //읽은 데이터를 jsonData에 복사


		location += bufferlength; //현재까지 읽은 데이터 길이 갱신
    }

	_pclose(ppopen); //출력 스트림 닫기


	cJSON* jsonplate = cJSON_Parse(jsonData); //JSON 데이터 파싱

    //여기서 정상적으로 랭크 데이터를 불러왔을때

	//API 응답은 총 4개를 반환함, 응답코드, userNum(쉽게 말하면 UID),닉네임, 랭크 순위, MMR(RP 개념), UserAmblem(유저 엠블럼은 빈 값으로만 반환되어서 무시)
	cJSON* rankObj = cJSON_GetObjectItemCaseSensitive(jsonplate, "topRanks");  //topRanks 항목에서 랭크 데이터 배열
	if (cJSON_IsArray(rankObj)) { //rankObj가 배열인지 확인
		printf("이터널 리턴 %d 시즌 랭킹",season); //총 랭크 수 출력

		for (int i = 0; i < 1001; i++) { //모든 랭킹 돌기 -> api는 1001개까지 반환함
			cJSON* item = cJSON_GetArrayItem(rankObj, i); //i번째 유저 추출
            cJSON* rank = cJSON_GetObjectItemCaseSensitive(item, "rank"); //랭크 순위 추출
			cJSON* nickname = cJSON_GetObjectItemCaseSensitive(item, "nickname"); //닉네임 추출
			cJSON* mmr = cJSON_GetObjectItemCaseSensitive(item, "mmr"); //MMR 추출

			if (cJSON_IsNumber(rank) && cJSON_IsString(nickname) && cJSON_IsNumber(mmr)) {//각 항목 타입 확인
				printf("[Rank %d] %-20s (MMR: %d)\n", rank->valueint, nickname->valuestring, mmr->valueint); //랭크 정보 출력
            }
        }
    }

	cJSON_Delete(jsonplate); //파싱된 JSON 객체 메모리 해제
	free(jsonData); //할당된 메모리 해제


	printf("\n");
	SetConsoleOutputCP(949); //원래 인코딩으로 복구

    return 0;
}