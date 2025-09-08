#!/bin/bash

# Usage: ./run_integrate.sh <a> <b> <steps> <input.csv> <column_index>
echo "✅ Script is running..."

if [ "$#" -ne 5 ]; then
  echo "Usage: $0 <a> <b> <steps> <input.csv> <column_index>"
  exit 1
fi

A=$1
B=$2
STEPS=$3
CSV_FILE=$4
COL_INDEX=$5

echo "➡️  Step 1: Generating header from $CSV_FILE (column $COL_INDEX)..."
python3 dataToHeader.py "$CSV_FILE" output.h "$COL_INDEX"

if [ $? -ne 0 ]; then
  echo "❌ CSV to header conversion failed."
  exit 1
fi

echo "✅ Header generated."

echo "➡️  Step 2: Compiling integration program using Makefile..."
make

if [ $? -ne 0 ]; then
  echo "❌ Compilation failed."
  exit 1
fi

echo "✅ Compilation successful."

echo "➡️  Step 3: Running integration..."
time ./integration_seq "$A" "$B" "$STEPS"