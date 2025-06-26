#ifndef RSP_API_H
#define RSP_API_H

// 각 GPIO의 BCM 번호 지정
typedef struct {
    int data1_bcm; // 데이터 1 (최상위 비트)
    int data2_bcm; // 데이터 2 (최하위 비트)
    int clk_bcm;   // 클럭(동기화) 핀
} rsp_gpio_t;

typedef enum {
    RSP_SCISSORS = 0, // 00
    RSP_ROCK     = 1, // 01
    RSP_PAPER    = 2, // 10
    RSP_INVALID  = 3  // 11 (예외처리용)
} rsp_hand_t;

// API 함수
int rsp_init(const rsp_gpio_t *gpio);      // GPIO 초기화 (export 등)
int rsp_cleanup(const rsp_gpio_t *gpio);   // GPIO 해제 (unexport)
int rsp_set_direction(const rsp_gpio_t *gpio, int out); // 핀 방향 설정
int rsp_send(const rsp_gpio_t *gpio, rsp_hand_t hand);  // hand를 2비트로 전송
int rsp_recv(const rsp_gpio_t *gpio, rsp_hand_t *hand); // 2비트 수신
int rsp_wait_for_clk(const rsp_gpio_t *gpio);           // CLK rising 대기

#endif