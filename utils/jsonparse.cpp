#include "jsonparse.h"

#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

JsoncppParseRead::JsoncppParseRead() {}

JsoncppParseRead::~JsoncppParseRead() {}

bool JsoncppParseRead::ParseJsonToString(std::string &des_string,
                                         const Json::Value &src_json)
{
    std::ostringstream oStringStream;
    Json::StreamWriterBuilder sWBuilder;
    sWBuilder["commentStyle"] = "None";
    sWBuilder["indentation"]  = "";
    std::unique_ptr<Json::StreamWriter> writer(sWBuilder.newStreamWriter());
    int res = writer->write(src_json, &oStringStream);
    if (res == 0) {
        // do nothing
    }
    des_string.clear();
    des_string = oStringStream.str();
    return true;
}
bool JsoncppParseRead::ReadStringToJson(const std::string &src_string,
                                        Json::Value &dst_json)
{
    if (!dst_json.isNull()) {
        // do nothing
    }
    std::istringstream iStringStream(src_string);
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    JSONCPP_STRING errs;
    bool result = parseFromStream(builder, iStringStream, &dst_json, &errs);
    if (result) {
        return true;
    } else {
        return false;
    }
    return true;
}

bool JsoncppParseRead::ReadFileToJson(const std::string &file_name,
                                      Json::Value &des_json)
{
    std::ifstream in_file(file_name, std::ios::binary);
    if (!in_file.is_open()) {
        printf("\nJsoncppParseRead::ReadFileToJson !in_file.is_open() \n");
        printf("file name = %s \n\n", file_name.c_str());
        return false;
    }
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    JSONCPP_STRING errs;
    bool result = parseFromStream(builder, in_file, &des_json, &errs);
    in_file.close();
    if (result) {
        return true;
    } else {
        printf("\n JsoncppParseRead::ReadFileToJson result = false \n\n");
        return false;
    }
}
