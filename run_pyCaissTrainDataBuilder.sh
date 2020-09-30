#!/bin/bash

echo "run caiss train data builder begin..."
cd python/dataProcess
python3 pyCaissTrainDataBuilder.py
echo "run caiss train data builder finished..."