#**************************************************************
# @author Chunel
# @Name caiss-linux-env.sh
# @date 2020/11/15 1:25 上午
# @Desc Caiss在linux上运行需要安装的环境集合
#**************************************************************/

sudo apt-get update     # 更新软件列表
sudo apt-get upgrade    # 更新软件

sudo apt-get install git
sudo apt-get install cmake
sudo apt-get install g++
sudo apt-get install libboost-dev      # 安装boost库
sudo apt-get install libeigen3-dev     # 安装eigen3
sudo cp -r /usr/include/eigen3/Eigen/ /usr/local/include/
