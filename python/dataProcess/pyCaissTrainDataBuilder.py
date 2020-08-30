#!/usr/bin/env python
# encoding: utf-8

import json
import datetime

from bert_serving.client import BertClient
from python.dataProcess.pyCaissBertServer import *


def build_train_data(data_path, output_path):
    words_list = []
    with open(data_path, 'r') as fr:
        for word in fr.readlines():
            words_list.append(word)

    print('begin to embedding')
    start = datetime.datetime.now()
    client = BertClient()
    tensors = client.encode(words_list)
    print('bert embedding finished, time cost is : {0}'.format(datetime.datetime.now() - start))

    fw = open(output_path, 'w+')
    for i in range(0, len(words_list)):
        if 0 == i % 1000:
            print('already process [{0}] lines, [{1}] lines was left.'.format(i, len(words_list) - i))
        result_dict = {words_list[i].strip('\n'): [str(tensors[i][j]) for j in range(0, len(tensors[i]))]}
        fw.writelines(json.dumps(result_dict) + '\n')
    return


# 执行以下逻辑，获取用于caiss库训练的文件内容
if __name__ == '__main__':
    # 开启bert服务
    bert_model_path = r'./uncased_L-12_H-768_A-12'    # bert模型所在的文件路径
    server = CaissBertServer(bert_model_path)
    server.start()
    print('bert server has been started')

    embedding_file_path = r'./doc/english-words-71290.txt'    # 获取需要处理的文档
    result_path = r'./doc/caiss_train.txt'    # 训练结束后，可供caiss训练的文件的位置
    # 构造可供caiss训练的文件内容
    build_train_data(embedding_file_path, result_path)

    # 关闭bert服务
    server.close()
    print('bert server has been closed')

