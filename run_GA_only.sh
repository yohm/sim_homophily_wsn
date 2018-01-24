#!/bin/bash

set -eux

if [ $# -ne 5 ]; then
  echo "[Error] invalid number of arguments $#" 1>&2
  echo "  Usage: ./run.sh <N> <p_jump> <F> <q> <seed>" 1>&2
  exit 1
fi

SCRIPT_DIR=$(cd $(dirname $0);pwd)
P_TRI=0.0
P_LD=0.0
T_MAX=1000
time $SCRIPT_DIR/wsn.out $1 $P_TRI $2 $P_LD 1.0 0.1 $3 $4 $T_MAX $5

$SCRIPT_DIR/network_analysis/run.sh net.edg

