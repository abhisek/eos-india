#!/usr/bin/env python 
import sys
import os
import socket
import struct
import random
import SMB

DEBUG = 1

# DCE RPC Version
DCERPC_VERSION_MAJOR	= 5
DCERPC_VERSION_MINOR	= 0

# DCE RPC Packet types
DCERPC_BIND 	= 11
DCERPC_BIND_ACK	= 12
DCERPC_BIND_NAK	= 13
DCERPC_ALT_CTX	= 15
DCERPC_CALL 	= 00
DCERPC_RESPONSE	= 02
DCERPC_FAULT	= 03

# DCE RPC Common Header Flags
PFC_FIRST_FRAG		= 0x01	# PDU is first frag
PFC_LAST_FRAG		= 0x02	# PDU is last frag
PFC_CANCEL_PENDING	= 0x04	# Cancel any pending PDU 
PFC_RESERVED		= 0x08	# Reserved
PFC_CONC_MPX		= 0x10	# Support cocurrent multiplexing
PFC_DID_NOT_EXEC	= 0x20	# RPC was not executed
PFC_MAYBE		= 0x40	# `maybe` call semantic requested
PFC_OBJECT_UUID		= 0x80	# Object UUID is included

# DCE RPC Header Size
DCERPC_COMMON_HEADER_SIZE 	= 16
DCERPC_CALL_HEADER_SIZE		= 12

# Microsoft DCERPC known UUIDs
UUIDs = { 'MGMT':'afa8bd80-7d8a-11c9-bef4-08002b102989', 
	'REMACT':'4d9f4ab8-7d1c-11cf-861e-0020af6e7c57',
	'SYSACT':'000001a0-0000-0000-c000-000000000046',
	'LSA_DS':'3919286a-b10c-11d0-9ba8-00c04fd92ef5',
	'SAMR':'12345778-1234-abcd-ef00-0123456789ac',
	'MSMQ':'fdb3a030-065f-11d1-bb9b-00a024ea5525',
	'EVENTLOG':'82273fdc-e32a-18c3-3f78-827929dc23ea',
	'SVCCTL':'367abb81-9844-35f1-ad32-98f038001003',
	'SPOOLER':'12345678-1234-abcd-ef00-0123456789a',
	'TAPI':'2F5F6520-CA46-1067-B319-00DD010662DA',
	'UMPNPMGR':'8d9f4e40-a03d-11ce-8f69-08003e30051b',
	'MSDTC':'906b0ce0-c70b-1067-b317-00dd010662da'
	}

class NDR:
	"""
	This class contains methods required for NDR data encoding used for Marshalling
	of data supported by Microsoft implementation of DCE RPC
	"""

	def DwordAlign(self, len):
		string = ''
		len = ((4 - (len & 3 )) & 3)
		for x in range(0,len):
			string += chr(random.randint(1,255))
		return string

	def Ascii2Unicode(self, string):
		unicode = ''
		for x in string:
			unicode += struct.pack('<H',ord(x))
		return unicode

	def Unicode2Ascii(self, string):
		ascii = ''
		for x in range(0,len(string),2):
			ascii += string[x+1]
		return ascii

	def Hyper(self, *hyper):
		ret = ''
		for h in hyper:
			ret += struct.pack('<Q',h)
		return ret
		
	def Long(self, *long):
		ret = ''
		for l in long:
			ret += struct.pack('<L',l)
		return ret

	def Short(self, *short):
		ret = ''
		for s in short:
			ret += struct.pack('<H',s)
		return ret

	def Byte(self, *byte):
		ret = ''
		for b in byte:
			ret += struct.pack('<B',b)
		return ret

	def UnicodeConformantVaryingString(self, string):
		length = len(string)
		
		if not length or string[length-1] != '\x00':
			string += '\x00'
			length += 1
			
		string = self.Long(length,0,length) + self.Ascii2Unicode(string)
		
		align = self.DwordAlign(length*2)
		if align:
			string += align
			
		return string

	def UnicodeConformantVaryingStringPreBuilt(self, string):
		if len(string) % 2:
			string += '\x00'

		l = len(string) / 2
		string = self.Long(l,0,l) + string
		
		align = self.DwordAlign(len(string))
		if align:
			string += align

		return string

	def UniConformantArray(self, array):
		string = self.Long(len(array)) + array

		align = self.DwordAlign(len(string))
		if align:
			string += align

		return string

	def UniConformantByteArray(self, byte_array):
		string = self.Long(len(byte_array),len(byte_array)) + byte_array
		
		align = self.DwordAlign(len(string))
		if align:
			string += align

		return string

class DCERPC:
	"""
	This class contains most of the functions required for doing Remote Procedure Call
	as per DCE/X-Open standards. It also implements methods for doing Microsoft specific
	Remote Procedure Call implementation.
	"""
	
	def __init__(self):
		self.assoc_group 	= 0x00
		self.callid 		= 1
		self.ep			= 'ncan_ip_tcp' # DCERPC End point
		self.max_xmit_frag	= 4280
		self.max_recv_frag	= 4280
		self.connected		= 0
		self.smb_user		= ''
		self.smb_pass		= ''
		self.smb_serv_name	= '*SMBSERVER'
		self.smb_my_name	= 'WORKGROUP'
		self.smb_share		= ''
		self.smb_pipe		= ''
		self.smb_tid		= 0
		self.smb_fid		= 0
		self.timeout		= 20

	def debug(self,error,*msg):
		if not DEBUG:
			return
		if(error):
			buf = '[-] DCERPC:'
		else:
			buf = '[+] DCERPC:'
		for x in msg:
			buf += x
		print buf

	def __cleanup__(self):
		self.debug(0,'Destroying DCERPC class')
		if self.connected:
			if self.ep == 'ncan_ip_tcp':
				self.sock.close()
			if self.ep == 'ncan_np':
				self.sock.close_file()
				self.sock.disconnect_tree()
				self.sock.logoff()

	def SetEndPoint(self,endpoint):
		self.debug(0,'Setting endpoint to \'',endpoint,'\'')
		self.ep = endpoint

	# Sets the information required for doing RPC over SMB
	def SetSMBInformation(self,user,passwd,serv_name,my_name,share,pipe):
		if len(user):
			self.smb_user = user
		if len(passwd):
			self.smb_pass = passwd
		if len(serv_name):
			self.smb_serv_name = serv_name
		if len(my_name):
			self.smb_my_name = my_name
		if len(share):
			self.smb_share = share
		if len(pipe):
			self.smb_pipe = pipe

	## Socket Functions ##

	def ConnectServer(self,host,port):
		if self.ep == 'ncan_ip_tcp':
			self.debug(0,'Using ncan_ip_tcp endpoint')
			self.sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
			self.sock.settimeout(self.timeout)
			try:
				self.debug(0,'Connecting to ',host,':',str(port))
				self.sock.connect((host,port))
				self.connected = 1
			except:
				self.debug(0,'Failed to connect')
				return False
			return True
		elif self.ep == 'ncan_np':
			self.debug(0,'Using ncan_np endpoint')
			self.debug(0,'Doing SMB Negotiation')
			try:
				self.sock = SMB.SMB(self.smb_serv_name,
				host,
				my_name=self.smb_my_name,
				sess_port=port,
				timeout = self.timeout)
			except SMB.SessionError:
				self.debug(1,'SMB Negotiation Failed')
				return False
			self.debug(0,'Doing SMB SessionSetupAndX')
			try:
				self.sock.login(self.smb_user,self.smb_pass)
			except SMB.SessionError:
				self.debug(1,'SMB SessionSetupAndX Failed')
				return False
			self.debug(0,'Connecting to SMB Share ',self.smb_share)
			try:
				self.smb_tid = self.sock.connect_tree(
				self.smb_share,
				'?????\x00',
				self.smb_pass)
			except SMB.SessionError:
				self.debug(1,'Failed to connected to share: ',self.smb_share)
				return False
			self.debug(0,'Opening named pipe ',self.smb_pipe,' for reading/writing')
			try:
				self.smb_fid = self.sock.nt_create(self.smb_tid,
				self.smb_pipe)
			except SMB.SessionError:
				self.debug(1,'Failed to open pipe: ',self.smb_pipe)
				return False
				
			self.connected = 1
			return True
		else:
			self.debug(1,'Unknown DCERPC endpoint')
			return False
	
	def SendPacket(self,packet):
		if not self.connected:
			self.debug(1,'Socket not connected')
			return False
		if self.ep == 'ncan_ip_tcp':
			s = self.sock.send(packet)
			self.debug(0,'Send ',str(s),' bytes to ncan_ip_tcp endpoint')
			return s
		elif self.ep == 'ncan_np':
			try:
				self.sock.write_file(self.smb_tid, 
				self.smb_fid , 
				0, 
				len(packet), 
				packet)
			except SMB.SessionError:
				self.debug(1,'Failed to write data to ncan_np endpoint')
				return False
			return True
		return False

	def RecvPacket(self):
		if not self.connected:
			self.debug(1,'Socket not connected')
			return False
		if self.ep == 'ncan_ip_tcp':
			# Read the PDU Header
			packet = self.sock.recv(10)
			if len(packet) < 10:
				self.debug(1,'Error receiving packet from ncan_ip_tcp endpoint')
				return False

			# Decode Fragment Length
			fraglen = struct.unpack('<H',packet[8:10])[0]

			# Read the rest of the PDU
			packet += self.sock.recv(fraglen - 10)

			# Validate
			if len(packet) != fraglen:
				self.debug(1,'Fraglen doesnt match with actual length of packet')
				return False
				
			self.debug(0,'Received PDU of fraglen ',str(fraglen),' from ncan_ip_tcp endpoint')
			return packet
		elif self.ep == 'ncan_np':
			try:
				packet = self.sock.read_file(self.smb_tid, 
				self.smb_fid, 
				0x00)
			except SMB.SessionError:
				self.debug(1,'Error receiving packet from ncan_np endpoint')
				return False
			if not packet:
				self.debug(1,'Error receiving packet from ncan_np endpoint')
				return False
				
			fraglen = struct.unpack('<H',packet[8:10])[0]
			self.debug(0,'Received PDU of fraglen ',str(fraglen),' from ncan_np endpoint')

			# Validate
			if len(packet) != fraglen:
				self.debug(1,'Fraglen doesnt match with actual length of packet')
				return False
			return packet

		return False

	## End of Socket Functions ##

	## Utility Functions ##
	
	# This function converts a UUID string into its binary equivalent
	# Expected UUID format: 'afa8bd80-7d8a-11c9-bef4-08002b102989'
	def UUID_to_Bin(self,uuid):
		if not uuid:
			return False
		chunks = uuid.split('-')
		if len(chunks) < 5:
			self.debug(1,'Invalid UUID string')
			return False
		bin_uuid = struct.pack('<L',int(chunks[0],16))
		bin_uuid += struct.pack('<H',int(chunks[1],16))
		bin_uuid += struct.pack('<H',int(chunks[2],16))
		bin_uuid += struct.pack('>H',int(chunks[3],16))
		for x in range(0,len(chunks[4]),2):
			bin_uuid += struct.pack('B',int(chunks[4][x:x+2],16))

		return bin_uuid

	def Bin_to_UUID(self,bin_uuid):
		string = '%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x' % (
		struct.unpack('<L',bin_uuid[0:4])[0],
		struct.unpack('<H',bin_uuid[4:6])[0],
		struct.unpack('<H',bin_uuid[6:8])[0],
		struct.unpack('>H',bin_uuid[8:10])[0],
		struct.unpack('B',bin_uuid[10:11])[0],
		struct.unpack('B',bin_uuid[11:12])[0],
		struct.unpack('B',bin_uuid[12:13])[0],
		struct.unpack('B',bin_uuid[13:14])[0],
		struct.unpack('B',bin_uuid[14:15])[0],
		struct.unpack('B',bin_uuid[15:16])[0]
		)

		return string

	def IncrementCallID(self):
		self.callid += 1

	def TransferSyntax(self):
		return self.UUID_to_Bin('8a885d04-1ceb-11c9-9fe8-08002b104860')

	def TransferSyntaxVersion(self):
		return struct.pack('<hh',2,0)
	
	## End of Utility Functions ##

	## DCE RPC Operations ##

	# This function returns binary packed header information
	def BuildCommonHeader(self,major_ver,minor_ver,type,flags,datarep,fraglen,authlen,callid):
		header = struct.pack('b',major_ver)	# DCERPC Major Version
		header += struct.pack('b',minor_ver)	# DCERPC Minor Version
		header += struct.pack('b',type)		# DCERPC PDU Type
		header += struct.pack('b',flags)	# DCERPC Flags
		header += struct.pack('>l',datarep)	# DCERPC Data Representation Format
		header += struct.pack('<h',fraglen)	# DCERPC Fragment Length
		header += struct.pack('<h',authlen)	# DCERPC Authentication Context Length
		header += struct.pack('<l',callid)	# DCERPC Caller ID

		return header

	# This functions returns a DCERPC Bind Interface Request PDU
	def BuildBindRequest(self, uuid, uuid_ver_major, uuid_ver_minor):
		if not uuid:
			return False
	
		# Build the Common Header
		BindPacket = self.BuildCommonHeader(
		DCERPC_VERSION_MAJOR,		# Major Version
		DCERPC_VERSION_MINOR,		# Minor Version
		DCERPC_BIND,			# DCERPC Packet type
		PFC_FIRST_FRAG|PFC_LAST_FRAG,	# DCERPC Flags
		0x10000000,			# DCERPC Data Representation
		72,				# Frag Size
		0,				# Associated Group
		self.callid)			# Caller ID

		# Build the Bind Request Header
		BindPacket += struct.pack('<h',self.max_xmit_frag)	# Maximum Send Frag
		BindPacket += struct.pack('<h',self.max_recv_frag)	# Maximum Recv Frag
		BindPacket += struct.pack('<l',self.assoc_group)	# Associated Group
		BindPacket += struct.pack('b',1)			# Number of Context
		BindPacket += '\x00'*3					# Reserved
		BindPacket += struct.pack('<h',0)			# Context ID
		BindPacket += struct.pack('b',1)			# Number of Transaction items
		BindPacket += '\x00'					# Reserved
		BindPacket += self.UUID_to_Bin(uuid)			# Interface UUID
		BindPacket += struct.pack('<h',uuid_ver_major)		# UUID Major Version
		BindPacket += struct.pack('<h',uuid_ver_minor)		# UUID Minor Version
		BindPacket += self.TransferSyntax()			# Transfer Syntax
		BindPacket += self.TransferSyntaxVersion()
		
		return BindPacket
		
	# Function to be called for binding to remote RPC interface
	def BindInterface(self, uuid, uuid_ver_major, uuid_ver_minor):
		if not self.connected:
			self.debug(1,'Socket not connected')
			return False

		# Build the DCERPC Bind Request Header
		request = self.BuildBindRequest(uuid, uuid_ver_major, uuid_ver_minor)

		# Send the PDU
		self.debug(0,'Sending Bind request PDU')
		if not self.SendPacket(request):
			return False

		# Read the response
		response = self.RecvPacket()
		if not response:
			self.debug(1,'Failed to read response')
			return False

		# Decode the packet
		type = struct.unpack('b',response[2])[0]	# DCERPC Packet Type
		if type != DCERPC_BIND_ACK:
			self.debug(1,'Unexpected PDU received (PDU type ',str(type),')')
			return False

		self.debug(0,'DCERPC Bind ACK response received')
		index = 16	# Start of BIND ACK Response Header
		
		self.max_xmit_frag = struct.unpack('<H',response[index:index+2])[0]	# Maximum Trasmit Frag
		self.debug(0,'\tMax_xmit_frag: ',str(self.max_xmit_frag))
		index += 2
		
		self.max_recv_frag = struct.unpack('<H',response[index:index+2])[0]	# Maximum Recv Frag
		self.debug(0,'\tMax_recv_frag: ',str(self.max_recv_frag))
		index += 2
		
		self.assoc_group = struct.unpack('<L',response[index:index+4])[0]	# Associated Group
		self.debug(0,'\tAssociated Group: ',str(self.assoc_group))
		index += 4
		
		secondary_addr_len = struct.unpack('<H',response[index:index+2])[0]	# Secondary Address Len
		self.debug(0,'\tSecondary Address Len: ',str(secondary_addr_len))
		index += 2
		
		secondary_addr = response[index:index+secondary_addr_len]	# Secondary Address
		self.debug(0,'\tSecondary Address: ',secondary_addr)
		index += secondary_addr_len
		
		if secondary_addr_len % 2:
			index += 1	# Skip reserved byte for alignment 
			
		num_results = struct.unpack('b',response[index:index+1])[0]	# Number of results
		index += 1
		
		index += 3	# Skip reserved bytes
		
		ack_result = struct.unpack('<H',response[index:index+2])[0]	# Acknowledgement Result
		self.debug(0,'\tAcknowledgement Result: ',str(ack_result))
		index += 2
		
		ack_reason = struct.unpack('<H',response[index:index+2])[0]
		self.debug(0,'\tAcknowledgement Reason: ',str(ack_reason))
		index += 2
		
		trans_syntax = response[index:index+16]				# Transfer syntax
		index += 16
	
		trans_syntax_ver = struct.unpack('<L',response[index:index+4])[0]

		if ack_result:
			self.debug(1,'Bind Request denied')
			return False

		self.debug(0,'DCERPC Bind request accepted')
			
		return True

	# This function returns a list of DCERPC Request frags
	def BuildCallRequest(self,opnum,stub):
		frag_list = []
		Packet_List = []
		stub_size = len(stub)
		CallPacket = ''
	
		# Calculate fragcount and frag the data
		if stub_size > self.max_recv_frag:	# Fragging Required
			frag_count = stub_size / self.max_recv_frag
			if stub_size % self.max_recv_frag:
				frag_count += 1
		else:
			frag_count = 1			# Fragging Not Required
	
		while frag_count:
			start = 0
			if frag_count == 1:
				end = len(stub)
			else:
				end = self.max_recv_frag
			frag_list.append(stub[start:end])
			stub = stub[end:]
			
			frag_count -= 1

		for x in range(0,len(frag_list)):

			flags = 0
			if x > 0 and x < (len(frag_list)-1):
				flags = 0
			if x == 0:
				flags |= PFC_FIRST_FRAG
			if x == (len(frag_list)-1):
				flags |= PFC_LAST_FRAG
			
			CallPacket = self.BuildCommonHeader(
			DCERPC_VERSION_MAJOR,		# DCERPC Major Version
			DCERPC_VERSION_MINOR,		# DCERPC Minor Version
			DCERPC_CALL,			# DCERPC Packet type
			flags,				# DCERPC Flag
			0x10000000,			# DCERPC Data Representation Format
			len(frag_list[x])+24,		# Frag Size
			0,				# Authentication Length
			self.callid)			# Caller ID

			CallPacket += struct.pack('<L',len(frag_list[x]))	# Allocation Hint
			CallPacket += struct.pack('<H',0x00)			# Context ID
			CallPacket += struct.pack('<H',opnum)
			CallPacket += frag_list[x]

			Packet_List.append(CallPacket)

		return Packet_List


	# This function performs a RPC
	def CallProcedure(self,opnum,stub,noresponse=0):
		if not self.connected:
			self.debug(1,'Socket not connected')
			return False

		# Build the RPC header
		request = self.BuildCallRequest(opnum,stub)
		
		self.debug(0,'Sending stub of size [',str(len(stub)),'] opnum[',str(opnum),']')
		# Send the frags
		i = 0
		if len(request) > 1:
			while i < (len(request)-1):
				self.debug(0,'\tSending frag[',str(len(request[i])),' bytes]',' [',str(i+1),'/',str(len(request)),']')
				if not self.SendPacket(request[i]):
					self.debug(1,'Send failed')
					return False
				i += 1

		if noresponse:
			self.SendPacket(request[i])
			self.debug(0,'noresponse=1')
			return True

		# Make the SMB_TransactNamedPipe setup
		setup = struct.pack('<HH',38,self.smb_fid)
		
		# Send the SMB_TransactNamedPipe/Last TCP packet
		if self.ep == 'ncan_np':
			response = self.sock.send_transNP(self.smb_tid,self.smb_fid,request[i])
		else:
			response = self.SendPacket(request[i])
			response = self.RecvPacket()

		if not response:
			self.debug(1,'Failed to receive response')
			return False

		# Decode the response
		type = struct.unpack('b',response[2])[0]	# DCERPC Packet Type
		if type == DCERPC_FAULT:
			fault_status = struct.unpack('<L',response[24:28])[0]
			self.debug(1,'Received fault PDU (Fault status: ',hex(fault_status),')')
			return False

		if type != DCERPC_RESPONSE:
			self.debug(1,'Unexpected PDU (type: ',str(type),')')
			return False

		flags = struct.unpack('b',response[3])[0]
		if flags & PFC_DID_NOT_EXEC:
			self.debug(1,'RPC was not executed')
			return False
	
		# Get the RPC response stub
		response_stub = response[24:]
		self.debug(0,'Received response stub of ',str(len(response_stub)),' bytes')
					
		return response_stub

	## End of DCE RPC Operations ##
		
# For testing purpose
if __name__ == '__main__':
	rpc = DCERPC()
	rpc.SetEndPoint('ncan_np')
	rpc.SetSMBInformation('','','*SMBSERVER','','\\\\192.168.0.18\\IPC$','\\browser')
	rpc.ConnectServer('192.168.0.18',139)
	rpc.BindInterface(UUIDs['REMACT'],0,0)
#	stub = 'A'*55555
#	rpc.CallProcedure(22,stub)
#	rpc.BindInterface('afa8bd80-7d8a-11c9-bef4-08002b102989',2,0)
