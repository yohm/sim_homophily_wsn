#!/bin/bash

set -eux

SCRIPT_DIR=$(cd $(dirname $0);pwd)
$SCRIPT_DIR/wsn.out $@

$SCRIPT_DIR/network_analysis/run.sh net.edg
$SCRIPT_DIR/network_analysis/plot/plot_all.sh .
python $SCRIPT_DIR/plot_timeseries.py timeseries.dat
$SCRIPT_DIR/infomap/Infomap net.edg . -i link-list -z -2 --overlapping --clu
python $SCRIPT_DIR/parse_clu.py net.clu

