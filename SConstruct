
import distutils.sysconfig
import os
import os.path

prefix = os.getenv("PREFIX", "/usr/local")
program_dir = os.path.join(prefix, "bin")
extension_dir = distutils.sysconfig.get_python_lib(True)

client_env = Environment()
client_env.ParseConfig("pkg-config --cflags --libs dbus-1")

extension_env = Environment(SHLIBPREFIX=None)
extension_env.ParseConfig("pkg-config --cflags --libs libxklavier")
extension_env.ParseConfig("python2.5-config --includes")
extension_env.Append(CPPPATH="/usr/X11R6/include")

client = client_env.Program("xkbswitch-dbus-client.c")
daemon = File("xkbswitch-dbus-daemon")
extension = extension_env.LoadableModule("_xkbswitch.c")
module = File("xkbswitch.py")

install_ext = extension_env.Install(extension_dir, [extension, module])
install_bin = client_env.Install(program_dir, [client, daemon])
client_env.Alias("install-bin", install_bin)
extension_env.Alias("install-ext", install_ext)
