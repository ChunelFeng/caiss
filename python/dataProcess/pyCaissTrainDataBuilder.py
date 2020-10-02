#!/usr/bin/env python
# encoding: utf-8

import os
import json
import codecs

import numpy as np
from datetime import datetime

from dataProcess.pyCaissBert import *


# 生成供caiss训练的文件
def build_train_data(data_path, output_path, bert_model_path):
    tokenizer = build_bert_tokenizer(bert_model_path)

    model = build_bert_model(bert_model_path)

    words_list = []
    fw = open(output_path, 'w+')
    with open(data_path, 'r') as fr:
        for word in fr.readlines():
            words_list.append(word.strip('\n'))    # 读取本地带embedding词语的词表信息

    num = 0
    start = datetime.now()
    for word in words_list:
        indices, segments = tokenizer.encode(first=word, max_len=4)
        # 在词向量训练任务中，固定获取第一个词语的信息。因为第0个是[CLS]
        tensor = model.predict([np.array([indices]), np.array([segments])])[0][1]
        result_dict = {word: [str(tensor[i])[0:8] for i in range(0, len(tensor))]}
        fw.writelines(json.dumps(result_dict) + '\n')

        num += 1
        if 0 == num % 100:
            print('[caiss] bert predict [{0} / {1}] words, time cost is {2}.'
                  .format(num, len(words_list), datetime.now() - start))
            start = datetime.now()

    return


def main():
    # 开启bert服务
    bert_model_path = r'/home/chunel/model/bert_model/uncased_L-12_H-768_A-12/'    # bert模型所在的文件路径
    embedding_file_path = r'./doc/english-words-71290.txt'    # 获取需要处理的文档
    result_path = r'./doc/caiss_train.txt'    # 训练结束后，可供caiss训练的文件的位置

    # 构造可供caiss训练的文件内容
    print('[caiss] begin to build train data...')
    start = datetime.now()
    build_train_data(embedding_file_path, result_path, bert_model_path)
    print('[caiss] build train data finished, time cost is {0}...'.format(datetime.now() - start))


# 执行以下逻辑，获取用于caiss库训练的文件内容
if __name__ == '__main__':
    main()
