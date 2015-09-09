#!/usr/bin/python
import os
import sys
from datetime import datetime

from case   import CaseSet 
from device import Device  

class Executor(object):
    def __init__(self, exe_fullpath, devices, cases, duplicate=2):
        self._exe_abspath   = os.path.abspath(exe_fullpath) # string        : executable binary including path and filename
        self._exe_name      = os.path.basename(exe_fullpath)# string        : executable file name
        self._devices       = devices                       # list          : device id list
        self._cases         = cases                         # CaseSet
        self._duplicate     = duplicate                     # int           : run every case for 3 times
        self._reports_dir   = []                            # string list   : one device, one reports log directory 
        
    def _prepare(self, device):
        """
        finish one-time things on device before profile
        """
        remote_dir = device.get_work_dir()
        device.exec_cmd("rm -rf " + remote_dir + " && mkdir -p " + remote_dir)
        device.copy_to_device(src = self._exe_abspath, dst = remote_dir)
        device.exec_cmd("chmod 777 " + os.path.join(remote_dir, self._exe_name))
        
        
    def _write_stdout_stderr(self, reports_dir, stdout, stderr):
        with open(os.path.join(reports_dir, "stdout.txt"), "a") as fout:
            with open(os.path.join(reports_dir, "stderr.txt"), "a") as ferr:
                fout.write(stdout)
                ferr.write(stderr)
    
    def get_reports_dir(self):
        return self._reports_dir
        
    def run(self):
        """
        RUN:
            one executable
            several cases
            several devices
        return:
            outfiles, which is a dict {case_id -> {device_id -> log file }} 
        """
        outfiles = {}       # dict  : {case_id -> {device_id -> log file }} 
        for device_id in self._devices:
            device = self._devices[device_id]
            remote_dir = device.get_work_dir()
            reports_dir   = "profile_reports_"                                       \
                      + device_id \
                      + "_"                                                          \
                      + self._exe_name                                               \
                      + "_"                                                          \
                      + datetime.now().strftime('%Y%m%d-%H-%M-%S')                   \
                      + "/"
            os.mkdir(reports_dir)
            self._reports_dir.append(reports_dir)
            
            case_num = len(self._cases)
            cur_case = 1
            for case_id in self._cases:
                self._prepare(device)
                case = self._cases[case_id]
                print str(cur_case) + "/" + str(case_num), case_id
                cur_case += 1
                
                stdout = stderr = ""
                stdout += "\n# ======================="
                stdout += "\n# Executor: " + self._exe_abspath
                stdout += "\n# Case    : " + case_id
                stdout += "\n# Device  : " + device.get_address()
                stdout += "\n# ======================="
                # copy signals to device
                for signal in case.get_input_signals():
                    out, err = device.copy_to_device(src = signal, dst=remote_dir)
                    stdout += "\n\t" + out
                    stderr += "\n\t" + err
                # execute and copy profile_report
                if case_id not in outfiles.keys():
                    outfiles[case_id] = {}
                outfiles[case_id][device_id] = []
                try:
                    for k in range(self._duplicate):
                        stdout += "\ntime " + str(k) + " ...\n"
                        print "\ttime ", k, "..."
                        
                        sout, serr = device.profile_cmd("cd " + remote_dir + " && " + device.get_pre_argv() + " ./" + self._exe_name + " " + case.get_cmdline())
                        stdout += "\t" + sout.replace('\n', '\n\t')
                        stderr += "\t" + serr.replace('\n', '\n\t')
                        
                        outfile = os.path.join(reports_dir, case_id + "_" + str(k) + ".log")
                        sout, serr = device.copy_from_device(src = os.path.join(remote_dir, "profile_report.log"), dst=outfile)
                        stdout += sout.replace('\n', '\n\t')
                        stderr += serr.replace('\n', '\n\t')
                        if serr == "":
                            outfiles[case_id][device_id].append(outfile)
                except (KeyboardInterrupt):
                    print "KeyboardInterrupt in executor.Executor.run()"
                    sys.exit()
                except:
                    raise
                
                sout, serr = device.exec_cmd("rm -rf " + remote_dir)
                stdout += sout.replace('\n', '\n\t')
                stderr += serr.replace('\n', '\n\t')
                self._write_stdout_stderr(reports_dir, stdout, stderr)
                
        return outfiles
                
if __name__ == "__main__":
    cases = CaseSet()
    cases.parse_xml("x.xml")
    d     = Device("root@10.204.5.123", "ssh")
    e = Executor("ddp_udc_profiling_armv7int_neon_dlb_profile", d, cases)
    e.run()
            
