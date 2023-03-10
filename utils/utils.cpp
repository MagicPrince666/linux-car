#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

std::string Utils::ScanIioDevice(std::string name) {
    std::string device = "";
    if(!name.empty()) { //扫描设备
        for(int i = 0; i < 10; i++) {
            std::string filename = "/sys/bus/iio/devices/iio:device" + std::to_string(i) + "/name";
            // std::cout << "iio bus name" << filename << std::endl;
            struct stat buffer;   
            if(stat(filename.c_str(), &buffer) == 0) {
                std::ifstream iio_name;
                iio_name.open(filename, std::ios::in);
                char buff[64] = {0};
                iio_name.read(buff, sizeof(buff));
                std::string dev_name(buff);

                if (dev_name.find(name) != std::string::npos) {
                    // std::cout << "find " << name << " device\n";
                    device = "/sys/bus/iio/devices/iio:device" + std::to_string(i) +"/";
                    break;
                }
            } else {
                // 搜索完了，没有找到对应设备
                std::cout << "iio bus scan end\n";
                break;
            }
        }
    }

    return device;
}

std::string Utils::ReadFileIntoString(const std::string& path) {
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    std::string contents(buffer.str());
    input_file.close();
    return contents;
}

void Utils::getFiles(std::string path, std::vector<std::string>& files)
{
	// check the parameter !
	if( path.empty() ) {
		return;
	}
	// check if dir_name is a valid dir
	struct stat s;
	lstat( path.c_str(), &s );
	if( ! S_ISDIR( s.st_mode ) ) {
		return;
	}

	struct dirent * filename;    // return value for readdir()
	DIR * dir;                   // return value for opendir()
	dir = opendir( path.c_str() );
	if( NULL == dir ) {
		return;
	}

	/* read all the files in the dir ~ */
	while( ( filename = readdir(dir) ) != NULL ) {
		// get rid of "." and ".."
		if( strcmp( filename->d_name , "." ) == 0 ||
			strcmp( filename->d_name , "..") == 0 )
			continue;
        std::string full_path = path + filename->d_name;
        struct stat s;
        lstat( full_path.c_str(), &s );
        if( S_ISDIR( s.st_mode ) ) {
            continue;
        }
        files.push_back(full_path);
	}
}
