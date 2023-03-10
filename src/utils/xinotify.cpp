/**
 * @file xinotify.cc
 * @author 黄李全 (hlq@ldrobot.com)
 * @brief
 * @version 0.1
 * @date 2021-06-10
 * @copyright Copyright (c) {2021} 深圳乐动机器人版权所有
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

#include "interface.h"
#include "xinotify.h"

Xinotify::Xinotify()
{
    inotify_fd_ = inotify_init();
    if (inotify_fd_ < 0) {
        fprintf(stderr, "inotify_init failed\n");
    }
    assert(inotify_fd_ > 0);
    MY_EPOLL.EpollAdd(inotify_fd_, std::bind(&Xinotify::HandleEvent, this));
    std::cout << BOLDGREEN << "Inotify init" << std::endl;
}

Xinotify::~Xinotify()
{
    if (inotify_fd_ > 0) {
        MY_EPOLL.EpollDel(inotify_fd_);
        for (auto wds : file_watch_fd_map_) {
            inotify_rm_watch(inotify_fd_, wds.second);
        }
        close(inotify_fd_);
    }
}

bool Xinotify::AddFileWatch(const std::string &path, std::function<void()> handler)
{
    // 保证文件存在
    if (-1 == access(path.c_str(), F_OK)) {
        fprintf(stderr, "Check file %s\n", path.c_str());
        return false;
    }

    // IN_MODIFY 监控文件被修改
    int watch_fd = inotify_add_watch(inotify_fd_, path.c_str(), IN_ALL_EVENTS);
    if (watch_fd < 0) {
        fprintf(stderr, "inotify_add_watch %s failed\n", path.c_str());
        return false;
    } else {
        file_watch_fd_map_[path]         = watch_fd;
        listeners_file_change_[watch_fd] = handler;
    }
    std::cout << BOLDGREEN << "Inotify add watch " << path << std::endl;
    return true;
}

bool Xinotify::DelFileWatch(const std::string &path)
{
    if (file_watch_fd_map_.count(path)) {
        inotify_rm_watch(inotify_fd_, file_watch_fd_map_[path]);
        listeners_file_change_.erase(file_watch_fd_map_[path]);
        file_watch_fd_map_.erase(path);
        std::cout << BOLDGREEN << "Remove file from watch " << path << std::endl;
        return true;
    }
    return false;
}

int Xinotify::HandleEvent()
{
    char buf[512];
    struct inotify_event *event;
    int event_size = sizeof(struct inotify_event);

    // 检测事件是否发生，没有发生就会阻塞
    int read_len = read(inotify_fd_, buf, sizeof(buf));

    // 如果read的返回值，小于inotify_event大小出现错误
    if (read_len < event_size) {
        printf("could not get event!\n");
        return -1;
    }

    // 因为read的返回值存在一个或者多个inotify_event对象，需要一个一个取出来处理
    int pos = 0;
    while (read_len >= event_size) {
        event = (struct inotify_event *)(buf + pos);
        if (event->len) {
            if (event->mask & IN_CLOSE_WRITE) {
                // printf("Modify file: %s\n", event->name);
                auto handle_it = listeners_file_change_.find(event->wd);
                if (handle_it != listeners_file_change_.end()) {
                    handle_it->second();
                }
            }
        }

        // 一个事件的真正大小：inotify_event 结构体所占用的空间 + inotify_event->name 所占用的空间
        int temp_size = event_size + event->len;
        read_len -= temp_size;
        pos += temp_size;
    }
    return 0;
}
