在Linux系统上，按照以下步骤编译&&安装runtime库：
1)安装依赖包： 
```bash
sudo apt-get install libboost-system1.67-dev libboost-thread1.67-dev libboost-log1.67-dev
sudo apt-get install source-highlight doxygen graphviz
sudo apt-get --no-install-recommends install asciidoc
```
2)编译安装vsomeip（Transport protocol层）
```bash
git clone https://github.com/COVESA/vsomeip.git
cd vsomeip/
mkdir build
cd build/
cmake -DENABLE_SIGNAL_HANDLING=1 ..
make
sudo make install
```
3)编译安装COMMONAPI（IDL层）
```bash
git clone https://github.com/COVESA/capicxx-core-runtime.git
cd capicxx-core-runtime/
mkdir build
cd build/
cmake -D CMAKE_INSTALL_PREFIX=/usr/local ..
make
sudo make install
```
4)编译安装common-api someip（Middleware层）
```bash
git clone https://github.com/COVESA/capicxx-someip-runtime.git
cd capicxx-someip-runtime/
mkdir build
cd build/
cmake -D USE_INSTALLED_COMMONAPI=ON -D CMAKE_INSTALL_PREFIX=/usr/local ..
make
```

切换JAVA版本到Oracle jdk1.8(maven版本和jdk版本关系密切，一定要根据自己系统安装对于的jdk版本)：

```bash
sudo update-alternatives --install /usr/bin/java java /opt/jdk1.8.0_281/bin/java 700
sudo update-alternatives --install /usr/bin/javac javac /opt/jdk1.8.0_281/bin/javac 700
sudo update-alternatives --install /usr/bin/jar jar /opt/jdk1.8.0_281/bin/jar 700
sudo update-alternatives --config java # 选择jdk1.8
java -version # 检查是否配置成功
```
5)编译安装COMMONAPI代码生成器
编译CommonAPI Core Runtime代码生成工具：

```bash
git clone https://github.com/COVESA/capicxx-core-tools.git
cd capicxx-core-tools/org.genivi.commonapi.core.releng
mvn -Dtarget.id=org.genivi.commonapi.core.target clean verify
```

解压得到代码生成工具：

```bash
cd someip_dev
unzip -d ./commonapi_core_generator ./capicxx-core-tools/org.genivi.commonapi.core.cli.product/target/products/commonapi_core_generator.zip
chmod +x ./commonapi_core_generator/commonapi-core-generator-linux-x86_64
```
自行安装工具到环境

6)编译安装COMMONAPI-SOMEIP代码生成器
编译CommonAPI SomeIP Runtime代码生成工具：

```bash
git clone https://github.com/GENIVI/capicxx-someip-tools.git
cd capicxx-someip-tools/org.genivi.commonapi.someip.releng/
mvn -DCOREPATH=/home/lxl/Develop/capicxx-core-tools -Dtarget.id=org.genivi.commonapi.someip.target clean verify
```

解压得到代码生成工具：

```bash
org.genivi.commonapi.someip.cli.product/target/products/commonapi_someip_generator.zip
cd someip_dev
unzip -d ./commonapi_someip_generator ./capicxx-someip-tools/org.genivi.commonapi.someip.cli.product/target/products/commonapi_someip_generator.zip
chmod +x ./commonapi_someip_generator/commonapi-someip-generator-linux-x86_64
```
自行安装工具到环境

7)生成代码示例：

```bash
commonapi-core-generator-linux-x86_64 -sk ./xxx.fidl
commonapi-someip-generator-linux-x86_64 ./xxx.fdepl
```