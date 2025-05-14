#!/bin/bash

A=1; B=0;
for a in $(ls /usr/share)
do
  [ -d /usr/share/$a ] && let B++
done
echo "$B, $A"
