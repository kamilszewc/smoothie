#!/usr/bin/env python
# -*- coding: utf-8 -*-
# All right reserved by Kamil Szewc, Gdansk/Nancy 2009 

from sys import argv,exit
from numpy import *
import numpy
from pylab import *
from PostProc.Data import Data
from PostProc.Field import Field

if __name__ == '__main__':

    d = Data(argv[1])

    x1, y1 = [], []
    x2, y2 = [], [] 
    u1, v1 = [], []
    u2, v2 = [], []
    for i in range(d.N):
        if (d.c[i] == 1):
            x1.append(d.x[i])
            y1.append(d.y[i])
            u1.append(d.u[i])
            v1.append(d.v[i])
        if (d.c[i] == 0):
            x2.append(d.x[i])
            y2.append(d.y[i])
            u2.append(d.u[i])
            v2.append(d.v[i])

    r = Field(d)

    extent = (0,d.XCV,0,d.YCV)
    figsize = (6, 6*d.YCV/d.XCV)

    figure()
    imshow(r.u, origin='lower', extent=extent)
    colorbar()
    title("U speed")

    figure()
    imshow(r.v, origin='lower', extent=extent)
    colorbar()
    title("V speed")

    figure()
    imshow(sqrt(r.v**2 + r.u**2), origin='lower', extent=extent)
    colorbar()
    title("sqrt(v**2 + u**2)")

    figure()
    imshow(r.p, origin='lower', extent=extent)
    colorbar()
    title("P pressure")

    figure()
    imshow(r.d, cmap=cm.autumn, origin='lower', extent=extent)
    colorbar()
    title("D density")

    figure()
    imshow(r.tp, cmap=cm.autumn, origin='lower', extent=extent)
    colorbar()
    title("Temperature")

    figure(figsize=figsize)
    xlim(0,d.XCV)
    ylim(0,d.YCV)
    plot(x1, y1, 'co', markersize=3.1, markeredgecolor='w')
    plot(x2, y2, 'ko', markersize=3.1, markeredgecolor='w') 
    xlabel('x')
    ylabel('y')
    savefig('dist.pdf', format='pdf')

    figure(figsize=figsize)
    quiver(d.x, d.y, d.u, d.v)
    title("Velocity")

    show()
