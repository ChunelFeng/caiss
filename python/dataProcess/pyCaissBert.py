#!/usr/bin/env python
# encoding: utf-8

"""
@author: Chunel
@contact: chunel@foxmail.com
@file: pyCaissBert.py
@time: 2020/10/2 2:33 下午
@desc:
"""

import os
import codecs
from keras_bert import *


def build_bert_tokenizer(bert_model_path):
    # 根据传入的信息，构造bert的tokenizer信息
    token_dict = {}
    print('[caiss] begin to load bert vocab.txt...')
    with codecs.open(bert_model_path + 'vocab.txt', 'r', 'utf8') as reader:
        for line in reader:
            token = line.strip()
            token_dict[token] = len(token_dict)    # 从bert的词表中读取信息
    return Tokenizer(token_dict)


def build_bert_model(bert_path, trainable=True, training=False, seq_len=None):
    # 根据传入的路径信息，构建bert模型
    print('[caiss] begin to build bert model...')
    bert_config_path = os.path.join(bert_path, 'bert_config.json')
    bert_checkpoint_path = os.path.join(bert_path, 'bert_model.ckpt')

    bert_layer = load_trained_model_from_checkpoint(
        bert_config_path, bert_checkpoint_path, training=training, seq_len=seq_len)

    for layer in bert_layer.layers:
        layer.trainable = trainable

    print('[caiss] build bert model success...')
    return bert_layer
