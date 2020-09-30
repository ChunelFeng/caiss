#!/usr/bin/env python
# encoding: utf-8

import os
import json
import datetime
import codecs
import numpy as np

from keras_bert import *


def build_bert_layer(bert_path, trainable=True, training=False, seq_len=None, name='caiss'):
    bert_config_path = os.path.join(bert_path, 'bert_config.json')
    bert_checkpoint_path = os.path.join(bert_path, 'bert_model.ckpt')

    bert_layer = load_trained_model_from_checkpoint(
        bert_config_path, bert_checkpoint_path, training=training, seq_len=seq_len)

    bert_layer.name = name

    for layer in bert_layer.layers:
        layer.trainable = trainable

    return bert_layer


# 通过bert构建词向量信息
def bert_predict(data, bert_model_path):
    token_dict = {}
    with codecs.open(bert_model_path + 'vocab.txt', 'r', 'utf8') as reader:
        for line in reader:
            token = line.strip()
            token_dict[token] = len(token_dict)    # 从bert的词表中读取信息

    tokenizer = Tokenizer(token_dict)
    tokens = tokenizer.tokenize(data)

    model = build_bert_layer(bert_model_path)
    indices, segments = tokenizer.encode(first=data, max_len=len(tokens) + 100)
    predicts = model.predict([np.array([indices]), np.array([segments])])[0]
    return tokens, predicts    # 返回标签信息，和预测的结果信息


# 生成供caiss训练的文件
def build_train_data(data_path, output_path, bert_model_path):
    words = ''
    with open(data_path, 'r') as fr:
        for word in fr.readlines():
            words = words + ' ' + word     # 读入本地词表中的词语信息

    print('begin to embedding...')
    start = datetime.datetime.now()
    tokens, tensors = bert_predict(data=words, bert_model_path=bert_model_path)
    print('bert embedding finished, time cost is : {0}'.format(datetime.datetime.now() - start))

    fw = open(output_path, 'w+')
    for i in range(1, len(tokens)-1):    # 第一个信息是[CLS]，最后一个信息是[SEP]
        if 0 == i % 1000:
            print('already process [{0}] lines, [{1}] lines was left.'.format(i, len(tokens) - i))
        result_dict = {tokens[i]: [str(tensors[i][j]) for j in range(0, len(tensors[i]))]}
        fw.writelines(json.dumps(result_dict) + '\n')
    return


# 执行以下逻辑，获取用于caiss库训练的文件内容
if __name__ == '__main__':
    # 开启bert服务
    print('[caiss] bert server has been started')
    bert_model_path = r'/home/chunel/model/bert_model/uncased_L-12_H-768_A-12/'    # bert模型所在的文件路径

    embedding_file_path = r'./doc/english-words-71290.txt'    # 获取需要处理的文档
    result_path = r'./doc/caiss_train.txt'    # 训练结束后，可供caiss训练的文件的位置
    # 构造可供caiss训练的文件内容
    build_train_data(embedding_file_path, result_path, bert_model_path)
