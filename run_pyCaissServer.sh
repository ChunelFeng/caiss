#!/bin/bash

echo "nohup run caiss server begin..."
nohup python3 python/pyCaissServer.py >>output.log 2>&1 &