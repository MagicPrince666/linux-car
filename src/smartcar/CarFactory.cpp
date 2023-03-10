#include "CarFactory.h"
#include "interface.h"

CarProduct::CarProduct(std::string name) :car_name_(name) 
{
    JsoncppParseRead::ReadFileToJson(CONFIG_FILE_PATH, conf_json_);
}

bool CarProduct::LoadPwmConfig(std::string key, PwmPram &pwm)
{
    if(conf_json_.isMember(key) && conf_json_[key].isObject()) {
        if(conf_json_[key].isMember("chip") && conf_json_[key]["chip"].isInt()) {
            pwm.chip = conf_json_[key]["chip"].asInt();
        }
        if(conf_json_[key].isMember("channel") && conf_json_[key]["channel"].isInt()) {
            pwm.channel = conf_json_[key]["channel"].asInt();
        }
        if(conf_json_[key].isMember("polarity") && conf_json_[key]["polarity"].isBool()) {
            pwm.polarity = conf_json_[key]["polarity"].asBool();
        }
        if(conf_json_[key].isMember("period") && conf_json_[key]["period"].isInt()) {
            pwm.period = conf_json_[key]["period"].asInt();
        }
        if(conf_json_[key].isMember("dutycycle") && conf_json_[key]["dutycycle"].isInt()) {
            pwm.dutycycle = conf_json_[key]["dutycycle"].asInt();
        }
        return true;
    }
    return false;
}

bool CarProduct::LoadPinConfig(std::string key, int &pin)
{
    pin = -1;
    if(conf_json_.isMember(key) && conf_json_[key].isInt()) {
        pin = conf_json_[key].asInt();
        return true;
    }
    return false;
}

bool CarProduct::LoadDistanceConfig(std::string key, bool &enable)
{
    if(conf_json_.isMember("distance") && conf_json_["distance"].isObject()) {
        if(conf_json_["distance"].isMember(key) && conf_json_["distance"][key].isBool()) {
            enable = conf_json_["distance"][key].asBool();
            return true;
        }
    }
    return false;
}