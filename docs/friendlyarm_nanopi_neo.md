# 友善之臂 nanopi neo

## 编译
```
$ export STAGING_DIR=/Volumes/unix/HandsomeMod/staging_dir
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=cmake/build_for_h3.cmake ..
$ cmake -DCMAKE_TOOLCHAIN_FILE=cmake/build_for_h3.cmake -DCMAKE_BUILD_TYPE=Debug ..
```