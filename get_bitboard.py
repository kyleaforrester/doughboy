#!/usr/bin/env python3

import sys

args = [2**int(i) for i in sys.argv[1:]]
print(hex(sum(args)))
