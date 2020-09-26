#!/usr/bin/env python
# encoding: utf-8

"""
@author: Chunel
@contact: chunel@foxmail.com
@file: summary.py
@time: 2020/9/26 4:01 下午
@desc:
"""

import datetime
import json
import sys
import time


class SummaryLog:
    def __init__(self):
        self.time = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time()))
        self.ts_start = int(round(time.time() * 1000))
        self.query = ''
        self.ts_cost = 0
        self.result = []

    def record(self, query, result):
        self.query = query
        self.result = result

    def __str__(self):
        return json.dumps({
            'time': self.time,
            'ts_cost': int(round(time.time() * 1000)) - self.ts_start,
            'query': self.query,
            'result': self.result
        })
