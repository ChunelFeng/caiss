#!/usr/bin/env python
# encoding: utf-8

import json

import tornado.ioloop
import tornado.web
from bert_serving.client import BertClient
#from bert_serving.server.helper import get_args_parser
#from bert_serving.server import BertServer

from python.pyCaiss import *

CAISS_LIB_PATH = r'../doc/linux/libCaiss.so'                    # caiss动态库所在路径
CAISS_MODEL_PATH = r'../doc/demo_2500words_768dim.caiss'     # caiss模型所在路径
BERT_MODEL_PATH = r'./uncased_L-12_H-768_A-12'          # bert模型所在路径

MAX_THREAD_SIZE = 1    # caiss最大并发数量（推荐不超过cpu核数）
DIM = 768              # 数据维度


class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.write("Hello, welcome to the world of Caiss")


class CaissWordHandler(tornado.web.RequestHandler):
    def get(self):
        query_word = self.get_argument('query', '')
        if len(query_word) == 0:
            self.write('please enter query word.')
            return
        top_k = self.get_argument('top', '5')

        ret, result_str = caiss.sync_search(handle, query_word, CAISS_SEARCH_WORD, int(top_k), 0)
        if 2 == ret:
            self.write('this is not a word : [' + query_word + ']')
            return
        elif 0 != ret:
            self.write('search failed for the reason of : [' + ret + ']')
            return

        result_dict = json.loads(result_str)
        word_list = list()
        for info in result_dict['details']:
            word_list.append(info['label'])

        self.write('the query word is [' + query_word + '].')
        self.write('<br>')
        self.write('the word you also want to know maybe : ')
        self.write(str(word_list))
        self.write('.<br>')


class CaissSentenceHandler(tornado.web.RequestHandler):
    def get(self):
        pass
        # query_sent = self.get_argument('sent', '')
        # if len(query_sent) == 0:
        #     self.write('please enter sentence info.')
        #     return
        #
        # if query_sent[0].isalnum() is False:
        #     self.write('please enter english sentence.')
        #     return
        #
        # res = bert_client.encode([query_sent])
        # res_vec = res[0].tolist()
        #
        # top_k = self.get_argument('top', '3')
        # ret, result_str = caiss.sync_search(handle, res_vec, CAISS_SEARCH_QUERY, int(top_k), 0)
        # if 0 != ret:
        #     self.write('search failed for the reason of : ' + ret)
        #     return
        #
        # result_dict = json.loads(result_str)
        # sent_list = list()
        # for info in result_dict['details']:
        #     sent_list.append(info['label'])
        #
        # self.write('the query sentence is [' + query_sent + '].')
        # self.write('<br>')
        # self.write('the info you also want to know maybe : ')
        # self.write('<br>')
        # for i in sent_list:
        #     self.write('****' + i)
        #     self.write('<br>')


def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/caiss/word", CaissWordHandler),
        (r'/caiss/sentence', CaissSentenceHandler)
    ])


def tornado_server_start():
    app = make_app()
    app.listen(8888)
    tornado.ioloop.IOLoop.current().start()


# def bert_server_start():
#     # 感谢哈工大人工智能团队提供的bert服务
#     args = get_args_parser().parse_args(['-num_worker', '1',
#                                          '-model_dir', BERT_MODEL_PATH,
#                                          '-port', '5555',
#                                          '-port_out', '5556',
#                                          '-max_seq_len', 'NONE',
#                                          '-mask_cls_sep',
#                                          '-cpu'])
#     bert_server = BertServer(args)
#     bert_server.start()


if __name__ == "__main__":
    # http://127.0.0.1:8888/caiss/word?query=water
    #bert_server_start()    # 开启bert服务
    print('[caiss] bert server start success...')

    #bert_client = BertClient()
    print('[caiss] bert client start success...')

    caiss = PyCaiss(CAISS_LIB_PATH, MAX_THREAD_SIZE, CAISS_ALGO_HNSW, CAISS_MANAGE_SYNC)
    handle = c_void_p(0)
    caiss.create_handle(handle)
    caiss.init(handle, CAISS_MODE_PROCESS, CAISS_DISTANCE_INNER, DIM, CAISS_MODEL_PATH)
    print('[caiss] environment init success...')

    tornado_server_start()    # 开启tornado服务，对外提供能力

    caiss.destroy(handle)

