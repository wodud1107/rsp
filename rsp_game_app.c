#include "rsp_api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_hand(rsp_hand_t hand) {
    switch (hand) {
        case RSP_SCISSORS: printf("가위"); break;
        case RSP_ROCK:     printf("바위"); break;
        case RSP_PAPER:    printf("보"); break;
        default:           printf("Invalid"); break;
    }
}

int judge(rsp_hand_t me, rsp_hand_t oppo) {
    if (me == oppo) return 0; // 무승부
    if ((me == RSP_SCISSORS && oppo == RSP_PAPER) ||
        (me == RSP_ROCK && oppo == RSP_SCISSORS) ||
        (me == RSP_PAPER && oppo == RSP_ROCK))
        return 1; // 이김
    return -1; // 짐
}

rsp_hand_t get_hand_from_user() {
    int sel;
    printf("당신의 선택 (0: 가위, 1: 바위, 2: 보) > ");
    scanf("%d", &sel);
    if (sel < 0 || sel > 2) return RSP_INVALID;
    return (rsp_hand_t)sel;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s send|recv\n", argv[0]);
        return 1;
    }

    // 실제 연결된 BCM 번호로 수정
    rsp_gpio_t gpio = {17, 27, 22}; // 예: data1=17, data2=27, clk=22

    rsp_init(&gpio);

    if (strcmp(argv[1], "send") == 0) {
        // 송신자: 출력모드
        rsp_set_direction(&gpio, 1);
        rsp_hand_t my_hand = get_hand_from_user();
        if (my_hand == RSP_INVALID) {
            printf("잘못된 입력\n");
            rsp_cleanup(&gpio);
            return 1;
        }
        printf("당신의 선택: ");
        print_hand(my_hand);
        printf("\n상대에게 신호 전송중...\n");
        rsp_send(&gpio, my_hand);
        printf("전송 완료!\n");
    }
    else if (strcmp(argv[1], "recv") == 0) {
        // 수신자: 입력모드
        rsp_set_direction(&gpio, 0);
        printf("상대 신호 대기 중...\n");
        rsp_hand_t oppo_hand;
        rsp_recv(&gpio, &oppo_hand);
        printf("상대의 선택: ");
        print_hand(oppo_hand);
        printf("\n");
    }
    else {
        printf("Usage: %s send|recv\n", argv[0]);
    }

    rsp_cleanup(&gpio);
    return 0;
}