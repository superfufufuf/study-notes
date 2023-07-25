在Linux系统上，按照以下步骤编译&&安装vsomeip库：
编译安装vsomeip
```bash
git clone https://github.com/COVESA/vsomeip.git
cd vsomeip/
mkdir build
cd build/
cmake -DENABLE_SIGNAL_HANDLING=1 ..
make
sudo make install
```
