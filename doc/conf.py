import subprocess
subprocess.call('doxygen .doxygen', shell=True)

html_extra_path = ['build/html']
