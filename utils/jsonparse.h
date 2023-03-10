/**
 * @file jsonparse.h
 * @author 黄李全 (846863428@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-01-17
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */

#pragma once

#include <stdio.h>

#include <iostream>
#include <string>

#include "json/json.h"
class JsoncppParseRead
{
public:
    JsoncppParseRead();
    ~JsoncppParseRead();
    static bool ParseJsonToString(std::string &des_string, const Json::Value &src_json);

    static bool ReadStringToJson(const std::string &src_string, Json::Value &dst_json);

    static bool ReadFileToJson(const std::string &file_name, Json::Value &des_json);

private:
};
