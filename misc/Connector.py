import telnetlib
import socket

def InterectShell(host, port):
	try:
		tt = telnetlib.Telnet(host,port)
	except socket.error:
		print '[-]Connector: Unable to connect to %s:%d' % (host,port)
		return False
	print '[+] Connector: Connected to %s:%d' % (host,port)
	print '[+] Connector: Entering interective session\n'
	tt.interact()
