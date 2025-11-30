#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <Windows.h> 
#include "cJSON.h"
#include "getuser.h"
#define API_KEY  "G8qG8ACBH5RHr8j2H0kt866wdH8PSUV8kUdg4Ns3"
#define BUFFER_SIZE 10000

void getuserInfo(char* username,int season) {
	int rankseason = season * 2 + 17; //시즌 -> API용 시즌 코드 변환`


	int usernameLength = strlen(username); // username의 길이 계산
    char encodedUsername[1000];
    int encodedIndex = 0; // 쓰기 전용 인덱스

	for (int i = 0; i < usernameLength; i++) { // username의 각 문자에 대해 반복
        unsigned char usernametemp = (unsigned char)username[i]; //한국어를 넣을때 음수가 나오지 않도록 하기

		if (isalnum(usernametemp)) { // 영숫자일 경우 그대로 복사
            encodedUsername[encodedIndex++] = usernametemp;

        }
		else { // 영숫자가 아닐 경우 퍼센트 인코딩
			encodedUsername[encodedIndex++] = '%'; // 퍼센트 기호 추가

            // 16진수 변환
			char fowardfourbit = (usernametemp >> 4); // 상위 4비트 추출
			if (fowardfourbit > 9) { //10진수 이상일 경우 문자로 변환
				encodedUsername[encodedIndex++] = fowardfourbit + 'A' - 10; //A-F 문자 처리
            }
			else { //0-9일 경우 숫자로 변환
				encodedUsername[encodedIndex++] = fowardfourbit + '0'; //0-9 숫자 처리
            }

			char backfourbit = (usernametemp & 15); // 하위 4비트 추출
			if (backfourbit > 9) { //10진수 이상일 경우 문자로 변환
				encodedUsername[encodedIndex++] = backfourbit + 'A' - 10; //A-F 문자 처리
            }
			else { //0-9일 경우 숫자로 변환
				encodedUsername[encodedIndex++] = backfourbit + '0'; //0-9 숫자 처리
            }
        }   
    }
	encodedUsername[encodedIndex] = '\0'; // 문자열 종료 문자 추가
    FILE* ppopen; //어김없이 파이프


    size_t maxDataSize = 100000; //최대치까지 담을수 있도록 일단 많은 메모리 할당
    char* jsonData = (char*)malloc(maxDataSize); //JSON 데이터를 담을수 있도록 유동적인 메모리 할당
    char buffer[BUFFER_SIZE]; //임시 버퍼



    char command[1000]; //일단 넣을수 있도록 용량을 크게 한 빈 껍데기 준비

    
	snprintf(command, sizeof(command), "curl -s -X GET \"https://open-api.bser.io/v1/user/nickname?query=%s\" -H \"accept: application/json\" -H \"x-api-key: %s\"", encodedUsername, API_KEY); //빈껍데기에 템플릿을 바탕으로 시즌 데이터 넣기
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
	cJSON* message = cJSON_GetObjectItemCaseSensitive(jsonplate, "code"); //aPI 응답 코드 추출
	if (cJSON_IsNumber(message) && message->valueint == 404) { //응답 코드가 404일경우(존재하지 않는 유저)
		printf("\n[ERROR] 존재하지 않는 유저입니다. (Error 404)\n\n"); //오류 출력

        cJSON_Delete(jsonplate); // 메모리 정리
        free(jsonData);          // 메모리 정리
        return 0;                // 함수 종료
    }
    //정상적으로 불러왔다면
	cJSON* user = cJSON_GetObjectItemCaseSensitive(jsonplate, "user");  //user 객체 추출
	cJSON* userId = cJSON_GetObjectItemCaseSensitive(user, "userId"); // userId 추출
	//여기서 중요한점, 여기서 메모리를 해제하면 안됨. userId가 가리키는 메모리가 해제되기 때문
	
	FILE* ppopen2; //파이프가 2개지요
    int maxDataSize2 = 500000; //최대치까지 담을수 있도록 일단 많은 메모리 할당
    char* jsonData2 = (char*)malloc(maxDataSize2); //JSON 데이터를 담을수 있도록 유동적인 메모리 할당
	char buffer2[BUFFER_SIZE]; //임시 버퍼
        
    command[10000]; //일단 넣을수 있도록 용량을 크게 한 빈 껍데기 준비
	snprintf(command, sizeof(command), "curl -s -X GET \"https://open-api.bser.io/v2/user/stats/uid/%s/%d/3\" -H \"accept: application/json\" -H \"x-api-key: %s\" 2>&1", userId->valuestring, rankseason, API_KEY); //빈껍데기에 템플릿을 바탕으로 시즌 데이터 넣기
    fflush(stdout); //버퍼 비우기
    ppopen = _popen(command, "r"); //명령어 실행
    if (ppopen == NULL) { //명령어 실행 실패시
        perror("popen failed"); //오류 출력
        free(jsonData); //메모리 해제
        SetConsoleOutputCP(949); //원래 인코딩으로 복구
        return 1;
    }

    location = 0; //현재까지 읽은 데이터 길이
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


    jsonplate = cJSON_Parse(jsonData); //JSON 데이터 파싱
	message = cJSON_GetObjectItemCaseSensitive(jsonplate, "code"); //aPI 응답 코드 추출
	if (cJSON_IsNumber(message) && message->valueint == 404) { //응답 코드가 404일경우(존재하지 않는 유저)
		printf("\n[ERROR] 존재하지 않는 유저입니다1. (Error 404)\n\n"); //오류 출력

        cJSON_Delete(jsonplate); // 메모리 정리
        free(jsonData);          // 메모리 정리
        return 0;                // 함수 종료
    }
	cJSON* userStatsArray = cJSON_GetObjectItemCaseSensitive(jsonplate, "userStats"); //userStats 배열 추출
	cJSON* userInfo = cJSON_GetArrayItem(userStatsArray, 0); //유저정보 추출
   
    SetConsoleOutputCP(65001); //username 출력을 위해 인코딩 변경
    printf("\n\n### %s ", username);
	SetConsoleOutputCP(949); //한글 출력을 위해 인코딩 변경
    printf("시즌 %d 랭크통계\n", season); //총 랭크 수 출력

   SetConsoleOutputCP(65001); //api 출력을 위해 인코딩 변경
  
   cJSON* totalGames = cJSON_GetObjectItemCaseSensitive(userInfo, "totalGames"); //랭크 게임 횟수 추출
       cJSON* totalWins = cJSON_GetObjectItemCaseSensitive(userInfo, "totalWins"); //승리 횟수 추출
       cJSON* totalTeamKills = cJSON_GetObjectItemCaseSensitive(userInfo, "totalTeamKills"); //총 TK 추출
       cJSON* totalDeaths = cJSON_GetObjectItemCaseSensitive(userInfo, "totalDeaths"); //총 데스 추출
       cJSON* escapeCount = cJSON_GetObjectItemCaseSensitive(userInfo, "escapeCount"); //탈출 횟수 추출
       cJSON* averageRank = cJSON_GetObjectItemCaseSensitive(userInfo, "averageRank"); //평균 순위 추출
       cJSON* averageKills = cJSON_GetObjectItemCaseSensitive(userInfo, "averageKills"); //평균 킬 추출
       cJSON* averageAssistants = cJSON_GetObjectItemCaseSensitive(userInfo, "averageAssistants"); //평균 어시스트 추출
       cJSON* averageHunts = cJSON_GetObjectItemCaseSensitive(userInfo, "averageHunts"); //평균 야생동물 처치수 추출
       cJSON* top1 = cJSON_GetObjectItemCaseSensitive(userInfo, "top1"); //1등 비율 추출
       cJSON* top2 = cJSON_GetObjectItemCaseSensitive(userInfo, "top2"); //2등
       cJSON* top3 = cJSON_GetObjectItemCaseSensitive(userInfo, "top3"); //3
       cJSON* top5 = cJSON_GetObjectItemCaseSensitive(userInfo, "top5"); //5
       cJSON* top7 = cJSON_GetObjectItemCaseSensitive(userInfo, "top7"); //7
       cJSON* mmr = cJSON_GetObjectItemCaseSensitive(userInfo, "mmr"); //rp 추출
       cJSON* rank = cJSON_GetObjectItemCaseSensitive(userInfo, "rank"); //등수 추출
	   int userrank = rank->valueint; //등수 변수에 저장
	   int mmrvalue = mmr->valueint; //rp 변수에 저장
	   char rankTier[64] = ""; //티어 문자열 변수

	   if (mmrvalue < 600) { //아이언 티어 구간
           if (mmrvalue < 150)
			   strcpy(rankTier, "Iron 4");
           else if (mmrvalue < 300) strcpy(rankTier, "Iron 3");
           else if (mmrvalue < 450) strcpy(rankTier, "Iron 2");
           else strcpy(rankTier, "Iron 1");
       }

	   else if (mmrvalue < 1400) { //브론즈 티어 구간
           if (mmrvalue < 800) strcpy(rankTier, "Bronze 4");
           else if (mmrvalue < 1000) strcpy(rankTier, "Bronze 3");
           else if (mmrvalue < 1200) strcpy(rankTier, "Bronze 2");
           else strcpy(rankTier, "Bronze 1");
       }

	   else if (mmrvalue < 2400) { //실버 티어 구간
           if (mmrvalue < 1650) strcpy(rankTier, "Silver 4");
           else if (mmrvalue < 1900) strcpy(rankTier, "Silver 3");
           else if (mmrvalue < 2150) strcpy(rankTier, "Silver 2");
           else strcpy(rankTier, "Silver 1");
       }
	   else if (mmrvalue < 3600) { //골드 티어 구간
           if (mmrvalue < 2700) strcpy(rankTier, "Gold 4");
           else if (mmrvalue < 3000) strcpy(rankTier, "Gold 3");
           else if (mmrvalue < 3300) strcpy(rankTier, "Gold 2");
           else strcpy(rankTier, "골드 1");
       }
	   else if (mmrvalue < 5000) { //플래티넘 티어 구간
           if (mmrvalue < 3950) strcpy(rankTier, "Platinum 4");
           else if (mmrvalue < 4300) strcpy(rankTier, "Platinum 3");
           else if (mmrvalue < 4650) strcpy(rankTier, "Platinum 2");
           else strcpy(rankTier, "Platinum 1");
       }

	   else if (mmrvalue < 6400) { //다이아몬드 티어 구간
           if (mmrvalue < 5350) strcpy(rankTier, "Diamond 4");
           else if (mmrvalue < 5700) strcpy(rankTier, "Diamond 3");
           else if (mmrvalue < 6050) strcpy(rankTier, "Diamond 2");
           else strcpy(rankTier, "Diamond 1");
       }

	   else if (mmrvalue < 7200) { //메테오라이트 티어 구간
           strcpy(rankTier, "Meteorite");
       }
	   else if (mmrvalue < 7900) { //미스릴 티어 구간
           strcpy(rankTier, "Mythril");
       }
	   else {//데미갓/ 이터니티 구간
		   if (userrank <= 300) //상위 300위 이내면 이터니티
               strcpy(rankTier, "Eternity");
		   else //그 외는 데미갓
			   strcpy(rankTier, "Demigod");
       }
       SetConsoleOutputCP(65001); //API 출력을 위해 인코딩 변경
       printf("\n===========================================================================================\n");
       printf(" Default Infomation in Ranked\n");
       printf("=============================================================================================\n");
       printf("Times Played In Ranked : %d\n", totalGames->valueint);
       printf("Tier: %s\n", rankTier);
       printf("Current RP: %d\n", mmr->valueint);
       printf("Current Rank Position: %d\n", userrank);

       printf("Times at survived in Ranked : %d", totalWins->valueint);
       printf("\n===========================================================================================\n");
       printf(" Statistics in Game\n");
       printf("=============================================================================================\n");
       printf("Total TeamKill and Deaths\n");
       printf("TK / D \n");
       printf("%d / %d\n\n", totalTeamKills->valueint, totalDeaths->valueint);

	   printf("Average Kills / Average Assists\n");
	   printf("%d / %d\n\n", averageKills->valueint, averageAssistants->valueint);
	   printf("Average Rank Place : %.2f\n\n", averageRank->valuedouble);
	   printf("Times Escaped : %d\n\n", escapeCount->valueint);
	   printf("Average Wild Animals Hunted : %.2f\n", averageHunts->valuedouble);

	   printf("\n===========================================================================================\n");
       printf(" Rank percentage");
       printf("\n===========================================================================================\n");
	   printf("1st place : %.2f%%\n", top1->valuedouble*100);
	   printf("upper than 2nd place : %.0f%%\n", top2->valuedouble*100);
	   printf("upper than 3rd place : %.0f%%\n", top3->valuedouble*100);
	   printf("upper than 5th place : %.0f%%\n", top5->valuedouble*100);
	   printf("upper than 7th place : %.0f%%\n", top7->valuedouble*100);


      
	   char* subjectJson = subjectCodeRead("subjects.json"); // subjects.json 파일에서 캐릭터 코드-이름 읽어오기
	   cJSON* characterStats = cJSON_GetObjectItemCaseSensitive(userInfo, "characterStats"); //characterStats 배열 추출
	   cJSON* subjects = cJSON_Parse(subjectJson); // subjects.json 파싱

           printf("\n===========================================================================================\n");
           printf(" Primary Subjects\n");
           printf("=============================================================================================\n");
           printf(" Subject Name | Times Played | Times Survived | Most Kills | Upper than 3rd place Count | Average rank\n");
           printf("=============================================================================================\n");

           
		   for (int i = 0; i < 3; i++) { //3개만 출력하기에 3번 반복
			   cJSON* stats = cJSON_GetArrayItem(characterStats, i); //i번째 캐릭터 통계 추출


			   cJSON* subjectCode = cJSON_GetObjectItemCaseSensitive(stats, "characterCode"); // 캐릭터 코드
			   cJSON* totalGames = cJSON_GetObjectItemCaseSensitive(stats, "totalGames"); // 플레이 횟수
			   cJSON* wins = cJSON_GetObjectItemCaseSensitive(stats, "wins"); // 생존 횟수
			   cJSON* maxKillings = cJSON_GetObjectItemCaseSensitive(stats, "maxKillings"); // 최대 킬 수
			   cJSON* top3 = cJSON_GetObjectItemCaseSensitive(stats, "top3"); // 3위 이내 횟수
			   cJSON* avgRank = cJSON_GetObjectItemCaseSensitive(stats, "averageRank"); // 평균 순위

               char subjectName[32]; // 캐릭터 이름 담을 공간
			   char subjectCodeStr[16];  // 문자열로 변환한 캐릭터 코드


    
			   sprintf(subjectCodeStr, "%d", subjectCode->valueint); // 캐릭터 코드를 문자열로 변환

  
			   cJSON* nameItem = cJSON_GetObjectItemCaseSensitive(subjects, subjectCodeStr); // subjects.json에서 캐릭터 이름 찾기


				   strncpy(subjectName, nameItem->valuestring, sizeof(subjectName) - 1); // 이름 복사
				   subjectName[sizeof(subjectName) - 1] = '\0'; // 널 종료 문자 추가


               if (totalGames) {
				   printf(" %s | %d Games | %d Survived | %d Kills | %d | %d\n", subjectName, totalGames->valueint, wins->valueint, maxKillings->valueint, top3->valueint, avgRank->valueint); // 통계 출력
               }
           }
           printf("\n\n");
		   free(subjectJson);
		   cJSON_Delete(subjects);
		   free(jsonData);
		   cJSON_Delete(jsonplate); //모든 메모리 해제


		   SetConsoleOutputCP(949); // 인코딩 복구

}

char* subjectCodeRead(const char* filename) { // subjects.json 파일 읽기
	FILE* f = fopen(filename, "r");// 파일 열기
	fseek(f, 0, SEEK_END); // 파일 끝으로 이동
	long length = ftell(f); // 파일 길이 얻기
	fseek(f, 0, SEEK_SET); // 파일 시작으로 이동
	char* buffer = (char*)malloc(length + 1); // 버퍼 할당
	if (buffer) { // 할당 성공 시
		fread(buffer, 1, length+1, f); // 파일 내용 읽기
		buffer[length] = '\0'; // 널 종료 문자 추가
    }
	fclose(f);// 파일 닫기
	return buffer; // 버퍼 반환
}