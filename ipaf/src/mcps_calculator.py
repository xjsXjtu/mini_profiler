#!/usr/bin/python
import sys
import optparse

class McpsCalculator(object):
    def __init__(self, root, coff):
        self._root              = root          # string    : root node name 
        self._root_total_cycles = -1            # int
        self._root_calls        = -1            # int       : number of calls for root node
        self._coff              = float(coff)   # float
        
    def read_n_lines(self, file, n):
        lines = []
        for i in range(0, n):
            lines.append(file.readline().strip())
        return lines  
    
    def _cal_mcps_one_node(self, lines):
        pro_lines = lines[0:2]
        call_num = 5
        for n in range(2,5):
            cycl_words      = lines[n].strip().split()
            call_words      = lines[call_num].strip().split()
            cycls           = float(cycl_words[-2])
            calls           = int(call_words[-1])
            cycle_all       = cycls * calls
            percent         = 1.0 * cycls * calls / self._root_total_cycles
            mcps            = percent * self._root_total_cycles / self._root_calls  * self._coff
            pro_lines.append(lines[n] + " \t===MCPS: %2.1f, %2.1f%%===" %(mcps, percent * 100))
        pro_lines = pro_lines + lines[5:7]
        return pro_lines
    
    def _print_lines(self, file, lines):
        for line in lines:
            print >> file, line
            
    def process(self, infile, outfile):
        file = open(infile)
        outf = open(outfile, 'w')
        try:
            self._print_lines(outf, ["coff = " + str(self._coff)])
            # root node
            while 1:
                line_nodename = file.readline()
                if line_nodename == "": 
                    self._print_lines(outf, ["Error: " + self._root + " is not in the profile log."])
                    break
                line_nodename = line_nodename.strip()
                words = line_nodename.split()
                if self._root in words:
                    lines = [line_nodename] + self.read_n_lines(file, 6)
                    self._root_total_cycles = int(lines[1].strip().split()[-2])
                    self._root_calls        = int(lines[5].strip().split()[-1])
                    self._print_lines(outf, self._cal_mcps_one_node(lines))
                    break
                else:
                    self._print_lines(outf, [line_nodename])
                    
            # other nodes
            while 1:
                lines = self.read_n_lines(file, 7)
                if not lines[-1]:
                    break
                self._print_lines(outf, self._cal_mcps_one_node(lines))
        finally:
           file.close()
           outf.close()

def parse_cmdline():    
    parser = optparse.OptionParser(__doc__)
    parser.add_option("-f", "--file", dest="file", metavar="NAME", action="store", default=[],
                      help='profile report file name.')
    parser.add_option("-r", "--root", dest="root", metavar="NAME", action="store", default="",
                      help='root node name.')
    parser.add_option("-c", "--coff", dest="coff", metavar="NAME", action="store", default=[],
                      help='coff to convert to MCPS.')
    parser.add_option("-o", "--output", dest="output", metavar="NAME", action="store", default="out.log",
                      help='output file name')
    options, args = parser.parse_args()
    return options
    
if __name__ == "__main__":
    option = parse_cmdline()
    p = McpsCalculator(option.root, option.coff)
    p.process(option.file, option.output)
                