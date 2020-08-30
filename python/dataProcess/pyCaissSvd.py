#!/usr/bin/env python
# encoding: utf-8

import json
import numpy as np
import datetime
import matplotlib.pyplot as plt
from sklearn.decomposition import TruncatedSVD
import joblib

N_COMPONENTS = 600    # 降维后，数据的维度信息
DIM = 768             # 原始维度信息
SVD_MODEL_PATH = './caiss-svd.plk'    # svd模型的名称
BERT_PROCESS_TEXT_PATH = './bert_71290words_768dim.txt'
SVD_PROCESS_TEST_PATH = './svd_71290words_{0}dim.txt'.format(N_COMPONENTS)


def svd_load():
    # 重新拉起svd模型
    svd = joblib.load(SVD_MODEL_PATH)
    test = [i for i in range(0, DIM)]
    result = svd.transform(np.asarray([test]))    # 生成一条数据，测试其降维后的结果
    print(result)


def svd_process():
    result_key = []
    result_val = []
    print('start load info')
    with open(BERT_PROCESS_TEXT_PATH, 'r') as fr:
        lines = fr.readlines()
        for line in lines:
            line_json = json.loads(line)
            result_key.append(list(line_json.keys())[0])    # 用于记录
            result_val.append(list(line_json.values())[0])

    print('start svd')
    array = np.asarray(result_val, dtype=float)
    start = datetime.datetime.now()
    svd = TruncatedSVD(n_components=N_COMPONENTS)

    x_dr = svd.fit_transform(array)

    print(datetime.datetime.now() - start)
    joblib.dump(svd, SVD_MODEL_PATH)    # 保存模型的位置信息

    start = datetime.datetime.now()
    fw = open(SVD_PROCESS_TEST_PATH, 'w+')    # 将降维后的数据，写入txt文档中
    for i in range(0, len(result_key)):
        result_dict = {result_key[i]: [str(round(float(x_dr[i][j]), 6)) for j in range(0, len(x_dr[i]))]}
        fw.writelines(json.dumps(result_dict) + '\n')
    print('svd save txt cost {}'.format(datetime.datetime.now() - start))
    return


def svd_lookup():
    result_key = []
    result_val = []
    with open(BERT_PROCESS_TEXT_PATH, 'r') as fr:
        lines = fr.readlines()
        for line in lines:
            line_json = json.loads(line)
            result_key.append(list(line_json.keys())[0])    # 用于记录
            result_val.append(list(line_json.values())[0])

    array = np.asarray(result_val, dtype=float)
    print('begin to fit')
    start = datetime.datetime.now()
    svd_line = TruncatedSVD(n_components=N_COMPONENTS).fit(array)
    end = datetime.datetime.now() - start
    print('svd look up cost {0}'.format(end))

    show_list = [i+1 for i in range(0, N_COMPONENTS)]    # 跟pca有所区别

    plt.plot(show_list, np.cumsum(svd_line.explained_variance_ratio_))    # 查看累加贡献比
    plt.xticks(show_list)
    plt.show()
    print('finished ...')


if __name__ == '__main__':
    #svd_lookup()
    #svd_process()
    svd_load()
