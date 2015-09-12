0. Overview
----
    This is a light-weight profiler and related automatic tool.
    
    It's designed to:
    
        - cover all platforms
        
        - easy to integrate(just include a header file)
        
        - minimize the cost of profiler itself
        
        - easy to deploy automatic profiling

1. Components
----
    mini_profiler: 
        
        This is the core profiler based on timer functions. 
        
        It's a set of header files. 
        
        Check the precision analyse in precision_analys directory. 
        
        To use it, You just need to include "mini_profiler_api.h". 
        
        See mini_profiler/examples/... to check how to use.
    
    ipaf(Integrated Profiling Automatic Framework):
        
        This is a Python based tool to deploy profiling automatically. 
        
        See ipaf/example/... to check how to use. 
        
    
2. Platform Coverage
----
    mini_profiler
        
        - H/W: ARM, x86/x64
        
        - OS : Windows, Linux, OS X, Android
    
    ipaf
        
        - Host machine
            
            Any H/W or OS, which installed: Python, ssh, adb 
        
        - target device
            
            Linux, Android, Windows

3. Test Coverage
----
    Only small part of platforms has been tested for the time being. 

    mini_profiler

        - H/W: ARM, x86/x64
        
        - OS : Linux
    
    ipaf
    
        - Host machine : Linux
        
        - Target device: Linux, Android
        
4. Future works
----
    - Visualize the profiling results, probably like Valgrind. 
    
    - Improve test coverage, and deploy automatically
    
    - Implement multi-branch tree in the son-brother way, so as to improve D-Cache.  
        
        
            