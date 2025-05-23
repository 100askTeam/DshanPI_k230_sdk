include $(MPP_SRC_DIR)/middleware/mpp.mk
include $(MPP_SRC_DIR)/middleware/rt-smart.mk

AR = riscv64-unknown-linux-musl-ar
ARFLAGS = -rc

CURRECT_DIR_NAME=$(shell basename `pwd`)
LOCAL_SRC_DIR = $(shell pwd)
BIN = $(MPP_SRC_DIR)/userapps/sample/elf/$(CURRECT_DIR_NAME).elf
LIBPATH = $(MPP_LIB_PATH)
LIBS = $(MPP_LIBS)

LIBS_FOR_CONSOLE_APPLICATION = -lssl -lcrypto
CC_CFLAGS_EX=-mcmodel=medany -march=rv64imafdcv -mabi=lp64d -Wall -O0 -g -gdwarf-2 -n --static $(KCFLAGS)

LIVE555_USAGEENVIRONMENT= ../UsageEnvironment
LIVE555_BASICUSAGEENVIRONMENT= ../BasicUsageEnvironment
LIVE555_GROUPSOCK = ../groupsock
LIVE555_LIVEMEDIA = ../liveMedia

LOCAL_LIBS_PATH = -L$(LIVE555_USAGEENVIRONMENT) -L$(LIVE555_BASICUSAGEENVIRONMENT) -L$(LIVE555_GROUPSOCK) -L$(LIVE555_LIVEMEDIA)
LOCAL_LIBS = -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment

LIVE555_COMPILE_OPTS = -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DNO_OPENSSL=1

LOCAL_CFLAGS = -I$(LOCAL_SRC_DIR) -I$(LIVE555_GROUPSOCK)/include -I$(LIVE555_USAGEENVIRONMENT)/include -I$(LIVE555_BASICUSAGEENVIRONMENT)/include -I$(LIVE555_LIVEMEDIA)/include

WERROR_FLAGS = -Wno-register -Wno-multichar -Wno-pessimizing-move -Wno-deprecated-declarations -Wno-unused-result -Wno-unused-variable -Wno-format -Wno-return-type -Wno-sign-compare -Wno-unused-label

CXX_FLAGS = $(WERROR_FLAGS) \
			$(LIVE555_COMPILE_OPTS) \

SRCS = $(wildcard $(LOCAL_SRC_DIR)/*.c)

OBJS = $(patsubst %.c,%.o,$(SRCS))

CPP_SRCS = $(wildcard $(LOCAL_SRC_DIR)/*.cpp)

CPP_OBJS = $(patsubst %.cpp,%.o,$(CPP_SRCS))

all:$(BIN)
	#@-rm -f *.o
	@echo "${PWD}/Makefile all"

$(OBJS): %.o : %.c
	@echo CC $@
	@$(CC) $(CC_CFLAGS_EX) $(LOCAL_CFLAGS) $(BSP_CFLGAS) $(RTSMART_CFLAGS) $(MPP_USER_CFLGAS) $(LOCAL_LIBS)-c $< -o $@

$(CPP_OBJS): %.o : %.cpp
	@echo CXX $@
	@$(CPP) $(CC_CFLAGS_EX) $(CXX_FLAGS) $(LOCAL_CFLAGS) $(BSP_CFLGAS) $(RTSMART_CFLAGS) $(MPP_USER_CFLGAS) $(LOCAL_LIBS) -c $< -o $@

$(BIN): $(OBJS) $(CPP_OBJS)
	@echo LD $@
	@$(CPP) -o $(BIN)  $(LINKFLAG) -Wl,--whole-archive -Wl,--no-whole-archive -n --static $^ -L$(LIBPATH) $(CXX_FLAGS) $(LOCAL_LIBS) $(LOCAL_LIBS_PATH) -Wl,--start-group $(LIBS) -Wl,--end-group

clean:
	echo "${PWD}/Makefile clean"
	rm -rf $(BIN) $(OBJS) $(CPP_OBJS)

.PHONY: all clean
