#!/usr/bin/env python
# encoding: utf-8

from sklearn.decomposition import PCA
import numpy as np
import matplotlib.pyplot as plt
import json
import datetime
import joblib

N_COMPONENTS = 600    # 降维后，数据的维度信息
DIM = 768             # 原始维度信息
PCA_MODEL_PATH = './caiss-pca.plk'    # pca模型的名称
BERT_PROCESS_TEXT_PATH = './bert_71290words_768dim.txt'
PCA_PROCESS_TEXT_PATH = './pca_71290words_{0}dim.txt'.format(N_COMPONENTS)


def pca_load():
    # 重新拉起pca模型
    pca = joblib.load(PCA_MODEL_PATH)
    test = [i for i in range(0, DIM)]
    result = pca.transform(np.asarray([test]))    # 生成一条数据，测试其降维后的结果
    print(result)


def pca_process():
    # 生成pca模型，并将降维后的数据，放入txt文档中
    result_key = []
    result_val = []
    with open(BERT_PROCESS_TEXT_PATH, 'r') as fr:    # 读取待处理的向量信息和对应的词语信息
        lines = fr.readlines()
        for line in lines:
            line = json.loads(line)
            result_key.append(list(line.keys())[0])
            result_val.append(list(line.values())[0])

    start = datetime.datetime.now()
    array = np.asarray(result_val)

    # PCA相当于是对array的协方差矩阵的top-n个特征矩阵组成的矩阵，乘以原先的array，从而降至n维
    pca = PCA(n_components=N_COMPONENTS)
    x_dr = pca.fit_transform(array)    # x_dr是降维后的信息
    print('pca fit transform cost {}'.format(datetime.datetime.now() - start))

    joblib.dump(pca, PCA_MODEL_PATH)    # 保存模型的位置信息

    start = datetime.datetime.now()
    fw = open(PCA_PROCESS_TEXT_PATH, 'w+')    # 将降维后的数据，写入txt文档中
    for i in range(0, len(result_key)):
        result_dict = {result_key[i]: [str(round(float(x_dr[i][j]), 6)) for j in range(0, len(x_dr[i]))]}
        fw.writelines(json.dumps(result_dict) + '\n')
    print('pca save txt cost {}'.format(datetime.datetime.now() - start))
    return


def pca_lookup():
    # 查找何时的节点信息
    result_key = []
    result_val = []
    with open(BERT_PROCESS_TEXT_PATH, 'r') as fr:
        lines = fr.readlines()
        for line in lines:
            line_json = json.loads(line)
            result_key.append(list(line_json.keys())[0])    # 用于记录
            result_val.append(list(line_json.values())[0])

    array = np.asarray(result_val)
    print('begin to fit')
    start = datetime.datetime.now()
    pca_line = PCA().fit(array)
    end = datetime.datetime.now() - start
    print('pca look up cost {0}'.format(end))

    show_list = [i+1 for i in range(0, DIM)]

    plt.plot(show_list, np.cumsum(pca_line.explained_variance_ratio_))    # 查看累加贡献比
    plt.xticks(show_list)
    plt.show()


if __name__ == '__main__':
    #pca_lookup()
    #pca_process()
    pca_load()
