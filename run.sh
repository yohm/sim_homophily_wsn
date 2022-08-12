#!/bin/bash

set -eux

if [ $# -ne 11 ]; then
  echo "[Error] invalid number of arguments $#" 1>&2
  echo "  Usage: ./run.sh <N> <p_tri> <p_jump> <p_ld> <w_r> <F> <q> <N_g> <w_k> <t_max> <seed>" 1>&2
  exit 1
fi

SCRIPT_DIR=$(cd $(dirname $0);pwd)
time $SCRIPT_DIR/wsn.out $1 $2 $3 $4 1.0 0.1 $5 $6 $7 $8 $9 ${10} ${11}

$SCRIPT_DIR/network_analysis/run.sh net.edg
$SCRIPT_DIR/network_analysis/plot/plot_all.sh .
python $SCRIPT_DIR/plot_timeseries.py timeseries.dat

