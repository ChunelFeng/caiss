#!/bin/bash

echo "begin to run caiss server..."
nohup python3 python/pyCaissServer.py >>output.log 2>&1 &