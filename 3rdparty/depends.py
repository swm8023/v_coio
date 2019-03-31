import yaml
import sys
import os
import shutil
import platform
import atexit
import subprocess

SOURCE_DIR = "source"
BUILD_DIR = "build"

# read config file (depends.yaml)
config = None
with open("depends.yaml") as f:
    try:
        config = yaml.load(f)
    except yaml.YAMLError as e:
        print e

assert config is not None
print config

# log.txt
log = open("log.txt", "w", 0)
@atexit.register
def close_log():
    log.flush()
    log.close()

class Logger(object):
    def __init__(self):
        # backup old stdout
        self.terminal = sys.stdout

    def write(self, messgae):
        self.terminal.write(messgae)
        log and log.write(messgae)

logger = Logger()
sys.stdout = logger
sys.stderr = logger

# check os
os_name = platform.system().lower()
BUILD_OS = [v for k, v in {"win": "win", "darwin": "osx", "linux" : "linux"}.items() if k in os_name][0]
print "Build OS: ", BUILD_OS


def create_dir(dir_name):
    if not os.path.exists(dir_name):
        os.mkdir(dir_name)

def remove_dir(dir_name):
    if os.path.exists(dir_name):
        shutil.rmtree(dir_name)

def create_empty_dir(dir_name):
    remove_dir(dir_name)
    create_dir(dir_name)

# get real path
SOURCE_DIR = os.path.realpath(config.get("source", "source"))
BUILD_DIR = os.path.realpath(config.get("build", "build"))
print 'Source Dir: ', SOURCE_DIR
print 'Build Dir:', BUILD_DIR

create_dir(SOURCE_DIR)
create_dir(BUILD_DIR)


def run_cmd(cmd):
    print '-- run -- ', cmd
    poll_code = None
    try:
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, bufsize=0)
        while True:
            output = p.stdout.readline()
            if output:
                print output,
            poll_code = p.poll()
            if poll_code is not None:
                break
    except Exception, e:
        p.terminate()
        print e
    finally:
        # flush, counldn't block here
        p.stdout.flush()
        output =  p.stdout.read()
        if output:
            print output,
    assert poll_code == 0, 'run cmd failed, code %s!'%poll_code
    

class Builder(object):
    def __init__(self, target=None):
        self.ninja = None

    def build_ninja(self):
        source_dir = os.path.join(SOURCE_DIR, "ninja")
        build_dir = os.path.join(BUILD_DIR, "ninja")

        suffix = ".exe" if BUILD_OS == "win" else ""
        self.ninja = os.path.join(build_dir, "ninja%s"%(suffix, ))
        if os.path.exists(self.ninja):
            print '================== Ninja Alredy Build, Skipped... =================='
            return
        
        create_empty_dir(build_dir)

        cmd = "cd %s && python %s/configure.py --bootstrap"%(build_dir, source_dir)
        run_cmd(cmd)

    def clean(self):
        remove_dir(BUILD_DIR)

    def build_one_lib(self, lib_name, cmake_dir, cmake_args=None):
        source_dir = os.path.join(SOURCE_DIR, lib_name)
        build_dir = os.path.join(BUILD_DIR, lib_name)
        install_dir = os.path.realpath(config.get("install", "install"))

        create_empty_dir(build_dir)

        now_path = os.path.realpath(".")
        os.chdir(build_dir)
        
        print '================== BUILD %s BEGIN =================='%(lib_name.upper(), )
        final_cmake_args = []
        final_cmake_args.extend(config.get("cmake_args", []))

        if cmake_args:
            final_cmake_args.extend(cmake_args)
        final_cmake_args.append("-DCMAKE_INSTALL_PREFIX=%s"%(install_dir,))
        final_cmake_args.append("-DCMAKE_MAKE_PROGRAM=%s"%self.ninja)
        
        cmake_arg_str = " ".join(final_cmake_args)
        cmd = " ".join(["cmake -G Ninja", cmake_arg_str, "%s/%s"%(source_dir, cmake_dir)])
        
        run_cmd(cmd)
        run_cmd(self.ninja)
        run_cmd(self.ninja + " install")
        print '================== BUILD %s END =================='%(lib_name.upper(), )

        os.chdir(now_path)


    def build_libs(self):
        for name, attr in config["depends"].items():
            if name == "ninja":
                continue
            self.build_one_lib(name, "", attr.get('cmake_args'))


    def download_libs(self):
        for name, attr in config["depends"].items():
            git_url = attr.get("git", None)
            if git_url:
                source_path = os.path.join(SOURCE_DIR, name)
                if not os.path.exists(source_path):
                    run_cmd("cd %s && git clone %s %s"%(SOURCE_DIR, git_url, name))
                    if attr.get('tag'):
                        run_cmd("cd %s && git checkout %s"%(source_path, attr.get("tag")))


    def work(self):
        self.download_libs()
        self.build_ninja()
        self.build_libs()


if __name__ == "__main__":
    if len(sys.argv) > 1:
        if sys.argv[1] == "clean":
            Builder().clean()
            exit(0)


    Builder().work()
