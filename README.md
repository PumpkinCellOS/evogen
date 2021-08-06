# evogen
A library to generating Minecraft worlds.

## Build instructions

* Install build dependencies:
    * C++20 capable compiler (GCC 10+)
    * CMake
    * Git
```sh
sudo apt install cmake
sudo add-apt-repository 'deb http://mirrors.kernel.org/ubuntu hirsute main universe'
sudo apt-get update
sudo apt-get install gcc-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11 --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-11 --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-11  --slave /usr/bin/cpp cpp /usr/bin/cpp-11
```
(See https://stackoverflow.com/questions/67298443/when-gcc-11-will-appear-in-ubuntu-repositories)
* Install "normal" dependencies:
    * CPP NBT
```sh
git clone https://github.com/SpockBotMC/cpp-nbt --depth 1
```

* Create build directory
```sh
mkdir build
cd build
```

* Configure and build
```sh
cmake ..
make -j$(nproc)
```

* Run
```sh
cmd/evogen
```
