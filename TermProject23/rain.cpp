#pragma warning(disable: 4996) // scanf, gets, fopen등의 함수 보안 취약점 경고 무시
#include <stdio.h> // 기본 헤더 파일
#include <stdlib.h> // 시스템 함수 호출 용
#include <conio.h> // gets함수 호출 용
#include <string.h> // 문자열 함수 호출 용
#include <time.h> // time함수와 clock함수 호출 용
#include <windows.h> // PlaySound 함수 호출 용
#include <mmsystem.h> // PlaySound 함수 호출 용
#pragma comment(lib, "winmm.lib") // PlaySound 함수 호출 용
#define HAVE_STRUCT_TIMESPEC // windows.h와 pthread.h간의 시간 구조체 재정의 오류 방지용
#include <pthread.h> // 스레드 동작 구성 용

#define BGMPATH "AcidRain.wav" // 배경음악의 경로(파일 이름)
#define SPEED 1000 // 단어가 생성되는 간격 (단위 : ms). 조절하여 난이도 증감 가능. 500이하는 플레이 어려움.

char* words[35] = { // 단어 표본은 총 35개국의 수도 이름
	"서울",
	"북경",
	"동경",
	"예루살렘",
	"앙카라",
	"더블린",
	"런던",
	"파리",
	"암스테르담",
	"브뤼셀",
	"룩셈부르크",
	"베를린",
	"빈",
	"바르샤바",
	"프라하",
	"브라티슬라바",
	"부다페스트",
	"류블랴나",
	"레이캬비크",
	"코펜하겐",
	"오슬로",
	"스톡홀름",
	"헬싱키",
	"탈린",
	"베른",
	"리스본",
	"마드리드",
	"로마",
	"아테네",
	"오타와",
	"워싱턴",
	"멕시코시티",
	"산티아고",
	"캔버라",
	"웰링턴"
};

typedef struct { // 산성비(단어) 구조체
	int x; // 단어의 x좌표
	char word[20]; // 단어 저장 공간
} rain;

rain rains[21]; // 20번째 rains는 판정선.(0~19번째까지만 화면 표시)
clock_t start, end; // 게임 시작, 진행(종료) 시간
double sec = 0.0; // 플레이 누적 시간
double ph = 7.0; // 수소 이온 농도
char buffer[50]; // 사용자 입력 버퍼

void help(); // 도움말 함수
void viewlog(); // 점수 보기 함수
void initrains(); // 단어 배열 초기화 함수
void gamemain(); // 게임 주 함수
void prnscreen(); // 화면 출력 함수
void makerain(); // 새 단어 생성 함수

static pthread_t p_thread; // 스레드 이름
static int thr_id; // 스레드의 아이디
static int thr_exit = 1; // 스레드 종료 여부 상태

void* t_function(void* data); // 스레드화 할 사용자 입력 함수
void start_thread(); // 스레드 시작 함수
void end_thread(); // 스레드 중지 함수

int main(void)
{
	int choice = 0;


	printf("\n\n\n"); // 첫 화면
	printf("        ###     ######  #### ########     ########     ###    #### ##    ## \n");
	printf("       ## ##   ##    ##  ##  ##     ##    ##     ##   ## ##    ##  ###   ## \n");
	printf("      ##   ##  ##        ##  ##     ##    ##     ##  ##   ##   ##  ####  ## \n");
	printf("     ##     ## ##        ##  ##     ##    ########  ##     ##  ##  ## ## ## \n");
	printf("     ######### ##        ##  ##     ##    ##   ##   #########  ##  ##  #### \n");
	printf("     ##     ## ##    ##  ##  ##     ##    ##    ##  ##     ##  ##  ##   ### \n");
	printf("     ##     ##  ######  #### ########     ##     ## ##     ## #### ##    ## \n");
	printf("\n\n                      [ 시작하려면 아무키나 누르세요 ]\n");
	getch();

	while (1)
	{
		system("cls"); // 콘솔창 초기화
		printf("\n\n\n                            [ 메인메뉴 ]\n\n\n\n\n"); // 메인메뉴
		printf("                            1. 게임시작\n\n");
		printf("                            2. 기록보기\n\n");
		printf("                            3. 도 움 말\n\n");
		printf("                            4. 게임종료\n\n");
		printf("                       선택>");
		scanf("%d", &choice);

		switch (choice)
		{
		case 1:
			gamemain(); // 게임의 주 함수 호출 (게임 시작)
			break;
		case 2:
			viewlog(); // 점수 기록 출력 함수 호출
			break;
		case 3:
			help(); // 도움말 출력 함수 호출
			break;
		case 4:
			system("cls"); // 콘솔창 초기화
			return 0; // 게임 종료
			break;
		default: // 그외 입력 무시
			break;
		}
	}
}

void help() // 도움말 출력 함수
{
	system("cls"); // 콘솔창 초기화
	printf("타자연습보다 더 재밌었던 추억의 그 게임 '산성비'를 C언어로 구현한 게임입니다.\n");
	printf("하늘에서 내리는 단어들을 빨리 입력하여 없애주세요!\n");
	printf("하단의 수소 이온 농도(pH)가 0이 되면 게임이 종료됩니다.\n");
	printf("점수가 저장됩니다. 메인메뉴의 '2. 기록보기'에서 확인 가능합니다.\n\n");
	printf("입력이 없는 상태에서 엔터를 누르면 화면이 초기화되는 버그가 있습니다.\n\n\n"); // 버그
	printf("아무키나 누르면 메인 메뉴로 이동합니다.");
	getch();
}

void viewlog() // 점수 기록을 출력하는 함수
{
	FILE* fp2 = NULL; // 점수 저장 파일을 불러올 파일 포인터
	double s;
	int cnt; // N번째 번호를 메김
	system("cls"); // 콘솔창 초기화
	printf("[ 점수 ]\n\n\n");
	fp2 = fopen("score.txt", "r"); // 점수 파일 불러오기
	if (fp2 == NULL) // 파일 열기 오류
		printf("한번도 게임을 하신적이 없거나,\n점수 파일을 불러오는데 문제가 있습니다.\n");
	else
	{
		cnt = 1; // 첫번째 부터
		while (EOF != fscanf(fp2, "%lf", &s)) // 파일의 마지막까지 읽으면서
		{
			printf("%d. %.2lf초\n", cnt, s); // 점수 출력
			cnt++; // 다음 번호
		}
	}
	printf("\n\n아무키나 누르면 메인메뉴로 이동합니다.");
	getch();
}
void gamemain() // 게임의 주 함수
{
	FILE* fp = NULL; // 점수 저장을 위한 파일 포인터
	ph = 7.0; // 수소 이온 농도 7.0으로 초기화
	system("cls"); // 콘솔창 초기화
	initrains(); // 단어 배열 초기화
	start_thread(); // 사용자로부터 입력을 받는 스레드 시작
	PlaySound(TEXT(BGMPATH), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); // 배경음악 재생
	start = clock(); // 게임 시작 시간 기록
	while (1)
	{
		Sleep(SPEED); // 지정한 시간만큼 단어 생성 지연
		makerain(); // 새로운 단어 생성
		end = clock(); // 해당 단어가 생성된 시간 기록
		sec = (double)(end - start) / CLK_TCK; // 현재까지 진행한 시간
		prnscreen(); // 화면 출력
		if (ph <= 0) // 수소 이온 농도가 0 이하이면
			break; // 게임 오버
	}
	PlaySound(NULL, 0, 0); // 배경음악 중지
	end_thread(); // 입력 스레드 중지
	printf("\nGame Over!\n");
	fp = fopen("score.txt", "a"); // 점수 저장 파일 열기
	if (fp == NULL) // 파일 열기 오류
		printf("점수 기록 파일 작성 실패!\n\n");
	else
	{
		fprintf(fp, "%.2lf\n", sec); // 점수 기록
		fclose(fp);
	}
	printf("아무키나 누르면 메인메뉴로 이동합니다.\n");
	printf("메인메뉴가 나타나지 않으면 한번 더 입력해주세요.");
	getch();
}

void prnscreen() // 화면 출력 함수
{
	int i;
	system("cls"); // 콘솔창 초기화
	for (i = 0; i < 20; i++)
	{
		printf("%*s%s\n", rains[i].x, "", rains[i].word); // x좌표에 맞추어 가변적으로 단어 출력
	}
	printf("~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~"); // 판정선(rains[20].word)
	if (strcmp(rains[20].word, " ")) // 판정선에 단어가 남아있으면
		ph -= 0.5; // 0.5씩 산성화 시킴
	printf("\n[ pH ] %.1lf  [ 시간 ] %.2lf초\n\n", ph, sec); // 수소 이온 농도와 총 시간 출력
	printf("입력>"); // 사용자의 입력부
}

void makerain() // 새로운 단어(행)을 생성하는 함수
{
	int i;
	for (i = 20; i >= 0; i--)
	{
		strcpy(rains[i].word, rains[i - 1].word); // 기존 단어는 한 줄씩 밀고
		rains[i].x = rains[i - 1].x;
		rains[i - 1].x = 0;
		strcpy(rains[i - 1].word, " "); // 뒷 줄은 공백처리
	}
	rains[0].x = rand() % 53;
	srand(time(NULL));
	strcpy(rains[0].word, words[rand() % 35]); // 새로운 단어를 무작위로 배치
}

void initrains() // 단어 배열 초기화 함수 (모두 공백으로)
{
	int i;
	for (i = 0; i < 21; i++)
	{
		rains[i].x = 0;
		strcpy(rains[i].word, " ");
	}
}

void* t_function(void* data) // 스레드 처리할 단어 입력 함수
{
	while (!thr_exit) // 스레드가 중지될 때까지 입력을 계속 받음
	{
		int i;
		gets(buffer);
		for (i = 0; i < 20; i++)
		{
			if (strstr(rains[i].word, buffer)) // 입력 값과 일치하는 단어가 있으면
				strcpy(rains[i].word, " "); // 해당 단어 제거
		}
	}
}

void start_thread() // 스레드 시작 함수
{
	thr_exit = 0;
	thr_id = pthread_create(&p_thread, NULL, t_function, NULL); // 스레드 생성
}

void end_thread() // 스레드 중지 함수
{
	thr_exit = 1;
	pthread_cancel(p_thread); // 스레드 취소
}