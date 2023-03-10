/**
 * @file utils.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 封装一些小工具
 * @version 0.1
 * @date 2023-01-12
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <iostream>
#include <vector>

namespace Utils {
 /**
 * @brief 搜索设备 iio 设备
 * @param name 模块名称
 * @return std::string 返回路径
 */
std::string ScanIioDevice(std::string name);

/**
 * @brief 一次性读取文件所有内容
 * @param path 文件路径
 * @return std::string 内容
 */
std::string ReadFileIntoString(const std::string& path);

/**
 * @brief 获取文件夹下所有文件
 * @param path 
 * @param files 
 */
void getFiles(std::string path, std::vector<std::string>& files);
};