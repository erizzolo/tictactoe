#!/bin/bash
stty_orig=`stty -g`
stty -echo
./main
stty $stty_orig
