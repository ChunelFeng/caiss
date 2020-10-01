#!/usr/bin/env python3
# encoding: utf-8

from python.pyCaiss import *

LIB_PATH = './doc/linux.libCaiss.so'    # caiss动态库对应的路径
MODEL_FILE_PATH = '/home/chunel/model/caiss_model/bert_71290words_768dim.caiss'    # 最终生成的模型文件
TRAIN_FILE_PATH = '/home/chunel/model/caiss_model/bert_71290words_768dim.txt'    # 用于训练的样本文件
MAX_THREAD_SIZE = 1
DIM = 768
WORD = 'water'
TOP_K = 5
SEARCH_TYPE = CAISS_SEARCH_WORD
FILTER_EDIT_DISTANCE = 0
MAX_DATA_SIZE = 100000
NORMALIZE = 1
MAX_INDEX_SIZE = 64
PRECISION = 0.98
FAST_RANK = 5
REAL_RANK = 5
STEP = 1
MAX_EPOCH = 3
SHOW_SPAN = 1000


def train_demo():
    # 训练样例
    caiss = PyCaiss(LIB_PATH, MAX_THREAD_SIZE, CAISS_ALGO_HNSW, CAISS_MANAGE_SYNC)
    handle = c_void_p(0)
    ret = caiss.create_handle(handle)
    if CAISS_RET_OK != ret:
        print('create handle error, ret = {0}'.format(ret))
        return

    ret = caiss.init(handle, CAISS_MODE_TRAIN, CAISS_DISTANCE_INNER, DIM, MODEL_FILE_PATH)
    if CAISS_RET_OK != ret:
        print('init error, ret = {0}'.format(ret))
        return

    ret = caiss.train(handle, TRAIN_FILE_PATH, MAX_DATA_SIZE, NORMALIZE, MAX_INDEX_SIZE, PRECISION, FAST_RANK, REAL_RANK, STEP, MAX_EPOCH, SHOW_SPAN)
    if CAISS_RET_OK != ret:
        print('train error, ret = {0}'.format(ret))
        return

    caiss.destroy(handle)
    return


def search_demo():
    # 查询样例
    caiss = PyCaiss(LIB_PATH, MAX_THREAD_SIZE, CAISS_ALGO_HNSW, CAISS_MANAGE_SYNC)

    handle = c_void_p(0)
    ret = caiss.create_handle(handle)
    if CAISS_RET_OK != ret:
        print('create handle error, ret = {0}'.format(ret))
        return

    ret = caiss.init(handle, CAISS_MODE_PROCESS, CAISS_DISTANCE_INNER, DIM, MODEL_FILE_PATH)
    if CAISS_RET_OK != ret:
        print('init error, ret = {0}'.format(ret))
        return

    ret, result = caiss.sync_search(handle, WORD, SEARCH_TYPE, TOP_K, FILTER_EDIT_DISTANCE)
    if CAISS_RET_OK != ret:
        print('sync_search error, ret = {0}'.format(ret))
    else:
        print(result)    # 如果查询ok，则返回结果信息

    caiss.destroy(handle)
    return


if __name__ == '__main__':
    train_demo()
    #search_demo()
