安装依赖，参考https://fast-dds.docs.eprosima.com/en/latest/installation/sources/sources_linux.html：

```bash
sudo apt install libasio-dev libtinyxml2-dev
```

编译安装 Foonathan memory ，提供了经过优化的分配器：

```bash
git clone https://github.com/foonathan/memory.git
cd memory
mkdir build && cd build
# 默认安装路径：/usr/local
cmake ..
make
sudo make install
```

编译安装 Fast CDR ，提供了两种序列化机制：

```bash
git clone https://github.com/eProsima/Fast-CDR.git
mkdir Fast-CDR/build && cd Fast-CDR/build
cmake ..
make
sudo make install
```

安装编译 Fast-DDS：

```bash
git clone https://github.com/eProsima/Fast-DDS.git
mkdir Fast-DDS/build && cd Fast-DDS/build
# 打开Debug模式：
cmake -DLOG_NO_INFO=OFF -DEPROSIMA_BUILD=ON -DTHIRDPARTY=ON ..
make
sudo make install
```

编译代码生成工具 Fast-DDS Gen (需要先装好Java和Gradle)：

```bash
git clone --recursive https://github.com/eProsima/Fast-DDS-Gen.git
cd Fast-DDS-Gen
gradle assemble 或者 ./gradlew assemble
```

把编译出来的执行脚本加到zsh(你也可以是bash)环境变量：

```bash
gedit ~/.zshrc
加到最后一行：
# Fast-DDS Gen
export PATH=/home/lxl/Fast-DDS/Fast-DDS-Gen/scripts:$PATH
```

根据idl生成代码：
fastddsgen xxx.idl


FastDDS 有两套 API，接近传输层的 RTPS API 和规范的 DDS API。
通过fastddsgen -example生成的示例Publisher和Subscriber代码是使用的DDS API。
两套API的侧重点好像完全不一样。。。如果想深入学习，需要先学习dds协议和rtps协议。