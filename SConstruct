env = Environment()

env.Append(CPPPATH=['/usr/include/python3.4m'])
env.Append(LIBPATH=['/usr/lib/x86_64-linux-gnu'])
env.Append(RPATH=['/usr/lib/x86_64-linux-gnu'])
env.Append(LIBS=['boost_python-py34'])
env['LIBPREFIX'] = ''

env.Append(CPPFLAGS=['-std=c++11'])
env.Append(CPPFLAGS=['-O3'])

env.SharedLibrary('cfractal.cc')
