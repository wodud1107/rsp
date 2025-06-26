#include "rsp_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SYSFS_GPIO_PATH "/sys/class/rsp"

// 핀 export/unexport
static int sysfs_write(const char *path, const char *val) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    int ret = write(fd, val, strlen(val));
    close(fd);
    return ret;
}

static int export_gpio(int bcm) {
    char path[64];
    snprintf(path, sizeof(path), SYSFS_GPIO_PATH "/export");
    char num[8];
    snprintf(num, sizeof(num), "%d", bcm);
    return sysfs_write(path, num);
}

static int unexport_gpio(int bcm) {
    char path[64];
    snprintf(path, sizeof(path), SYSFS_GPIO_PATH "/unexport");
    char num[8];
    snprintf(num, sizeof(num), "%d", bcm);
    return sysfs_write(path, num);
}

static int set_gpio_dir(int bcm, const char *dir) {
    char path[128];
    snprintf(path, sizeof(path), SYSFS_GPIO_PATH "/rspgpio%d/direction", bcm);
    return sysfs_write(path, dir);
}

static int set_gpio_val(int bcm, int value) {
    char path[128];
    snprintf(path, sizeof(path), SYSFS_GPIO_PATH "/rspgpio%d/value", bcm);
    return sysfs_write(path, value ? "1" : "0");
}

static int get_gpio_val(int bcm) {
    char path[128];
    snprintf(path, sizeof(path), SYSFS_GPIO_PATH "/rspgpio%d/value", bcm);
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    char ch;
    int ret = read(fd, &ch, 1);
    close(fd);
    if (ret < 0) return -1;
    return (ch == '1') ? 1 : 0;
}

int rsp_init(const rsp_gpio_t *gpio) {
    export_gpio(gpio->data1_bcm);
    export_gpio(gpio->data2_bcm);
    export_gpio(gpio->clk_bcm);
    usleep(100*1000); // 100ms delay for sysfs
    return 0;
}

int rsp_cleanup(const rsp_gpio_t *gpio) {
    unexport_gpio(gpio->data1_bcm);
    unexport_gpio(gpio->data2_bcm);
    unexport_gpio(gpio->clk_bcm);
    return 0;
}

// out==1: 출력, out==0: 입력
int rsp_set_direction(const rsp_gpio_t *gpio, int out) {
    set_gpio_dir(gpio->data1_bcm, out ? "out" : "in");
    set_gpio_dir(gpio->data2_bcm, out ? "out" : "in");
    set_gpio_dir(gpio->clk_bcm,   out ? "out" : "in");
    return 0;
}

// hand를 2비트로 전송
int rsp_send(const rsp_gpio_t *gpio, rsp_hand_t hand) {
    // 2비트 세팅
    set_gpio_val(gpio->data1_bcm, (hand >> 1) & 1);
    set_gpio_val(gpio->data2_bcm, hand & 1);

    // CLK High (수신자에게 데이터 도착 알림)
    set_gpio_val(gpio->clk_bcm, 1);
    usleep(100*1000); // 100ms
    set_gpio_val(gpio->clk_bcm, 0);
    return 0;
}

// 수신 (2비트)
int rsp_recv(const rsp_gpio_t *gpio, rsp_hand_t *hand) {
    // CLK High 대기
    while (get_gpio_val(gpio->clk_bcm) == 0) {
        usleep(10*1000); // 10ms polling
    }
    // 데이터 읽기
    int bit1 = get_gpio_val(gpio->data1_bcm);
    int bit2 = get_gpio_val(gpio->data2_bcm);
    *hand = (rsp_hand_t)((bit1 << 1) | bit2);

    // 수신 이후 약간의 시간 대기
    usleep(100*1000); // 100ms
    return 0;
}

int rsp_wait_for_clk(const rsp_gpio_t *gpio) {
    while (get_gpio_val(gpio->clk_bcm) == 0) {
        usleep(10*1000); // 10ms polling
    }
    return 0;
}