#!/usr/bin/env python
# encoding: utf-8
# @Author Chunel
# @Name pyCaissDemo.py
# @Date 2020/9/15 12:56 下午
# @Desc Caiss的python版本Demo

from python.pyCaiss import *

LIB_PATH = 'libCaiss.dylib'    # Caiss动态库对应的路径
MODEL_FILE_PATH = 'demo_2500words_768dim.caiss'    # 最终生成的模型文件
TRAIN_FILE_PATH = 'demo_2500words_768dim.txt'    # 用于训练的样本文件
MAX_THREAD_SIZE = 1
DIM = 768
WORD = 'water'
TOP_K = 5
SEARCH_TYPE = CAISS_SEARCH_WORD
FILTER_EDIT_DISTANCE = 0
MAX_DATA_SIZE = 5000
NORMALIZE = 1
MAX_INDEX_SIZE = 64
PRECISION = 0.98
FAST_RANK = 5
REAL_RANK = 5
STEP = 1
MAX_EPOCH = 3
SHOW_SPAN = 1000


def train_demo(caiss):
    # 训练样例
    handle = c_void_p(0)
    ret = caiss.create_handle(handle)
    if CAISS_RET_OK != ret:
        print('create handle error, ret = {0}'.format(ret))
        return ret

    ret = caiss.init(handle, CAISS_MODE_TRAIN, CAISS_DISTANCE_INNER, DIM, MODEL_FILE_PATH)
    if CAISS_RET_OK != ret:
        print('init error, ret = {0}'.format(ret))
        return ret

    ret = caiss.train(handle, TRAIN_FILE_PATH, MAX_DATA_SIZE, NORMALIZE, MAX_INDEX_SIZE, PRECISION, FAST_RANK, REAL_RANK, STEP, MAX_EPOCH, SHOW_SPAN)
    if CAISS_RET_OK != ret:
        print('train error, ret = {0}'.format(ret))
        return ret

    ret = caiss.destroy(handle)
    return ret


def search_demo(caiss):
    # 查询样例
    handle = c_void_p(0)
    ret = caiss.create_handle(handle)
    if CAISS_RET_OK != ret:
        print('create handle error, ret = {0}'.format(ret))
        return ret

    ret = caiss.init(handle, CAISS_MODE_PROCESS, CAISS_DISTANCE_INNER, DIM, MODEL_FILE_PATH)
    if CAISS_RET_OK != ret:
        print('init error, ret = {0}'.format(ret))
        return ret

    ret, result = caiss.sync_search(handle, WORD, SEARCH_TYPE, TOP_K, FILTER_EDIT_DISTANCE)
    if CAISS_RET_OK != ret:
        print('sync_search error, ret = {0}'.format(ret))
    else:
        print(result)    # 如果查询ok，则返回结果信息

    ret = caiss.destroy(handle)
    return ret


if __name__ == '__main__':
    # 初始化Caiss环境，训练和查询功能均需要使用
    caiss = PyCaiss(LIB_PATH, MAX_THREAD_SIZE, CAISS_ALGO_HNSW, CAISS_MANAGE_SYNC)

    train_demo(caiss)

    search_demo(caiss)
