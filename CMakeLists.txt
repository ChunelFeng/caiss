cmake_minimum_required(VERSION 3.16)
project(ChunelAnn)

set(CMAKE_CXX_STANDARD 14)


add_library(ChunelAnn SHARED chunelAnnLib/ChunelAnnLib.cpp chunelAnnLib/ChunelAnnLib.h chunelAnnLib/ChunelAnnLibDefine.h algorithmCtrl/AlgorithmProc.h algorithmCtrl/AlgorithmInclude.h algorithmCtrl/hnsw/hnswProc/HnswProc.cpp algorithmCtrl/hnsw/hnswProc/HnswProc.h utilsCtrl/UtilsInclude.h utilsCtrl/UtilsProc.h utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.h)