import os
import signal
import subprocess

def find_and_kill_processes_using_port(port):
    command = f"lsof -ti:{port}"
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
    output, _ = process.communicate()
    pids = output.decode().splitlines()
    for pid in pids:
        try:
            os.kill(int(pid), signal.SIGKILL)
        except ValueError:
            continue
        except ProcessLookupError:
            print(f"Процесс с PID {pid} уже завершен.")

if __name__ == "__main__":
    find_and_kill_processes_using_port(5000)