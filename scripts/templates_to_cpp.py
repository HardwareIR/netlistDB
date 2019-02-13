#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import glob
import sys

"""
generate cpp file for strings from the templates

./templates_to_cpp.py input_0.v .. input_n.v output.cpp

"""

if __name__ == "__main__":
    assert len(sys.argv) > 2, ("at leas single input file", sys.argv)
    
    template_files = sys.argv[1:len(sys.argv) - 1]
    dst_file = sys.argv[-1]
    print(dst_file)
    with open(dst_file, 'w') as of:
        of.write('#include "serializer/verilog/verilog_templates.h"\n')
        of.write('namespace netlistDB { namespace serializer { namespace verilog {\n')
        for file_name in template_files:
            of.write('std::string template_' + os.path.basename(file_name).split(".")[0] + "=")
            with open(file_name) as f:
                for line in f:
                    line = line.replace("\n", "")
                    of.write('    "%s\\n"\n' % line)
            of.write(";\n")
            
        of.write("}}}\n")