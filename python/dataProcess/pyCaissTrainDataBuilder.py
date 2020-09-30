#!/usr/bin/env python
# encoding: utf-8

import os
import json
import codecs

import numpy as np
from keras_bert import *


def build_bert_layer(bert_path, trainable=True, training=False, seq_len=None):
    bert_config_path = os.path.join(bert_path, 'bert_config.json')
    bert_checkpoint_path = os.path.join(bert_path, 'bert_model.ckpt')

    bert_layer = load_trained_model_from_checkpoint(
        bert_config_path, bert_checkpoint_path, training=training, seq_len=seq_len)

    for layer in bert_layer.layers:
        layer.trainable = trainable

    return bert_layer


# 生成供caiss训练的文件
def build_train_data(data_path, output_path, bert_model_path):
    token_dict = {}
    with codecs.open(bert_model_path + 'vocab.txt', 'r', 'utf8') as reader:
        for line in reader:
            token = line.strip()
            token_dict[token] = len(token_dict)    # 从bert的词表中读取信息

    tokenizer = Tokenizer(token_dict)
    print('[caiss] begin to build bert model...')
    model = build_bert_layer(bert_model_path)
    print('[caiss] build bert model finished...')

    fw = open(output_path, 'w+')
    with open(data_path, 'r') as fr:
        num = 0
        for word in fr.readlines():
            word = word.strip('\n')
            indices, segments = tokenizer.encode(first=word, max_len=512)
            tensor = model.predict([np.array([indices]), np.array([segments])])[0][0]
            result_dict = {word: [str(tensor[j])[0:8] for j in range(0, len(tensor))]}
            fw.writelines(json.dumps(result_dict) + '\n')

            num += 1
            if 0 == num % 100:
                print('[caiss] bert predict {0} words, and {1} words left'.format(num, len(token_dict) - num))

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
