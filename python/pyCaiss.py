from ctypes import *


class PyCaiss:
    def __init__(self, path):
        self.caiss = CDLL(path)


