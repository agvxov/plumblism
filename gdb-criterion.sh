#!/bin/sh
gdb --eval-command 'target remote localhost:1234' test-criterion.out
