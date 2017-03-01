
import os

Import('envGlobal')

envGlobal.Append(CPPPATH=[os.path.abspath('.')])

env = envGlobal.Clone()

files  = env.Glob('*.cpp')

library = env.StaticLibrary('simu', files)
envGlobal.Install('$BUILDPATH/lib', library)
