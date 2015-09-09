#!/usr/bin/python
import os
import sys

import xml.etree.ElementTree as ET

class Case(object):
    def __init__(self, input_signals, cmdline, root_node, mcps_norm_coeff, description=""):
        self._input_signals     = input_signals       # string list : input signals which will be copied to remote device
        self._cmdline           = cmdline             # string      : cmdline that will execute on remote device
        self._root_node         = root_node           # string      : root node name in profile report
        self._mcps_norm_coeff   = mcps_norm_coeff     # float       : convert profile log to MCPS value
        self._description       = description         # string
    def get_input_signals(self):
        return self._input_signals
    def get_cmdline(self):
        return self._cmdline
    def get_root_node(self):
        return self._root_node
    def get_mcps_norm_coeff(self):
        return self._mcps_norm_coeff
    
    
    def show(self):
        print "     input_signals       :", self._input_signals   
        print "     cmdline             :", self._cmdline         
        print "     root_node           :", self._root_node       
        print "     samplerate          :", self._samplerate      
        print "     samples_per_func    :", self._samples_per_func
        print "     description         :", self._description     

        
class CaseSet(dict):
    def __init__(self):
        self = {}   # dict  : id -> Device object
        
    def _append(self, id, case):
        if id in self.keys():
            print id + " already exist."
            assert 0
        self[id] = case
    
    def parse_xml(self, xml_file):
        root = ET.parse(xml_file).getroot()
        for c in root.findall('case'):
            case_id   = c.get('id')
            theinputs = c.find('inputs')
            theinput = theinputs.findall('input')
            string_input_list = []
            for eachinput in theinput:
                abspath = os.path.join(os.path.dirname(__file__), '..', 'cases')
                for i in eachinput.text.split('/'):
                    abspath = os.path.join(abspath, i)
                string_input_list.append(abspath)
            case = Case(
                 input_signals    = string_input_list
                ,cmdline          = c.find('cmdline').text
                ,root_node        = c.find('root_node').text
                ,mcps_norm_coeff  = eval('1.0 * ' + c.find('mcps_norm_coeff').text)
                ,description      = c.find('description').text
                )
            self._append(case_id, case)
        return self
    
    def parse_xml_dir(self, xml_dir):
        for subdir, dirs, files in os.walk(xml_dir):
            for file in files:
                if file[-4:] == '.xml':
                    self.parse_xml(os.path.join(xml_dir, file))
        
    def show(self):
        for id in self.keys():
            print "#--------"
            print "#", id
            print "#--------"
            self[id].show()
    def show_id(self):
        ids = [id for id in self.keys()]
        ids.sort()
        for id in ids:
            print "    ", id
    
if __name__ == "__main__":
    cases = CaseSet()
    cases.parse_xml_dir(sys.argv[1])
    cases.show()