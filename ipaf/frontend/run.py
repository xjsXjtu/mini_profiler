#!/usr/bin/python
import os
import sys
import optparse

sys.path.append(os.path.join(os.path.dirname(__file__), "..", "src"))
from case       import CaseSet 
from device     import Device  
from device     import DeviceSet
from executor   import Executor
from analyzer   import Analyzer

def list_devices(verbose):
    all_devices = DeviceSet()
    all_devices.parse_xml(os.path.join(os.path.dirname(__file__), "..", "devices", "devices.xml"))
    print "Available Devices:"
    if verbose == 1:
        all_devices.show_id()
    elif verbose == 2:
        all_devices.show()

def list_cases(verbose):
    all_cases = CaseSet()
    all_cases.parse_xml_dir(os.path.join(os.path.dirname(__file__), "..", "cases"))
    print "Available Cases:"
    if verbose == 1:
        all_cases.show_id()
    elif verbose == 2:
        all_cases.show()
    
def main():
    # parse command line
    option = parse_cmdline()
    filtered_devices_id = option.devices
    is_list_devices = int(option.list_devices)
    filtered_cases_id = option.cases
    is_list_cases = int(option.list_cases)
    case_file = option.case_file
    executable = option.executable
    
    # check command line
    if is_list_devices:
        list_devices(is_list_devices)
        return
    if is_list_cases:
        list_cases(is_list_cases)
        return
    if not os.path.isfile(executable):
        print "Error: " + executable + " doesn't exist."
        return
 
    # parse cases 
    all_cases = CaseSet()
    if (case_file != ""):
        all_cases.parse_xml(os.path.join(os.path.dirname(__file__), case_file))
    else:
        all_cases.parse_xml_dir(os.path.join(os.path.dirname(__file__), "..", "cases"))
    filtered_case_set = {}
    if filtered_cases_id:
        for c in filtered_cases_id:
            if c in all_cases.keys():
                filtered_case_set[c] = all_cases[c]
            else:
                print "Warning: " + c + " is not an legal case ID."
    else:
        filtered_case_set = all_cases
    
    # parse devices
    all_devices = DeviceSet()
    all_devices.parse_xml(os.path.join(os.path.dirname(__file__), "..", "devices", "devices.xml"))
    filtered_device_set = {}
    for d in filtered_devices_id:
        if d in all_devices.keys():
            filtered_device_set[d] = all_devices[d]
        else:
            print "Warning: " + d + " is not an legal device ID."
    
    # execute profiling
    e = Executor(executable, filtered_device_set, filtered_case_set, duplicate=10)
    try:
        logfiles = e.run()
    except (KeyboardInterrupt):
        print "KeyboardInterrupt in run.main()"
        sys.exit()
    
    # analyze log
    for case_id in logfiles.keys():
        for device_id in logfiles[case_id].keys():
            case = all_cases[case_id]
            root_node = case.get_root_node() 
            coeff     = case.get_mcps_norm_coeff()
            interest_nodes = option.interest_nodes
            ana = Analyzer(root_node, coeff, interest_nodes)
            if len(logfiles[case_id][device_id]) > 0:
                ana.run(logfiles[case_id][device_id])
    print "Finished! \nSee profile logs here: "
    for dir in e.get_reports_dir():
        print "\t./" + dir
    

def parse_cmdline():    
    parser = optparse.OptionParser(__doc__)
    parser.add_option("-e", "--executable", dest="executable", metavar="NAME", action="store", default=[],
                      help='profile executable binary')
    parser.add_option("-d", "--devices", dest="devices", metavar="NAME", action="store", default=[],
                      help='profile device\'s ID list, seperated by \',\'')
    parser.add_option("-c", "--cases", dest="cases", metavar="NAME", action="store", default=[],
                      help='profile cases\' ID list, seperated by \',\'')
    parser.add_option("-n", "--interest-nodes", dest="interest_nodes", metavar="NAME", action="store", default=[],
                      help='interested nodes\' list, seperated by \',\'')
    parser.add_option("--ld", dest="list_devices", metavar="NAME",  action="store",  default=0,
                      help="list available devices, then exit. 0: do nothing; 1: only list id; 2: list detailed info")
    parser.add_option("--lc", dest="list_cases", metavar="NAME",  action="store",  default=0,
                      help="list available cases, then exit. 0: do nothing; 1: only list id; 2: list detailed info")
    parser.add_option("-f", "--case_file", dest="case_file", metavar="NAME",  action="store",  default="",
                      help="specify case file")
    options, args = parser.parse_args()
    if options.devices:
        options.devices = [x for x in options.devices.split(',')]
    if options.cases:
        options.cases = [x for x in options.cases.split(',')]
    if options.interest_nodes:
        options.interest_nodes = [x for x in options.interest_nodes.split(',')]
    return options
    
if __name__ == "__main__":
    main()
