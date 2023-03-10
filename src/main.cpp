#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <new>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include "spdlog/cfg/env.h" // support for loading levels from the environment variable
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/fmt/ostr.h" // support for user defined types
#include "spdlog/spdlog.h"

#include "makecar.h"
#include "jsonparse.h"
#include "interface.h"

#define BACKTRACE_DEBUG 0

#if BACKTRACE_DEBUG
#include <execinfo.h>

#define PRINT_SIZE_ 100

static void _signal_handler(int signum)
{
    void *array[PRINT_SIZE_];
    char **strings;

    size_t size = backtrace(array, PRINT_SIZE_);
    strings     = backtrace_symbols(array, size);

    if (strings == nullptr) {
        fprintf(stderr, "backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    switch (signum) {
    case SIGSEGV:
        fprintf(stderr, "widebright received SIGSEGV! Stack trace:\n");
        break;

    case SIGPIPE:
        fprintf(stderr, "widebright received SIGPIPE! Stack trace:\n");
        break;

    case SIGFPE:
        fprintf(stderr, "widebright received SIGFPE! Stack trace:\n");
        break;

    case SIGABRT:
        fprintf(stderr, "widebright received SIGABRT! Stack trace:\n");
        break;

    default:
        break;
    }

    for (size_t i = 0; i < size; i++) {
        fprintf(stderr, "%d %s \n", i, strings[i]);
    }

    free(strings);
    signal(signum, SIG_DFL); /* 还原默认的信号处理handler */

    exit(1);
}
#endif

int main(int argc, char *argv[])
{
    spdlog::info("--- version 2.0 ---");
    bool debug = false;
#if BACKTRACE_DEBUG
    signal(SIGPIPE, _signal_handler); // SIGPIPE，管道破裂。
    signal(SIGSEGV, _signal_handler); // SIGSEGV，非法内存访问
    signal(SIGFPE, _signal_handler);  // SIGFPE，数学相关的异常，如被0除，浮点溢出，等等
    signal(SIGABRT, _signal_handler); // SIGABRT，由调用abort函数产生，进程非正常退出
#endif
    if (argc >= 2) {
        std::string str = argv[1];
        if (str == "debug") {
            debug = true;
        }
    }

    Json::Value conf_json;
    JsoncppParseRead::ReadFileToJson(CONFIG_FILE_PATH, conf_json);

    std::string chip = "";
    if(conf_json.isMember("chip") && conf_json["chip"].isString()) {
        chip = conf_json["chip"].asString();
    }

    // ProduceCar carmaker;
    // std::unique_ptr<CarProduct> mycar(carmaker.Create(ORANGEPI_SMART_CAR));
    // mycar->Run(debug);

    if (chip == "ArmPi") {
        // 创建具体的小车工厂
        std::unique_ptr<CarFactory> factory(new ArmPiCar);
        // 通过工厂方法创建小车产品
        std::unique_ptr<CarProduct> mycar(factory->createCarProduct());
        mycar->Run(debug);
    } else if(chip == "F1c100s") {
        std::unique_ptr<CarFactory> factory(new F1c100sRemote);
        std::unique_ptr<CarProduct> f1c100s(factory->createCarProduct());
        f1c100s->Run(debug);
    } else {
        spdlog::error("Not support yet!!");
    }

    return 0;
}
