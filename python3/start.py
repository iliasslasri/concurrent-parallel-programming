import _thread
import time

def start():
    print("Start")
    _thread.start_new_thread(print_time, ("Thread-1", 2,))

def print_time(threadName, delay):
    count = 0
    while count < 5:
        time.sleep(delay)
        count += 1
        print("%s: %s" % (threadName, time.ctime(time.time())))



if __name__ == "__main__":
    start()
    while True:
        pass

```
$python3 start.py
Start
Thread-1: Sun Sep 15 20:36:22 2024
Thread-1: Sun Sep 15 20:36:24 2024
Thread-1: Sun Sep 15 20:36:26 2024
Thread-1: Sun Sep 15 20:36:28 2024
Thread-1: Sun Sep 15 20:36:30 2024
```