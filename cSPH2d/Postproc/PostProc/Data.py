#!/usr/bin/env python
# -*- coding: utf-8 -*-
# All right reserved by Kamil Szewc, Gdansk 2009 

import os.path
from DataFile import DataFile

KERNEL_TYPE = 3

class Data:
    """Data - this class reads particles information from file"""

    def __init__(self, filename):
        self.data = DataFile(filename, "float")
        self.N = self.data.rawnum()
        self.para = DataFile(os.path.split(filename)[0] + '/' + 'parameters.dat')
        self.__read_data__()
        self.__read_param__()
        self.__set_linked_list__()
	if filename.split('.')[-1] == "bz2":
            self.t = os.path.split(filename)[-1].split('.')[0] + '.'
            self.t = self.t + os.path.split(filename)[-1].split('.')[1]
            self.t = float(self.t)
        else:
            self.t = float(os.path.splitext(os.path.split(filename)[-1])[0])
        
    def __read_data__(self):
        self.id = self.data.getcol(1)
        self.phaseId = self.data.getcol(2)
        self.x  = self.data.getcol(3)
        self.y  = self.data.getcol(4)
        self.u  = self.data.getcol(5)
        self.v  = self.data.getcol(6)
        self.m  = self.data.getcol(7)
        self.p  = self.data.getcol(8)
        self.d  = self.data.getcol(9)
        self.di = self.data.getcol(10)
        self.o  = self.data.getcol(11)
        self.nu = self.data.getcol(12)
        self.mi = self.data.getcol(13)
        self.gamma = self.data.getcol(14)
        self.s  = self.data.getcol(15)
        self.b  = self.data.getcol(16)
        self.c  = self.data.getcol(17)
        self.nx = self.data.getcol(18)
        self.ny = self.data.getcol(19)
        self.na = self.data.getcol(20)
        self.cu = self.data.getcol(21)
        self.stx = self.data.getcol(22)
        self.sty = self.data.getcol(23)
        self.tp = self.data.getcol(24)

    def __read_param__(self):
        i=1 
        for raw in self.para.getcol(1):
            value = self.para.get(2,i)
            if (raw == "H"): self.H = float(value)
            if (raw == "I_H"): self.I_H = float(value)
            if (raw == "DH"): self.DH = float(value)
            if (raw == "DR"): self.DR = float(value)
            if (raw == "XCV"): self.XCV = float(value)
            if (raw == "YCV"): self.YCV = float(value)
            #if (raw == "NX"): self.NX = int(value)
            #if (raw == "NY"): self.NY = int(value)
            if (raw == "NCX"): self.NXC = int(value)
            if (raw == "NCY"): self.NYC = int(value)
            if (raw == "T_BOUNDARY_PERIODICITY"): self.B_PER = int(value)
            if (raw == "V_N"): self.V_N = float(value)
            if (raw == "V_S"): self.V_S = float(value)
            if (raw == "V_E"): self.V_E = float(value)
            if (raw == "V_W"): self.V_W = float(value)
            if (raw == "N"): self.N0 = int(value)
            if (raw == "NC"): self.NC = int(value)
            if (raw == "GAMMA"): self.GAMMA = float(value)
            if (raw == "SOUND"): self.SOUND = float(value)
            if (raw == "NU"): self.NU = float(value)
            if (raw == "G_X"): self.G_X = float(value)
            if (raw == "G_Y"): self.G_Y = float(value)
            if (raw == "KNORM"): self.KNORM = float(value)
            if (raw == "GKNORM"): self.GKNORM = float(value)
            if (raw == "TIME"): self.TIME = float(value)
            if (raw == "DT"): self.DT = float(value)
            if (raw == "NOUT"): self.NOUT = int(value)
            if (raw == "SIGMA"): self.SIGMA = float(value)
            if (raw == "T_SHARP"): self.T_SHARP = int(value)
            if (raw == "SHARP"): self.SHARP = float(value)
            if (raw == "T_SURF_TENS"): self.T_SURF_TENS = int(value)
            if (raw == "T_XSPH"): self.T_XSPH = int(value)
            if (raw == "XSPH"): self.XSPH = float(value)
            i+=1
        self.NX = int(self.XCV / self.DR)
        self.NY = int(self.YCV / self.DR)

    def __set_linked_list__(self):
        self.h = [-1 for i in range(self.NC)]
        self.l = [-1 for i in range(self.N)]

        for i in range(self.N):
            if KERNEL_TYPE <= 3:
                xc = long( 0.5*self.x[i]/self.H )
                yc = long( 0.5*self.y[i]/self.H )
            else:
                xc = long( self.x[i]/ (self.H*3.0) )
                yc = long( self.y[i]/ (self.H*3.0) )
            c = xc + yc*self.NXC
            if (xc < self.NXC) and (yc < self.NYC) and (xc >= 0) and (yc >= 0):
                self.l[i] = self.h[c]
                self.h[c] = i

            

        

