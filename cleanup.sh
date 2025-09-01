#!/usr/bin/bash

source ./config.sh

# mkdir for your DB files and copy them over from instructor's
if [ -d $MYDBDIR ]; then 
  echo $MYDBDIR
  ls -l $MYDBDIR
  rm  $MYDBDIR/* 
  echo $MYDBDIR
  ls -l $MYDBDIR
fi
