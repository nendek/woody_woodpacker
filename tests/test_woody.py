import subprocess
import argparse
from color_defines import *

files_fail = []
files_segv = []
exec_name = "woody_woodpacker"
global nb_tests
global nb_success
global nb_fail
global nb_segv
nb_tests = 0
nb_success = 0
nb_fail = 0
nb_segv = 0

def launch_test(dir_name, exec_cmd):
    global nb_tests
    global nb_success
    global nb_fail
    global nb_segv
    list_files = subprocess.run(["ls", dir_name], capture_output=True).stdout.decode("utf-8").split("\n")
    woody_cmd = "./woody"
    for files in list_files:
        if files != "": 
            nb_tests += 1
            mein_out = subprocess.run([exec_cmd + " " + dir_name + files], shell=True, stdout=subprocess.PIPE)
            if mein_out.returncode != 0 and mein_out.returncode != 1:
                print("{} => {}SEGV{} ".format(files, FAIL, ENDC))
                nb_segv += 1
                files_segv.append(files)
            else:
                nb_success += 1
                print("{} => {}SUCCESS{} ".format(files, OKGREEN, ENDC))
            woody_out = subprocess.run([woody_cmd], shell=True, stdout=subprocess.PIPE)
            if woody_out.returncode != 0 and woody_out.returncode != 1:
                print("{} => {}SEGV{} ".format("woody on " + files, FAIL, ENDC))
                nb_segv += 1
                files_segv.append("woody on " + files)
            else:
                nb_success += 1
                print("{} => {}SUCCESS{} ".format("woody on " + files, OKGREEN, ENDC))
    return 

def print_results():
    global nb_tests
    global nb_success
    global nb_fail
    print("\nRESUME\n\tnb tests : {}\n\tnb success : {}{}{}\n\tnb_fail : {}{}{}\n\tnb_segv : {}{}{}".format(nb_tests, OKGREEN, nb_success, ENDC, WARNING, nb_fail, ENDC, FAIL, nb_segv, ENDC))
    with open("logs.log", "w") as f:
        for elem in files_segv:
            f.write("segv: {}\n".format(elem))
        f.write("\n\n")
        for elem in files_fail:
            f.write("fail: {}\n".format(elem))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--directory", type=str, help="optional, choose a directory")
    parser.add_argument("-a", "--all", action="store_true", help="optional, launch on all dir")
    parser.add_argument("-t", "--test", action="store_true", help="optional, launch infinitely on one dir until diff")
    parser.add_argument("-r", "--recompile", action="store_true", help="optional, Do not recompile the exec")
    args = parser.parse_args()
    exec_cmd = "./" + exec_name

    if args.recompile:
        print("compilation and copying of {}".format(exec_name))
        subprocess.run(["rm -f {}".format(exec_name)], shell=True)
        ret = subprocess.run(["cd .. && make re && cp {} tests/.".format(exec_name)], shell=True, stdout=subprocess.PIPE)
        if ret.returncode != 0:
            print("\t{}compilation FAILURE {}".format(FAIL, ENDC))
            return
        else:
            print("\t{}compilation SUCCESS {}\n".format(OKGREEN, ENDC))
    dir_name = "binary_dir/"
    if args.directory == True:
        dir_name = args.directory
    
    launch_test(dir_name, exec_cmd)
    print_results()





if __name__ == "__main__":
    main()
