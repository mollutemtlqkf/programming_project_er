#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include "etc.h"
#include "OneSeasonViewer.h"
#include "multipleseason.h"
#include "getuser.h"
int rankSeason;


int main() {
    SetConsoleCP(65001);

    int mode = 0;
    while(mode != 777) {
        bootingmessage();
        printf("명령어 : ");
        scanf("%d", &mode);
        if (mode == 1) {
			oneseasonview();
        }
        else if(mode == 2) {
            printf("조회할 시즌의 범위를 적으시오 : ");
            int startseason, endseason;
            scanf("%d %d", &startseason, &endseason);
            multipleseason(startseason, endseason);
        }
        else if(mode == 3) {
            explainmessage();
        }
        else if (mode == 4) {
            printf("찾을 유저를 입력하세요 : ");
			char username[1557];
			scanf("%s", username);
			printf("조회할 시즌을 입력하세요 : ");
			scanf("%d", &rankSeason);
            getuserInfo(username,rankSeason);
		}
        else if (mode == 5) {
            printf("url 한국어 인코딩 테스트입니다\n");
			char testusername[] = "";
            SetConsoleCP(949);
            printf("닉네임을 입려하세요 : ");
			scanf("%s", testusername);
            int usernameLength = strlen(testusername); // username의 길이 계산
            char encodedUsername1[1000];
            int encodedIndex = 0; // 쓰기 전용 인덱스

            for (int i = 0; i < usernameLength; i++) { // username의 각 문자에 대해 반복
                unsigned char usernametemp = (unsigned char)testusername[i]; //한국어를 넣을때 음수가 나오지 않도록 하기

                if (isalnum(usernametemp)) { // 영숫자일 경우 그대로 복사
                    encodedUsername1[encodedIndex++] = usernametemp;

                }
                else { // 영숫자가 아닐 경우 퍼센트 인코딩
                    encodedUsername1[encodedIndex++] = '%'; // 퍼센트 기호 추가

                    // 16진수 변환
                    char fowardfourbit = (usernametemp >> 4); // 상위 4비트 추출
                    if (fowardfourbit > 9) { //10진수 이상일 경우 문자로 변환
                        encodedUsername1[encodedIndex++] = fowardfourbit + 'A' - 10; //A-F 문자 처리
                    }
                    else { //0-9일 경우 숫자로 변환
                        encodedUsername1[encodedIndex++] = fowardfourbit + '0'; //0-9 숫자 처리
                    }

                    char backfourbit = (usernametemp & 15); // 하위 4비트 추출
                    if (backfourbit > 9) { //10진수 이상일 경우 문자로 변환
                        encodedUsername1[encodedIndex++] = backfourbit + 'A' - 10; //A-F 문자 처리
                    }
                    else { //0-9일 경우 숫자로 변환
                        encodedUsername1[encodedIndex++] = backfourbit + '0'; //0-9 숫자 처리
                    }
                }
            }
            encodedUsername1[encodedIndex] = '\0'; // 문자열 종료 문자 추가
			printf("Euc-kr : %s\n", encodedUsername1);
            printf("똑같은 닉네임을 한번 더 입력하세요 : ");
			SetConsoleCP(65001);
			scanf("%s", testusername);
            usernameLength = strlen(testusername); // username의 길이 계산
            char encodedUsername[1000];
            encodedIndex = 0; // 쓰기 전용 인덱스

            for (int i = 0; i < usernameLength; i++) { // username의 각 문자에 대해 반복
                unsigned char usernametemp = (unsigned char)testusername[i]; //한국어를 넣을때 음수가 나오지 않도록 하기

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
			printf("Utf-8 : %s\n", encodedUsername);

        }
        else if (mode == 777) {
            printf("프로그램을 종료합니다\n");
            break;
        }
        else {
            printf("잘못된 입력입니다. 다시 입력해주세요\n");
		}
       
    }
}