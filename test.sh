#!/bin/env bash
set -e

for FILE in src/*.lf; do
  if [ -f $FILE ]; then
    cmake --fresh -Bbuild -DLF_APP=$(basename "$FILE" .lf) .
    cmake --build build --parallel 12
  fi
done
