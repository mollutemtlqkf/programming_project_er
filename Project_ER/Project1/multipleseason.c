#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "multipleseason.h"
#include "etc.h"

#define API_KEY  "G8qG8ACBH5RHr8j2H0kt866wdH8PSUV8kUdg4Ns3"
#define BUFFER_SIZE 300000 


struct userInfo {
    int UserNum;
    char nickname[16];
    int mmr;
    int totalrankings;
    int totalrp;
    int averagerp;
};


struct userInfo userInfo[11000];

int multipleseason(int start, int end) {
    int startseasonID = start * 2 + 17;
    int endseasonID = end * 2 + 17;

    // [유지 및 수정] seasoncount 로직 사용
    int seasoncount, startseason;

    if (startseasonID - endseasonID > 0) {
        seasoncount = (startseasonID - endseasonID) / 2 + 1;
        startseason = endseasonID; 
    }
    else {
        seasoncount = (endseasonID - startseasonID) / 2 + 1;
        startseason = startseasonID; 
    }

     

    FILE* ppopen;
    int structcount = 0;
    size_t maxDataSize = BUFFER_SIZE;

    // 메모리 할당 (반복문 밖)
    char* jsonData = (char*)malloc(maxDataSize);
    char buffer[BUFFER_SIZE];

    if (jsonData == NULL) {
        printf("Data getting failed\n");
        SetConsoleOutputCP(949);
        return 1;
    }

    char command[10000];


    for (int i = 0; i < seasoncount; i++) {

        int currentSeasonID = startseason + (i * 2);

        printf("Processing Season ID: %d...\n", currentSeasonID);

        memset(jsonData, 0, maxDataSize);


        snprintf(command, sizeof(command), "curl -s -X GET \"https://open-api.bser.io/v1/rank/top/%d/3\" -H \"accept: application/json\" -H \"x-api-key: %s\" 2>&1", currentSeasonID, API_KEY);

        fflush(stdout);

        ppopen = _popen(command, "r");

        int location = 0;
        while (fgets(buffer, sizeof(buffer), ppopen) != NULL) {
            int bufferlength = strlen(buffer);

            if (location + bufferlength >= maxDataSize - 1) {
                printf("\nData Overflow\n");
                break;
            }
            memcpy(jsonData + location, buffer, bufferlength);
            location += bufferlength;
        }
        _pclose(ppopen);

        cJSON* jsonplate = cJSON_Parse(jsonData);
        cJSON* message = cJSON_GetObjectItemCaseSensitive(jsonplate, "code");
        if (cJSON_IsNumber(message) && message->valueint == 404) {
            SetConsoleOutputCP(949); // 인코딩 복구
            printf("\n[ERROR] %d시즌은 존재하지 않는 시즌입니다. 해당 시즌은 패스합니다(Error 404)\n\n",((startseasonID+(2*i)-17)/2));
             
            continue;

        }
        cJSON* rankObj = cJSON_GetObjectItemCaseSensitive(jsonplate, "topRanks");
        if (cJSON_IsArray(rankObj)) {

            int arraySize = cJSON_GetArraySize(rankObj);

            for (int k = 0; k < arraySize; k++) {
                cJSON* item = cJSON_GetArrayItem(rankObj, k);

                cJSON* nickname = cJSON_GetObjectItemCaseSensitive(item, "nickname");


                int userexists = 0;
                for (int j = 0; j < structcount; j++) {
                    if (strcmp(userInfo[j].nickname, nickname->valuestring) == 0) {
                        cJSON* mmr = cJSON_GetObjectItemCaseSensitive(item, "mmr");
                        if (mmr) {
                            userInfo[j].totalrp += mmr->valueint;
                            userInfo[j].totalrankings += 1;
                            userInfo[j].averagerp = userInfo[j].totalrp / userInfo[j].totalrankings;
                        }
                        userexists = 1;
                        break;
                    }
                }

                if (userexists == 0) {

                    if (structcount >= 11000) break;

                    cJSON* mmr = cJSON_GetObjectItemCaseSensitive(item, "mmr");
                    cJSON* userNum = cJSON_GetObjectItemCaseSensitive(item, "userNum");

                    strcpy(userInfo[structcount].nickname, nickname->valuestring);

                    if (userNum) userInfo[structcount].UserNum = userNum->valueint;
                    if (mmr) {
                        userInfo[structcount].mmr = mmr->valueint;
                        userInfo[structcount].totalrp = mmr->valueint;
                        userInfo[structcount].averagerp = mmr->valueint;
                    }
                    userInfo[structcount].totalrankings = 1;
                    structcount++;
                }
            }
        }
        cJSON_Delete(jsonplate);
        Sleep(1000);
		printf("Season ID: %d processed. API on cooldown \n", currentSeasonID);
    } 


    free(jsonData);
    struct userInfo temp; 
    for (int i = 0; i < structcount - 1; i++) {
        for (int j = 0; j < structcount - 1 - i; j++) {
            // 앞의 유저보다 뒤의 유저 RP가 더 크면 자리를 바꿈 (내림차순)
            if (userInfo[j].totalrp < userInfo[j + 1].totalrp) {
                temp = userInfo[j];
                userInfo[j] = userInfo[j + 1];
                userInfo[j + 1] = temp;
            }
        }
    }

    // [결과 출력] 표 형식으로 출력

    printf("\n========================================================================\n");
	SetConsoleOutputCP(949);
    printf(" %s | %s | %s | %s | %s \n", "등수", "실험체 명", "총 RP", "평균 RP", "랭킹에 든 횟수");
    printf("========================================================================\n");
    SetConsoleOutputCP(65001);
    for (int i = 0; i < structcount; i++) {
        printf(" %d | %s | %d | %d | %d \n",i + 1,  userInfo[i].nickname, userInfo[i].totalrp,  userInfo[i].averagerp, userInfo[i].totalrankings);
    }
    printf("========================================================================\n");

    printf("\nAll processing done. Total Users: %d\n", structcount);
    SetConsoleOutputCP(949);

    return structcount;
}

