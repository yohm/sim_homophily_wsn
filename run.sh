#!/bin/bash

set -eux

SCRIPT_DIR=$(cd $(dirname $0);pwd)
$SCRIPT_DIR/wsn.out $@

$SCRIPT_DIR/network_analysis/run.sh net.edg
network_analysis/plot/plot_all.sh .

