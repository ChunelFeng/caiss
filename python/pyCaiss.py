#!/usr/bin/env python
# encoding: utf-8
# @Author Chunel
# @Name pyCaiss.py
# @Date 2020/9/15 12:53 下午
# @Desc Caiss的python版本

from ctypes import *

CAISS_MODE_DEFAULT = 0
CAISS_MODE_TRAIN = 1
CAISS_MODE_PROCESS = 2

CAISS_SEARCH_QUERY = 1
CAISS_SEARCH_WORD = 2
CAISS_LOOP_QUERY = 3
CAISS_LOOP_WORD = 4

CAISS_INSERT_OVERWRITE = 1
CAISS_INSERT_DISCARD = 2

CAISS_MANAGE_SYNC = 1
CAISS_MANAGE_ASYNC = 2

CAISS_DISTANCE_EUC = 1
CAISS_DISTANCE_INNER = 2
CAISS_DISTANCE_JACCARD = 3
CAISS_DISTANCE_EDITION = 99

CAISS_ALGO_HNSW = 1
CAISS_ALGO_MRPT = 2

# 异常值信息
CAISS_RET_WARNING = 1         # 流程告警
CAISS_RET_OK = 0              # 流程正常
CAISS_RET_ERR = -1            # 流程异常
CAISS_RET_RES = -2            # 资源问题
CAISS_RET_MODE = -3           # 模式选择问题
CAISS_RET_PATH = -4           # 路径问题
CAISS_RET_JSON = -5           # json解析问题
CAISS_RET_PARAM = -6          # 参数问题
CAISS_RET_HANDLE = -7         # 句柄申请问题
CAISS_RET_DIM = -8            # 维度问题
CAISS_RET_MODEL_SIZE = -9     # 模型尺寸限制问题
CAISS_RET_WORD_SIZE = -10     # 词语长度限制问题
CAISS_RET_SQL_PARSE = -11     # 传入的sql无法解析
CAISS_RET_NO_SUPPORT = -99    # 暂不支持该功能


class PyCaiss:
    def __init__(self, path, max_thread_size, algo_type, manage_type):
        self._caiss = CDLL(path)    # 如有路径报错问题，请尝试使用绝对路径
        self._dim = 0
        self._caiss.CAISS_Environment(max_thread_size, algo_type, manage_type)

    def create_handle(self, handle):
        return self._caiss.CAISS_CreateHandle(pointer(handle))

    def init(self, handle, mode, distance_type, dim, model_path):
        path = create_string_buffer(model_path.encode(), len(model_path)+1)
        ret = self._caiss.CAISS_Init(handle, mode, distance_type, dim, path)
        if CAISS_RET_OK == ret:
            self._dim = dim
        return ret

    def train(self, handle, data_path, max_data_size, normalize,
              max_index_size, precision, fast_rank, real_rank, step, max_epoch, show_span):
        path = create_string_buffer(data_path.encode(), len(data_path)+1)
        precision = c_float(precision)
        return self._caiss.CAISS_Train(handle, path, max_data_size, normalize,
                                       max_index_size, precision, fast_rank, real_rank, step, max_epoch, show_span)

    def sync_search(self, handle, info, search_type, top_k, filter_edit_distance):
        if search_type == CAISS_SEARCH_QUERY or search_type == CAISS_LOOP_QUERY:
            # 如果传入的是数组信息，需要将数组转成指针传递下去
            if self._dim != len(info):
                return CAISS_RET_DIM, ''

            vec = (c_float * self._dim)()
            for i in range(0, self._dim):
                vec[i] = info[i]
            ret = self._caiss.CAISS_Search(handle, vec, search_type, top_k, filter_edit_distance, None, None)
        else:
            word = create_string_buffer(info.encode(), len(info)+1)
            ret = self._caiss.CAISS_Search(handle, word, search_type, top_k, filter_edit_distance, None, None)

        if CAISS_RET_OK != ret:
            return ret, ''

        size = c_int(0)    # 获取结果大小
        ret = self._caiss.CAISS_GetResultSize(handle, byref(size))
        if CAISS_RET_OK != ret:
            return ret, ''

        result = create_string_buffer(size.value)
        ret = self._caiss.CAISS_GetResult(handle, result, size)
        if CAISS_RET_OK != ret:
            return ret, ''

        return ret, result.value.decode()

    def sync_execute_sql(self, handle, sql):
        sql = create_string_buffer(sql.encode(), len(sql)+1)
        ret = self._caiss.CAISS_ExecuteSQL(handle, sql, None, None)
        if CAISS_RET_OK != ret:
            return ret, ''

        size = c_int(0)    # 获取结果大小
        ret = self._caiss.CAISS_GetResultSize(handle, byref(size))
        if CAISS_RET_OK != ret:
            return ret, ''

        result = create_string_buffer(size.value)
        ret = self._caiss.CAISS_GetResult(handle, result, size)
        if CAISS_RET_OK != ret:
            return ret, ''

        return ret, result.value.decode()

    def insert(self, handle, node, label, insert_type):
        if self._dim != len(node):
            return CAISS_RET_DIM

        vec = (c_float * self._dim)()
        for i in range(0, self._dim):
            vec[i] = node[i]

        label_ = create_string_buffer(label.encode(), len(label)+1)
        return self._caiss.CAISS_Insert(handle, vec, label_, insert_type)

    def ignore(self, handle, label, is_ignore=True):
        label_ = create_string_buffer(label.encode(), len(label)+1)
        return self._caiss.CAISS_Ignore(handle, label_, is_ignore)

    def save(self, handle, model_path=None):
        model_path_ = create_string_buffer(model_path.encode(), len(model_path)+1) if model_path else None
        return self._caiss.CAISS_Save(handle, model_path_)

    def destroy(self, handle):
        return self._caiss.CAISS_DestroyHandle(handle)
