#include <stdio.h>
#define API_KEY = ""
int main()
{
	char username[16];
	printf("통계를 낼 플레이어의 닉네임을 입력하시오 : ");
	scanf_s("%s", username);
	if(sizeof(username) > 16)
	{
		printf("조회할 닉네임은 16자 이내로 설정해야 합니다.\n");
		return 1;
	}
	printf("플레이어 %s의 통계를 조회합니다...\n", username);
	research_player(username);


	return 0;
}