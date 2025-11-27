import socket

class IRCClient:
    def __init__(self, host, port):
        self.sock = socket.create_connection((host, port))
        self.sock_file = self.sock.makefile("r", encoding="utf-8", newline="\r\n")

    def send_line(self, line: str):
        self.sock.sendall((line + "\r\n").encode("utf-8"))

    def recv_line(self, timeout=1.0):
        """Read a line with a timeout so tests never hang forever."""
        self.sock.settimeout(timeout)
        try:
            line = self.sock_file.readline()
            if not line:
                raise TimeoutError("Socket closed or no data received")
            return line.rstrip("\r\n")
        except socket.timeout:
            raise TimeoutError("Timed out waiting for server response")

    def close(self):
        self.sock.close()
