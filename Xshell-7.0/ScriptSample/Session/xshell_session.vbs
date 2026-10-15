Sub Main
	' *** Open, Close, Sleep ***
	xsh.Session.Open("ssh://test:test@192.168.X.X")
	'xsh.Session.Open("C:\\Users\\...\\localhost.xsh")
	xsh.Screen.Synchronous = true
	xsh.Session.Sleep(1000)
	
	'*** StartLog, StopLog ***
	xsh.Session.LogFilePath = "C:\\Users\\...\\example.log"
	xsh.Session.StartLog()
	
	'*** Property ***
	Dim MsgInfo
	MsgInfo = "LogPath: " + xsh.Session.LogFilePath + "\n"
	MsgInfo = MsgInfo + "LocalAddress: " + xsh.Session.LocalAddress + "\n" 
	MsgInfo = MsgInfo + "Path: " + xsh.Session.Path + "\n"  
	MsgInfo = MsgInfo + "RemoteAddress: " + xsh.Session.RemoteAddress + "\n"  
	MsgInfo = MsgInfo + "RemotePort: " + CStr(xsh.Session.RemotePort) + "\n"
	MsgInfo = MsgInfo + "Connected: " + CStr(xsh.Session.Connected) + "\n"
	MsgInfo = MsgInfo + "Logging: " + CStr(xsh.Session.Logging)
	xsh.Dialog.MsgBox(MsgInfo)
	
	xsh.Session.Close()
	xsh.Session.StopLog()
End Sub