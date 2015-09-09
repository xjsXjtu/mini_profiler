#!/usr/bin/python
import os
import sys
import optparse

'''
    Get avg&min MCPSs of nodes from all profile logs of a specific directory.
'''

def get_logfile_list(dir):
    '''
    Return a list containing all *.log file names.
    '''
    assert(os.path.exists(dir) and os.path.isdir(dir))
    files = []
    for file in os.listdir(dir):
        if file.endswith(".log"):
            files.append(os.path.join(dir, file))
    return files

def calculate_mcps(line, coeff):
    words = line.strip().replace(',', ' ').replace(':', ' ').replace('=', ' ').split()
    if "MCPS" not in words:
        mcps = float(words[-2]) * coeff
    else:
        mcps = float(words[-2])
    return mcps

def get_one_node_one_logfile(node, file, coeff):
    '''
    Return node's MCPS in log file.
        -1    : cannot find the node
        others: mcps value
    '''
    pfile = open(file)
    try:
        bfind = False
        lines = pfile.readlines()
        line_iter = iter(lines)
        for line in line_iter:
            line = line.strip()
            words = line.split()
            if node in words:
                bfind = True
                break
        if bfind:
            line = line_iter.next()
            line = line_iter.next()
            line = line_iter.next()
            min_mcps = str(calculate_mcps(line, coeff))
            line = line_iter.next()
            avg_mcps = str(calculate_mcps(line, coeff))
        else:
            min_mcps = avg_mcps = "N/A"
        return min_mcps, avg_mcps
    finally:
       pfile.close()
    

def get_one_node_logfiles(node, files, coeff):
    res = {}
    for file in files:
        mcps = get_one_node_one_logfile(node, file, coeff)
        res[file] = mcps
    return res

def get_nodes_logfiles(nodes, files, coeff):
    res = {}
    for node in nodes:
        node_res = get_one_node_logfiles(node, files, coeff)
        res[node] = node_res
    return res

def print_result(res, file):
    '''
    Input:
        res: is a dict like this:
            {'node_name1': {"logfile_name1": (min_mcps1, avg_mcps1), "logfile_name2": (min_mcps2, avg_mcps2), ...},
             'node_name2': {"logfile_name1": (min_mcps1, avg_mcps1), "logfile_name2": (min_mcps2, avg_mcps2), ...}
            }
        file: output file name
    '''
    nodes =  res.keys(); nodes.sort()
    if len(nodes) < 1:
        return
    logs = res[nodes[0]].keys(); logs.sort()
    if len(logs) < 1:
        return
    simple_logs = ["log_" + str(i) for i in range(len(logs))]
        
    pfile = open(file, 'w')
    try:
        print >> pfile, "node_name,\t ",
        for i in range(len(logs)):
            print >> pfile, simple_logs[i] + ":min,\t", simple_logs[i] + ":avg,\t",
        print >> pfile,  ""       
        
        for node in nodes:
            print >> pfile, node + ',\t', 
            for log in logs:
                print >> pfile, res[node][log][0] + ',\t', res[node][log][1] + ',\t',
            print >> pfile,  ""
        for i in range(len(logs)):
            print >> pfile, simple_logs[i], "==>", logs[i]
    finally:
       pfile.close()

def parse_cmdline():   
    '''
    specify command options.
    '''
    parser = optparse.OptionParser(__doc__)
    parser.add_option("-d", "--dir", dest="dir", metavar="NAME", action="store", default=[],
                      help='directory where profile logs is')
    parser.add_option("-n", "--interest-nodes", dest="interest_nodes", metavar="NAME", action="store", default=[],
                      help='interested nodes\' list, seperated by \',\'')
    parser.add_option("-c", "--coeff", dest="coeff", metavar="NAME", action="store", default="1.0",
                      help='[optional] coeff to convert cycles to mcps, default: 1.0')
    parser.add_option("-o", "--output", dest="out", metavar="NAME", action="store", default=[],
                      help='output file')
    options, args = parser.parse_args()
    if options.interest_nodes:
        options.interest_nodes = [x for x in options.interest_nodes.split(',')]
    return options


def main():
    # parse command line
    option = parse_cmdline()
    files = get_logfile_list(option.dir)
    nodes = option.interest_nodes
    coeff = float(option.coeff)
    out   = option.out
    res   = get_nodes_logfiles(nodes, files, coeff)
    print_result(res, out)    
    
if __name__ == "__main__":
    main()
