#Simu
import os

print("SIMU")
Import('envGlobal')

envGlobal.Append(CPPPATH=[os.path.abspath('.')])

env = envGlobal.Clone()

envGlobal.Append(LIBS=[
	'pthread'
])

files  = env.Glob('*.cpp')

library = env.StaticLibrary('simu', files)
envGlobal.Install('$BUILDPATH/lib', library)
