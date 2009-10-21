#!/usr/bin/python

import sys
import os
import subprocess
import time

if len(sys.argv) != 2:
	print "please enter number of try-runs"
	exit(0)

n = int(sys.argv[1])
print n

runs = 0
failures = 0

for i in range(n):
	runs = runs + 1
	proc = subprocess.Popen(["./main"]);
	time.sleep(0.25)
	ret = proc.poll()
	if ret  == None:
		proc.terminate()
		print ret
	else:
		failures = failures + 1
		print ret

print "Summary:"
print "Runs:", runs
print "Failures:", failures
