/**
 * @file xinotify.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 文件监控
 * @version 0.1
 * @date 2023-01-13
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include "xepoll.h"
#include <unordered_map>
#include <vector>

#define MY_INOTIFY Xinotify::Instance()

class Xinotify
{
public:
    static Xinotify &Instance()
    {
        static Xinotify instance;
        return instance;
    }
    ~Xinotify();

    int HandleEvent();
    bool AddFileWatch(const std::string &path, std::function<void()> handler);
    bool DelFileWatch(const std::string &path);

private:
    Xinotify();
    int inotify_fd_;
    std::unordered_map<std::string, int> file_watch_fd_map_;
    std::unordered_map<int, std::function<void()>> listeners_file_change_;
};
