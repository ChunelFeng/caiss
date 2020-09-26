#!/bin/bash

echo "begin to run caiss server..."
nohup python3 pyCaissServer.py >>output.log 2>&1 &