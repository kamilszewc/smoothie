#!/usr/bin/env python
# -*- coding: utf-8 -*-
# All right reserved by Kamil Szewc, Gdansk 2010

import bz2

class DataFile:
    """Data - reads data stored in text files"""

    def __init__(self, filename, type="str"):
        if isinstance(filename, str):
            self.__read__(filename)
        if type == "float":
            for i in range(self.rawnum()):
                for j in range(self.colnum()):
                    self.data[i][j] = float(self.data[i][j])

    def __read__(self, filename):
        try:
            if filename.split(".")[-1] == "bz2":
                file = bz2.BZ2File(filename, 'r')
            else:
                file = open(filename, 'r')
        except:
            print "Error: There is no '%s' file" % filename

        datalines = file.readlines()
        file.close()

        self.data = [] 

        for dataline in datalines:
            self.data.append([])
            for data in dataline.split():
                self.data[len(self.data)-1].append(data)
                

    def rawnum(self):
        return len(self.data)

    def colnum(self):
        return len(self.data[0])

    def __checkcol__(self, col):
        if (col > self.colnum()) or (col <= 0):
            try:
                raise ValueError()
            except:
                print 'Bad col number'
                raise

    def __checkraw__(self, raw):
        if (raw > self.rawnum()) or (raw <= 0):
            try:
                raise ValueError()
            except:
                print 'Bad raw number'
                raise

    def get(self, col, raw):
        self.__checkraw__(raw)
        self.__checkcol__(col)
        return self.data[raw-1][col-1]

    def getraw(self, raw):
        self.__checkraw__(raw)
        return self.data[raw-1]

    def getcol(self, col):
        self.__checkcol__(col)
        column = [raw[col-1] for raw in self.data]
        return column

    def getall(self):
        return self.data

