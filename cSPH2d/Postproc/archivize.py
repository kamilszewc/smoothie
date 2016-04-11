#!/usr/bin/env python

import os
import re
import bz2
import hashlib

files = os.listdir("Results")
files = filter( (lambda files: re.match(r"\d{1,}.\d{6}.dat$", files)), files)

for file in files:
    if file.split(".")[-1] == 'dat':
        print "Archivize %s" % (file)
        try:
            f = open("Results/" + file, "r")
            data = f.read()
            f.close()
        except:
            print "Error reading files"
            exit()

        try:
            f = bz2.BZ2File("Results/" + file + ".bz2", "w")
            f.write(data)
            f.close()
        except:
            print "Error writing files"
            exit()

        try:
            os.remove("Results/" + file)
        except:
            print "Error removing files"

        try:
            shanum = hashlib.sha256()
            shanum.update(data) 
            f = open("Results/" + "shasum.dat", "a")
            f.write("%s\t%s" % (file, shanum.hexdigest()) )
            f.close()
        except:
            print "Error creating sha sum"

