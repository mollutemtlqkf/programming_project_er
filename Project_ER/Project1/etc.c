#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

void bootingmessage() {
        
    printf("### 이터널 리턴 전광판 조회 시스템에 오신것을 환영합니다\n");
    printf("원하는 기능을 골라서 숫자로 입력해주세요\n");
    printf("1.단일 시즌 전광판 조회\n");
    printf("2.여러 시즌 전광판 조회\n");
    printf("3.설명\n");
	printf("4.유저 정보 조회\n");
    printf("프로그램 종료는 777를 입력해주세요\n");
    printf("#########################################\n");
}

void explainmessage() {
    printf("살명모드에 오신것을 환영합니다\n");
    printf("설명을 원하시는 기능의 숫자를 입력하시오\n");
    int endcode = 0;
    while (endcode != 777) {
        printf("명령어 : ");
        scanf("%d", &endcode);
        if (endcode == 1) {
            printf("단일 시즌 전광판 조회는 특정 시즌의 전광판 데이터를 조회하는 기능입니다.\n");
            printf("시즌 번호를 입력하면 해당 시즌의 전광판 데이터가 출력됩니다.\n");
        }
        else if (endcode == 2) {
            printf("여러 시즌 전광판 조회는 지정한 시즌 범위 내에서 전광판 데이터를 조회하는 기능입니다.\n");
            printf("시작 시즌과 종료 시즌을 입력하면 해당 범위 내의 전광판 데이터가 출력됩니다.\n");
        }
        else if (endcode == 3) {
            printf("설명에서 설명을 왜 찾고 계신건가요?");
		}
        else if (endcode == 4) {
            printf("유저 정보 조회는 특정 유저의 시즌별 통계 정보를 조회하는 기능입니다.\n");
            printf("유저 닉네임과 시즌 번호를 입력하면 해당 유저의 시즌 통계가 출력됩니다.\n");
		}
        else if (endcode == 777) {
            printf("설명모드를 종료합니다\n");
            break;
        }
        else {
            printf("잘못된 입력입니다. 다시 입력해주세요\n");
		}
    }
}

    