#!/usr/bin/env python
# encoding: utf-8

from bert_serving.server.helper import get_args_parser
from bert_serving.server import BertServer


# 传入bert模型所在的文件夹位置，并启动模型
class CaissBertServer:
    def __init__(self, model_path):
        args = get_args_parser().parse_args(['-num_worker', '4',
                                             '-model_dir', model_path,
                                             '-port', '5555',
                                             '-port_out', '5556',
                                             '-max_seq_len', 'NONE',
                                             '-mask_cls_sep',
                                             '-cpu'])
        # 详细说明，请参考：https://github.com/hanxiao/bert-as-service
        self._server = BertServer(args)

    def start(self):
        self._server.start()

    def close(self):
        self._server.close()


if __name__ == '__main__':
    bert_model_path = r'/uncased_L-12_H-768_A-12'
    server = CaissBertServer(bert_model_path)
    server.start()

    server.close()

