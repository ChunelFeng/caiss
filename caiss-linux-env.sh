#**************************************************************
# @author Chunel
# @Name caiss-linux-env.sh
# @date 2020/11/15 1:25 上午
# @Desc Caiss在linux上运行需要安装的环境集合
#**************************************************************/

apt-get update     # 更新软件列表
apt-get upgrade    # 更新软件

apt-get install git
apt-get install cmake
apt-get install g++
apt-get install libboost-dev      # 安装boost库
apt-get install libeigen3-dev     # 安装eigen3
cp -r /usr/include/eigen3/Eigen/ /usr/local/include/
