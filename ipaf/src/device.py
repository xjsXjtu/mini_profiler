#!/usr/bin/python
import os
import sys
import subprocess
import xml.etree.ElementTree as ET

class AccessType:
    ssh = 1
    adb = 2
    
class Device(object):        
    def __init__(self, address, access_type, work_dir="~/ipaf_work/", pre_argv="", if_lock_cpufreq="no", locked_cpufreq_mhz=""):
    # access_type     : ssh, adb 
    # address         : account@IP or adb serial number
        self._address               = address            or ''
        self._access_typ            = access_type        or ''
        self._work_dir              = work_dir           or ''
        self._pre_argv              = pre_argv           or ''
        self._if_lock_cpufreq       = if_lock_cpufreq    or ''
        self._locked_cpufreq_mhz    = locked_cpufreq_mhz or ''
        if self._work_dir[-1] != "/":
            self._work_dir = self._work_dir + "/"
        if access_type == "ssh":
            self._access = AccessType.ssh
            self._exec    = "ssh " + address + " "
            self._cp_to   = "scp "
            self._cp_from = "scp "
        elif access_type == "adb":
            self._access = AccessType.adb
            self._exec    = "adb -s " + address + " shell "
            self._cp_to   = "adb -s " + address + " push "
            self._cp_from = "adb -s " + address + " pull "
        else:
            print "Error: access type" + access_type + " is not supported yet!"
            raise
        
    def get_work_dir(self):
        return self._work_dir
    def get_address(self):
        return self._address
    def get_pre_argv(self):
        return self._pre_argv
    
    def show(self):
        print "     address             :", self._address
        print "     type                :", self._access_typ     
        print "     work_dir            :", self._work_dir       
        print "     pre_argv            :", self._pre_argv       
        print "     if_lock_cpufrq      :", self._if_lock_cpufreq
        print "     locked_cpufreq_MHz  :", self._locked_cpufreq_mhz
    
    def _execl(self, cmd):
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)
        stdout, stderr = proc.communicate()
        return stdout, stderr
    
    def copy_to_device(self, dst, src):
        stdout = stderr = ""
        try:
            if self._access == AccessType.ssh:
                cmd = self._cp_to + src + " " + " " + self._address + ":" + dst
                stdout = "$ " + cmd
                out, stderr = self._execl(cmd.split())
                stdout += "\n" + out
            elif self._access == AccessType.adb:
                cmd = self._cp_to + " " + src + " " + dst
                stdout = "$ " + cmd
                out, stderr = self._execl(cmd.split())
                stdout += "\n" + out
            else:
                raise
        except (KeyboardInterrupt):
            print "KeyboardInterrupt in device.copy_from_device()"
            sys.exit()
        except:
            raise
        return stdout, stderr
                
    def copy_from_device(self, dst, src):
        stdout = stderr = ""
        try:
            if self._access == AccessType.ssh:
                cmd = self._cp_from + " " + self._address + ":" + src + " " + dst
                stdout = "$ " + cmd
                out, stderr = self._execl(cmd.split())
                stdout += "\n" + out
            elif self._access == AccessType.adb:
                cmd = self._cp_from + " " + src + " " + dst
                stdout = "$ " + cmd
                out, stderr = self._execl(cmd.split())
                stdout += "\n" + out
            else:
                raise
        except (KeyboardInterrupt):
            print "KeyboardInterrupt in device.copy_from_device()"
            sys.exit()
        except:
            raise
        return stdout, stderr
                
    def exec_cmd(self, cmd, sin=None, sout=None, serr=None):
        stdout = stderr = ""
        try:
            assert isinstance(cmd, str)
            cmd = self._exec + " " + cmd + " "
            stdout = "$ " + cmd
            out, stderr = self._execl(cmd.split())
            stdout += "\n" + out
        except (KeyboardInterrupt):
            print "KeyboardInterrupt in device.exec_cmd()"
            sys.exit()
        except:
            raise
        return stdout, stderr
    
    def profile_cmd(self, cmd):
        if self._locked_cpufreq_mhz:
            cmd = "export CPU_FREQ_MHZ=" + self._locked_cpufreq_mhz + " && " + cmd
        return self.exec_cmd(cmd)
        
    
class DeviceSet(dict):
    def __init__(self):
        self = {}   # dict  : name -> Device object
        
    def _append(self, name, device):
        assert name not in self.keys()
        self[name] = device
        
    def parse_xml(self, xml_file):
        root = ET.parse(xml_file).getroot()
        for d in root.findall('device'):
            name   = d.get('name')
            device = Device(
                 address            = d.find('address').text
                ,access_type        = d.find('access_type').text
                ,work_dir           = d.find('work_dir').text
                ,pre_argv           = d.find('pre_argv').text
                ,if_lock_cpufreq    = d.find('if_lock_cpufreq').text
                ,locked_cpufreq_mhz = d.find('locked_cpufreq_MHz').text
                )
            self._append(name, device)
        return self
        
    def show(self):
        for name in self.keys():
            print "#--------"
            print "#", name
            print "#--------"
            self[name].show()
    def show_id(self):
        ids = [id for id in self.keys()]
        ids.sort()
        for id in ids:
            print "    ", id
    
def test_class_Device_ssh():
    print "Testing ssh device:"
    print "==================="
    filename = "./tmp.txt"
    file = open(filename, 'w')
    subprocess.check_call("echo This is a test file from host machine.".split(), stdout = file)
    print "$ cat tmp.txt" 
    subprocess.check_call(("cat " + filename).split()) 
    d1 = Device("root@10.204.5.123", "ssh")
    d1.copy_to_device(src = filename, dst = filename)
    d1.exec_cmd("rm -rf " + filename + " && touch " + filename)
    d1.copy_from_device(src = filename, dst = "tmp2.txt")
    print "$ cat tmp2.txt" 
    subprocess.check_call("cat tmp2.txt".split())
    subprocess.check_call("rm tmp.txt tmp2.txt".split())

def test_class_Device_adb():
    print "Testing adb device:"
    print "==================="
    subprocess.call("echo 'This is a test file from host machine.' > tmp.txt")
    print "$ cat tmp.txt" 
    subprocess.call("cat tmp.txt") 
    d1 = Device("0123456789ABCDEFG", "adb")
    d1.copy_to_device(src = "tmp.txt", dst = "/data/local/tmp/tmp.txt")
    print "aa"
    d1.exec_cmd("cd /data/local/tmp/ && echo \'This is a test from device\' > tmp.txt ")
    print "bb"
    d1.copy_from_device(src = "/data/local/tmp/tmp.txt", dst = "tmp2.txt")
    print "$ cat tmp2.txt" 
    subprocess.call("cat tmp2.txt && rm -f tmp.txt tmp2.txt")
    d1.exec_cmd("jxion")

def test_parse_xml():
    dset = DeviceSet()
    dset.parse_xml(sys.argv[1])
    dset.show()
    
if __name__ == "__main__":
    pass
    test_class_Device_ssh()
    # test_class_Device_adb()
    # test_parse_xml()
    