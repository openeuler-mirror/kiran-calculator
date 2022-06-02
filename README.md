# kiran-calculator
## 编译

- 安装编译依赖
`sudo yum install gcc-c++ qt5-qtbase-devel qt5-linguist kiran-widgets-qt5-devel kiran-log-qt5-devel`
- 在源码根目录下创建**build**目录
`mkdir build`
- 生成**Makefile**
`cmake -DCMAKE_INSTALL_PREFIX=/usr ..`
- 编译
`make -j4`

## 安装
- 编译之后,在**build**目录下执行下述命令即可
`sudo make install`

## 运行
`$ /usr/bin/kiran-calculator`
