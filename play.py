#===================================================================
#    FileName: play_audio.py
#      Author: Yin Mingming
#       Email: ymingming@gmail.com
#     WebSite: http://www.????.com
#  CreateTime: 2010.01.01
#===================================================================
import time
import numpy as np
import pyaudio

import guiqwt.pyplot as plt 
def test_sig(fs,fc):
    pa = pyaudio.PyAudio()
    
    frames = 16384 
    t = np.arange(frames)/float(fs) 
    s = np.cos(2.0*np.pi*fc*t).astype(np.float32)

    stream = pa.open(format=pyaudio.paFloat32,channels = 1,rate =int(fs),output = True,frames_per_buffer=frames)

    #mv = np.max(np.abs(s))
    #s = s/mv
    #print len(s),s.mean(),s.max(),s.min()        
    data = s.tostring()
    idx=1
    while True:
        print idx
        stream.write(data) 
        idx += 1

    stream.close() 
    pa.terminate()


def main(fs,filename,cnt,dt):
    pa = pyaudio.PyAudio()

    s = np.loadtxt(filename).astype(np.float32)
    print len(s),s.mean(),s.max(),s.min()        

    frames = len(s)
    stream = pa.open(format=pyaudio.paFloat32,channels = 1,rate =int(fs),output = True,frames_per_buffer=frames)

    mv = np.max(np.abs(s))
    s = s/mv
    
    data = np.zeros(frames,dtype=np.float32)
    data[:len(s)] = s[:]

    data /= np.max(np.abs(data)) 
    data = data.tostring()

    for k in xrange(cnt):
        print k+1,'of',cnt
        stream.write(data) 
        time.sleep(dt) 

    stream.close() 
    pa.terminate()

if __name__ == '__main__':
    fs = 44100
    main(fs,"../dsp/m_code_1_2_3_4_5_6.txt",10000,0.1)
    #main(fs,"../dsp/m_code_1.txt",10000,0.1)
    #test_sig(fs,18000)

