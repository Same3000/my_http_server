import requests
import unittest
import socket
import subprocess as sp
import time

HOST: str = "127.0.0.1"
PORT: str = "8000"
EXE: str = "./httpd"

def spawn(httpd_exec: str, config_file: str, stdout_file: str) ->sp.Popen:
	with open(stdout_file, "w") as f:
		httpd_process = sp.Popen([httpd_exec, config_file],
		stdout=f, stderr=sp.PIPE, bufsize=0)
	time.sleep(0.10)

	return httpd_process

def kill_proc(process: sp.Popen):
	process.kill()
	
def test_get_index():
	proc = spawn(EXE, "src/config/tests/local_test.txt", "test.log")
	response = requests.get(f"http://{HOST}:{PORT}/file1.txt")
	assert response.status_code == 200
	assert response.raw.version == 11
	with open("config/tests/file1.txt", "r") as f:
		assert response.text == f.read()
	kill_proc(proc)

def test_get_noindex():
	proc = spawn(EXE, "src/config/tests/local_test.txt", "test.log")
	response = requests.get(f"http://{HOST}:{PORT}/file2.txt")
	assert response.status_code == 404
	assert response.raw.version == 11
	kill_proc(proc)

def test_bad_request():
	proc = spawn(EXE, "src/config/tests/local_test.txt", "test.log")
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((HOST, int(PORT)))	
	sock.sendall(b"GET /file1.txt HTTP1/2\r\nHost: localhost\r\nConnection:close\r\n\r\n")
	resp = sock.recv(1024)
	resp_decoded = resp.decode()
	assert "505" in resp_decoded
	kill_proc(proc)

def test_get_no_accees_index():
	proc = spawn(EXE, "src/config/tests/local_test.txt", "test.log")
	response = requests.get(f"http://{HOST}:{PORT}/no_access_test.txt")
	assert response.status_code == 403
	assert response.raw.version == 11
	kill_proc(proc)
