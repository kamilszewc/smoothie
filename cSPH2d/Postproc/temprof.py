#!/usr/bin/env python
from sys import argv
from numpy import *
from scipy.special import erf, erfc
from matplotlib.pyplot import *
from PostProc.Data import Data
from PostProc.Field import XField

Case = 1
Field = 1

def case1(x,t):
    a = 0.001
    if x < 0.5:
        return 0.5*(erfc(0.5*(0.5-x)/sqrt(a*t)))
    if x > 0.5:
        return 0.5*(1.0 + erf(0.5*(x-0.5)/sqrt(a*t)))
    if x == 0.5:
        return 0.5

def case2(x,t):
    return sin(pi*x)*exp(-pi*pi*0.1*t)

def case3(x,t):
    kl = 1.0; kr = 3.0
    cl = 1.0; cr = 1.5
    dl = 1000.0; dr = 2000
    al = kl/(cl*dl); ar = kr/(cr*dr)
    Tc = (kr/sqrt(ar))/(kr/sqrt(ar) + kl/sqrt(al))
    if x < 0.5:
        return Tc*(erfc(0.5*(0.5-x)/sqrt(al*t)))
    if x > 0.5:
        return Tc*(1.0 + (kl/kr)*sqrt(ar/al)*erf(0.5*(x-0.5)/sqrt(ar*t)))
    if x == 0.5:
        return Tc

if __name__ == '__main__':
    fig = figure(figsize=(5.6,5.6))
    plt = fig.add_subplot(111)
    n = 0
    for arg in argv[1:]:
        d = Data(arg)
        x = linspace(0.001, 0.999, d.NX)
        xx = linspace(0.0001, 0.9999, d.NX*10) 
        ta = []
        for point in xx:
            if Case == 1: ta.append(case1(point, d.t))
            if Case == 2: ta.append(case2(point, d.t))
            if Case == 3: ta.append(case3(point, d.t))
        if n == 0: plt.plot(xx, ta, '-k', linewidth=2.0, label='Analytical')
        else: plt.plot(xx, ta, '-k', linewidth=2.0)
        if Field == 0:
            rx = XField(x, 0.5, d)
            plt.plot(rx.x, rx.tp, 'ow', markersize=5.5, markeredgewidth=1.0)
        if Field == 1:
            rx = []; rt = []
            for x, y, tp in zip(d.x, d.y, d.tp):
                if 0.45 < y <0.52:
                    rx.append(x)
                    rt.append(tp)
            if n == 0: plt.plot(rx, rt, 'ow', markersize=5.5, markeredgewidth=1.0, label='SPH')
            else: plt.plot(rx, rt, 'ow', markersize=5.5, markeredgewidth=1.0)
            
        
        if Case == 1: plt.text(0.75, 0.07, "$t^*=\,%s$" % (d.t*0.001), fontsize='large')
        if Case == 2: plt.text(0.5-0.08, case2(0.5, d.t)-0.069, "$t^*=\,%s$" % (d.t), fontsize='large')
        if Case == 3: plt.text(0.65, 0.07, "$t^*=\,%s$" % (d.t*0.001), fontsize='x-large')
        n += 1

plt.set_xlim(-0.02,1.02)
#plt.set_ylim(-0.02,1.02)
plt.set_xlabel(r"$x^*$", fontsize='x-large')
plt.set_ylabel(r"$T^*$", fontsize='x-large')
#plt.legend(loc=2)
fig.savefig("temprof.pdf", format='pdf')
show()
