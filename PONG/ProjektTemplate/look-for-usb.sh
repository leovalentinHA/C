#!/bin/bash

for name in `ls /dev/tty*`; do
  echo $name
done