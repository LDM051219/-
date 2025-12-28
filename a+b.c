#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { A_DICE = 5, B_DICE = 5, MAX_NAME = 20 };

static void dice_A(int out[5]) {
    for (int i = 0; i < A_DICE; i++) {
        out[i] = rand() % 6 + 1;
        printf("A의 %d번 주사위의 값: %d\n", i + 1, out[i]);
    }
}

static void dice_B(int out[5]) {
    for (int i = 0; i < B_DICE; i++) {
        out[i] = rand() % 6 + 1;
        printf("B의 %d번 주사위의 값: %d\n", i + 1, out[i]);
    }
}

static void flush_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

static void ask_name(const char* prompt, char out[MAX_NAME]) {
    while (1) {
        printf("%s", prompt);
        if (fgets(out, MAX_NAME, stdin) == NULL) {
            clearerr(stdin);
            continue;
        }
        size_t len = strlen(out);
        if (len > 0 && out[len - 1] == '\n') out[len - 1] = '\0';
        if (out[0] == '\0') {
            puts("빈 이름은 안 됩니다. 다시 입력하세요.");
            continue;
        }
        break;
    }
}

static char ask_choice(const char* prompt) {
    while (1) {
        printf("%s", prompt);
        int c = getchar();
        if (c == EOF) { clearerr(stdin); continue; }
        if (c == '\n') continue;
        flush_line();
        if (c == 'A' || c == 'a' || c == 'B' || c == 'b') {
            return (char)c;
        }
        puts("잘못된 선택입니다. A 또는 B를 입력하세요.");
    }
}

static int ask_int_in_range(const char* prompt, int lo, int hi) {
    int v;
    while (1) {
        printf("%s", prompt);
        int ok = scanf("%d", &v);
        if (ok != 1) {
            puts("정수를 입력하세요.");
            clearerr(stdin);
            flush_line();
            continue;
        }
        flush_line();
        if (v < lo || v > hi) {
            printf("%d ~ %d 사이로 입력하세요.\n", lo, hi);
            continue;
        }
        return v;
    }
}

char opposite_deck(char choice) {
    if (choice == 'A' || choice == 'a') return 'B';
    else return 'A';
}

static int score_dice(int dice[5]) {
    int sum = 0;
    for (int i = 0; i < 5; i++) sum += dice[i];
    return sum * 1000;
}

static void bidding_phase(const char* p1, const char* p2,
    int* p1Score, int* p2Score,
    int diceA[5], int diceB[5]) {
    char choice1 = ask_choice("플레이어 1 - 덱을 고릅니다 (A/B): ");
    int bid1 = ask_int_in_range("입찰할 금액(0~100,000): ", 0, 100000);
    printf("%s: 덱 %c, 금액 %d원\n", p1, choice1, bid1);

    char choice2 = ask_choice("플레이어 2 - 덱을 고릅니다 (A/B): ");
    int bid2 = ask_int_in_range("입찰할 금액(0~100,000): ", 0, 100000);
    printf("%s: 덱 %c, 금액 %d원\n", p2, choice2, bid2);

    if (choice1 == choice2 && bid1 == bid2) {
        puts("동일 덱·동일 금액 → 랜덤 배정");
        if (rand() % 2 == 0) {
            printf("%s 승, %s 패\n", p1, p2);
            choice2 = opposite_deck(choice1);
        }
        else {
            printf("%s 승, %s 패\n", p2, p1);
            choice1 = opposite_deck(choice2);
        }
    }
    else if (choice1 == choice2) {
        if (bid1 > bid2) {
            choice2 = opposite_deck(choice1);
            printf("%s가 높은 금액\n", p1);
        }
        else {
            choice1 = opposite_deck(choice2);
            printf("%s가 높은 금액\n", p2);
        }
    }

    // 입찰 점수 반영
    if (choice1 == choice2) {
        // 서로 원하는 덱을 가짐
        *p1Score -= bid1;
        *p2Score -= bid2;
    }
    else {
        // 한 명은 원하는 덱, 한 명은 아닌 덱
        if (choice1 == 'A') {
            *p1Score -= bid1;
            *p2Score += bid2;
        }
        else if (choice1 == 'B') {
            *p1Score -= bid1;
            *p2Score += bid2;
        }
    }
}

static void scoring_phase(const char* pname, int* pScore, int diceSet[5]) {
    int score = score_dice(diceSet);
    *pScore += score;
    printf("%s가 주사위 점수 %d점을 획득!\n", pname, score);
}

int main(void) {
    srand((unsigned)time(NULL));

    char p1[MAX_NAME], p2[MAX_NAME];
    int p1Score = 0, p2Score = 0;

    ask_name("플레이어 1 이름: ", p1);
    ask_name("플레이어 2 이름: ", p2);

    int diceA[5], diceB[5];

    for (int round = 1; round <= 13; round++) {
        printf("\n=== 라운드 %d ===\n", round);

        if (round != 13) {
            puts("주사위를 굴립니다...");
            dice_A(diceA);
            dice_B(diceB);
            bidding_phase(p1, p2, &p1Score, &p2Score, diceA, diceB);
        }

        if (round != 1) {
            puts("\n점수 획득 단계:");
            scoring_phase(p1, &p1Score, diceA);
            scoring_phase(p2, &p2Score, diceB);
        }

        printf("\n현재 점수: %s = %d, %s = %d\n", p1, p1Score, p2, p2Score);
    }

    printf("\n=== 최종 점수 ===\n%s: %d\n%s: %d\n", p1, p1Score, p2, p2Score);
    if (p1Score > p2Score) printf("%s 승리!\n", p1);
    else if (p2Score > p1Score) printf("%s 승리!\n", p2);
    else printf("무승부!\n");

    return 0;
}

