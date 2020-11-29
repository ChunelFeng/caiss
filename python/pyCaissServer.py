#!/usr/bin/env python3
# encoding: utf-8

import json
import sys

import numpy as np
import tornado.ioloop
import tornado.web

from python.dataLogs.summary import SummaryLog
from python.pyCaiss import *
from python.dataProcess.pyCaissTrainDataBuilder import *

CAISS_LIB_PATH = r'/home/chunel/code/cpp/caiss/doc/linux/libCaiss.so'                    # caiss动态库所在路径
CAISS_MODEL_PATH = r'/home/chunel/model/caiss_model/bert_71290words_768dim.caiss'     # caiss模型所在路径
BERT_MODEL_PATH = r'/home/chunel/model/bert_model/uncased_L-12_H-768_A-12/'          # bert模型所在路径

MAX_THREAD_SIZE = 1    # caiss最大并发数量
DIM = 768              # 数据维度


def show_info(tnd, info):
    # 展示信息
    search_result = json.loads(info)
    result_list = search_result.get('result')
    for result in result_list:
        tnd.write('The query info is : [{0}], '.format(result.get('query')))

        result_words = []
        for detail in result.get('details'):
            result_words.append(detail.get('label'))
        tnd.write('and the similar words maybe : {} <br>'.format(result_words))


class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.write("Hello, welcome to the world of Caiss.")


class CaissWordHandler(tornado.web.RequestHandler):
    def get(self):
        logs = SummaryLog()
        query_word = self.get_argument('query', '').lower()
        if len(query_word) == 0:
            self.write('please enter query word.')
            return
        top_k = self.get_argument('top', '5')

        ret, result_str = caiss.sync_search(handle, query_word, CAISS_SEARCH_WORD, int(top_k), 0)
        if 0 != ret:
            self.write('search failed for the reason of : [' + ret + ']')
            return

        show_info(self, result_str)

        self.write('<br>')
        self.write('有没前端大佬考虑加入我们，一起做个炫酷的展示页面啊哈哈哈！！！<br>')
        self.write('请随时联系我们哈哈哈！！！<br>')
        self.write('<br>')

        self.write('微信： Chunel_Fung <br>')
        self.write('邮箱： chunel@foxmail.com <br>')
        self.write('源码： ')
        self.write('<a href="https://www.github.com/ChunelFeng/caiss">www.github.com/ChunelFeng/cais</a><br />')
        self.write('论坛： ')
        self.write('<a href="http://www.chunel.cn">www.chunel.cn</a><br />')

        self.write('<br>')


class CaissSentenceHandler(tornado.web.RequestHandler):
    def get(self):
        query_sent = self.get_argument('sent', '')
        if len(query_sent) == 0:
            self.write('please enter sentence info.')
            return

        if query_sent[0].isalnum() is False:
            self.write('please enter english sentence.')
            return

        indices, segments = tokenizer.encode(first=query_sent, max_len=200)    # 句子最长200个单词
        # 在词向量训练任务中，固定获取第一个词语的信息
        res_vec = bert_model.predict([np.array([indices]), np.array([segments])])[0][0].tolist()

        top_k = self.get_argument('top', '3')
        ret, result_str = caiss.sync_search(handle, res_vec, CAISS_SEARCH_QUERY, int(top_k), 0)
        if 0 != ret:
            self.write('search failed for the reason of : ' + ret)
            return

        show_info(self, result_str)


def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/caiss/word", CaissWordHandler),
        (r'/caiss/sentence', CaissSentenceHandler)
    ])


def tornado_server_start():
    # tornado开启服务模式
    app = make_app()
    app.listen(8888)
    tornado.ioloop.IOLoop.current().start()


if __name__ == "__main__":
    # http://127.0.0.1:8888/caiss/word?query=water
    tokenizer = build_bert_tokenizer(BERT_MODEL_PATH)

    bert_model = build_bert_model(BERT_MODEL_PATH)

    caiss = PyCaiss(CAISS_LIB_PATH, MAX_THREAD_SIZE, CAISS_ALGO_HNSW, CAISS_MANAGE_SYNC)
    handle = c_void_p(0)
    caiss.create_handle(handle)
    caiss.init(handle, CAISS_MODE_PROCESS, CAISS_DISTANCE_INNER, DIM, CAISS_MODEL_PATH)
    print('[caiss] environment init success...')

    tornado_server_start()    # 开启tornado服务，对外提供能力

    caiss.destroy(handle)
