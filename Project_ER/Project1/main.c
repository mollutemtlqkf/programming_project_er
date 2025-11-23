#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include "etc.h"
#include "OneSeasonViewer.h"


struct userInfo {
    int UserNum;
	char nickname[16];
    int mmr; //왜 rp가 mmr로 표기되지?
    int totalrankings; //전광판 든 횟수
	int totalrp; //조회한 전체 RP -> 평균 RP 산출용 근데 이제 범위 조회할떄만 사용

};

int main() {
    SetConsoleOutputCP(949);

    int mode = 0;
    while(mode != 777) {
        bootingmessage();
        printf("명령어 : ");
        scanf("%d", &mode);
        if (mode == 1) {
			oneseasonview();
        }
        else if(mode == 2) {
            //여러 시즌 전광판 조회 함수 호출
        }
        else if(mode == 3) {
            printf("\n\n\n\n");
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