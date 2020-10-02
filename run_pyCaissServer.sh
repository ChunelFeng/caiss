#!/bin/bash

echo "nohup run caiss server begin..."
nohup python3 run_pyCaissServer.py >>output.log 2>&1 &
