#!/usr/bin/python
import os
import sys
import numpy as np
import matplotlib.pyplot as plt

def draw_one_node(line_ref, line_dut, figure_num):
    ref = line_ref.strip().replace(',', ' ').split()
    dut = line_dut.strip().replace(',', ' ').split()
    assert(ref[0] == dut[0])
    node_name = ref[0]
    ref_min = [float(x) for x in ref[1::2]]
    ref_avg = [float(x) for x in ref[2::2]]
    dut_min = [float(x) for x in dut[1::2]]
    dut_avg = [float(x) for x in dut[2::2]]
    
    print node_name + ":"
    print "ref,\t",
    print "%.2f,\t" %(np.min (np.array(ref_avg))),
    print "%.2f,\t" %(np.mean(np.array(ref_avg))),
    print "%.3f,\t" %(np.std (np.array(ref_avg))),
    print "%.2f,\t" %(np.mean(np.array(ref_min))),
    print "%.3f,\t" %(np.std (np.array(ref_min)))
    print "dut,\t",
    print "%.2f,\t" %(np.min (np.array(dut_avg))),
    print "%.2f,\t" %(np.mean(np.array(dut_avg))),
    print "%.3f,\t" %(np.std (np.array(dut_avg))),
    print "%.2f,\t" %(np.mean(np.array(dut_min))),
    print "%.3f,\t" %(np.std (np.array(dut_min)))    
    
    plt.figure(figure_num)
    plt.plot(ref_min, ref_avg, 'r*')
    plt.plot(dut_min, dut_avg, 'b+')
    plt.axis([-0.1 + min(min(ref_min), min(dut_min)),
               0.1 + max(max(ref_min), max(dut_min)),
              -0.1 + min(min(ref_avg), min(dut_avg)),
               0.1 + max(max(ref_avg), max(dut_avg))])
    plt.title(node_name + " (ref->RED, dut->BLUE)")
    plt.xlabel('min')
    plt.ylabel('avg')
    plt.show()
    
def main():
    pfile = open("summary_ref_dut.log")
    figure_num = 1
    print "ref/dut,\t avg's_min,\t avg's_avg,\t avg's_std,\t min's_avg,\t min's_std"
    try:
        lines = pfile.readlines()
        line_iter = iter(lines)
        line_iter.next() # skip first line
        for line in line_iter:
            if '==>' in line:
                return
            draw_one_node(line, line_iter.next(), figure_num)
            figure_num += 1
        plt.show() 
    finally:
       pfile.close()

if __name__ == "__main__":
    main()