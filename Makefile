# ==== 경로 및 파일명 ====
KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

DRIVER_SRC := rsp_driver.c
DRIVER_OBJ := rsp_driver.ko

API_SRC := rsp_api.c
API_OBJ := rsp_api.o

APP_SRC := rsp_game_app.c
APP_OBJ := rsp_game_app

# ==== 기본 타겟 ====
all: $(DRIVER_OBJ) $(APP_OBJ)

# ==== 드라이버 빌드 ====
$(DRIVER_OBJ): $(DRIVER_SRC)
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# ==== 앱 빌드 (API.o + app.c) ====
$(APP_OBJ): $(APP_SRC) $(API_SRC) rsp_api.h
	gcc -o $@ $(APP_SRC) $(API_SRC)

# ==== 모듈 빌드시 필요한 부분 ====
obj-m += rsp_driver.o

# ==== 청소 ====
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(APP_OBJ) *.o *.mod.c *.order *.symvers

.PHONY: all clean