import os
import glob


thispath: str = os.path.dirname(os.path.abspath(__file__))

ngenpath: str= r"C:\Windows\Microsoft.NET\Framework\v4.0.30319\ngen.exe"

for targetfile in glob.glob(thispath + r"\hm*\*.dll"):
    targetdir: str = os.path.dirname(os.path.abspath(targetfile))
    os.chdir(targetdir)

    cmd: str = r'{} install "{}" /AppBase:"{}"'.format(ngenpath, targetfile, thispath)
    print(cmd)
    os.system(cmd)
