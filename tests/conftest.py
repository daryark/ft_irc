import subprocess
import pytest
import time
import socket
import os
import signal
from .utils.irc_client import IRCClient

# #conftest.py is a special file pytest looks for automatically.
# Put reusable test setup there (things many tests need).

# #fixture is a function that prepares something for tests
# (example: create a Server object, create a temporary user, open a temp file).
# Tests ask for the fixture by name and pytest gives them the prepared object.
# Fixtures avoid repeating setup code in every test.

# project root = parent folder of tests/
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

BINARY_NAME = "./ircserv"   # << change if your binary has a different name
PORT = 6667                 # << choose any port your program uses
PASSWORD = "pass"           # << only if your server requires a password argument

@pytest.fixture(scope="session")
def irc_server():

    # 1. Build the project
    subprocess.run(["make", "all"], cwd=PROJECT_ROOT, check=True)

    # 2. Start the server
    server_cmd = [BINARY_NAME, str(PORT), PASSWORD]
    proc = subprocess.Popen(
        server_cmd,
        cwd=PROJECT_ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        preexec_fn=os.setsid  # to allow killing the whole process group
    )

    # 3. Wait until the server accepts connections
    deadline = time.time() + 5
    while time.time() < deadline:
        try:
            s = socket.create_connection(("127.0.0.1", PORT), timeout=0.3)
            s.close()
            break
        except Exception:
            time.sleep(0.1)
    else:
        os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
        pytest.fail("IRC server failed to start on port %d" % PORT)

    yield {
        "proc": proc,
        "host": "127.0.0.1",
        "port": PORT,
        "password": PASSWORD
    }

    # 4. Shutdown server after tests
    try:
        os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
    except Exception:
        pass
    proc.wait(timeout=2)
