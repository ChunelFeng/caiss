#**************************************************************
# @author Chunel
# @Name caiss-linux-env.sh
# @date 2020/11/15 1:25 上午
# @Desc Caiss在linux上运行需要安装的环境集合
#**************************************************************/

echo -e "\033[32m ************************************************************** \033[0m"
echo -e "\033[32m [caiss] begin to update environment, please always enter [Y] if needed. \033[0m"
apt-get update     # 更新软件列表
apt-get upgrade    # 更新软件

echo -e "\033[32m [caiss] begin to install [git], please enter [Y] if needed. \033[0m"
apt-get install git

echo -e "\033[32m [caiss] begin to install [cmake], please enter [Y] if needed. \033[0m"
apt-get install cmake

echo -e "\033[32m [caiss] begin to install [g++], please enter [Y] if needed. \033[0m"
apt-get install g++

echo -e "\033[32m [caiss] begin to install [boost], please enter [Y] if needed. \033[0m"
apt-get install libboost-dev      # 安装boost库

echo -e "\033[32m [caiss] begin to install [eigen3], please enter [Y] if needed. \033[0m"
apt-get install libeigen3-dev     # 安装eigen3
cp -r /usr/include/eigen3/Eigen/ /usr/local/include/

echo -e "\033[32m [caiss] congratulation! U have been finished install environment all Caiss needed on Linux. \033[0m"
echo -e "\033[32m [caiss] please go on as README described. \033[0m"
echo -e "\033[32m ************************************************************** \033[0m"
