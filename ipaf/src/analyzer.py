#!/usr/bin/python
import os
import sys
import optparse

from mcps_calculator  import McpsCalculator

class Analyzer(object):
    def __init__(self, root, coeff, interest_nodes=[]):
        self._root            = root
        self._mcps_calculator = McpsCalculator(root, coeff)                             # McpsCalculator    : base analyzer to calculate MCPS
        self._nodes           = interest_nodes                                          # string list       : nodes interested
        
    def find_avg_of_one_node(self, node, file):
        f = open(file)
        lines = f.readlines()
        avg_line = ""
        for i in range(0, len(lines)):
            line = lines[i]
            if node in line:
                avg_line = lines[i+4]
                break
        f.close()
        if(avg_line == ""):
            return 0.0
        return float(avg_line.strip().replace(',', ' ').split()[-2])
        
    def find_smallest_root_avg(self, files):
        small_file = ""
        small_avg  = 1e10
        for file in files:
            cur_avg = self.find_avg_of_one_node(self._root, file)
            if cur_avg < small_avg:
                small_avg = cur_avg
                small_file = file
        return small_file
        
    def run(self, in_files):
        # calculate mcps in place
        for src_file in in_files:
            raw_mcps_file = src_file + ".mcps"
            self._mcps_calculator.process(src_file, raw_mcps_file)
            os.remove(src_file)
            os.rename(raw_mcps_file, src_file)
        smallest_file = self.find_smallest_root_avg(in_files)
        
        outfile       = smallest_file + ".summary.csv"
        outf = open(outfile, 'w')
        print >> outf, "ATTENTION: all the following nodes should not be another one's child node."
        print >> outf, "node" + "\t" + "MCPS"
        root_mcps = self.find_avg_of_one_node(self._root, smallest_file)
        print >> outf, self._root + ", %2.1f" %(root_mcps)
        for node in self._nodes:
            node_mcps = self.find_avg_of_one_node(node, smallest_file)
            root_mcps -= node_mcps
            print >> outf, node + ", %2.1f" %(node_mcps)
        print >> outf, "others" + ", %2.1f" %(root_mcps)
        outf.close()        
        
def parse_cmdline():    
    parser = optparse.OptionParser(__doc__)
    parser.add_option("-f", "--files", dest="files", metavar="NAME", action="store", default=[],
                      help='profile report files, seperated by \',\'')
    parser.add_option("-r", "--root", dest="root", metavar="NAME", action="store", default="",
                      help='root node name.')
    parser.add_option("-n", "--interest-nodes", dest="interest_nodes", metavar="NAME", action="store", default=[],
                      help='interested nodes\' list, seperated by \',\'')
    parser.add_option("-c", "--coff", dest="coff", metavar="NAME", action="store", default=[],
                      help='coff to convert to MCPS.')
    parser.add_option("-o", "--output", dest="output", metavar="NAME", action="store", default="out.log",
                      help='output file name')
    options, args = parser.parse_args()
    if options.interest_nodes:
        options.interest_nodes = [x for x in options.interest_nodes.split(',')]
    if options.files:
        options.files = [x for x in options.files.split(',')]
    return options
    
if __name__ == "__main__":
    option = parse_cmdline()
    ana = Analyzer(option.root, option.coff, option.interest_nodes)
    ana.run(option.files)
    print "Finished"
        