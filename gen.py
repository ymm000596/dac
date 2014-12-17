import numpy as np
import guiqwt.pyplot as plt

def c2():
    fc = 2000.0
    fs = 44100.0
    ns = 1024*4

    t = np.arange(ns)/float(fs)

    sl = 16384.0*np.sin(2.0*np.pi*fc*t)
    sl = sl.astype(np.int16)
    sl[:ns/4] = 0

    sr = 16384.0*np.sin(2.0*np.pi*fc*t)
    sr = sr.astype(np.int16)
    sr[ns/4:] = 0 

#    plt.plot(t,sl)
    #plt.plot(t,sr)
    #plt.show()

    fp = open('c2.txt','w')
    for k in xrange(ns):
        buf = ("%d,%d\n")%(sl[k],sr[k])
        fp.write(buf)

def c1():
    fc = 1000.0
    fs = 32000.0
    ns = 1024

    t = np.arange(ns)/float(fs)

    sl = 16384.0*np.sin(2.0*np.pi*fc*t)
    sl = sl.astype(np.int16)


    plt.plot(t,sl)
    plt.show()

    fp = open('c1.txt','w')
    for k in xrange(ns):
        buf = ("%d\n")%(sl[k])
        fp.write(buf)

if __name__ == '__main__':
    c2()
