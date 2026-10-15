function Main()
{
	// *** Open, Close, Sleep ***
	xsh.Session.Open("ssh://test:test@192.168.X.X") 	
	//xsh.Session.Open("C:\\Users\\...\\localhost.xsh");
	xsh.Screen.Synchronous = true;
	xsh.Session.Sleep(1000);
	
	//*** StartLog, StopLog ***
	xsh.Session.LogFilePath = "C:\\Users\\...\\example.log";
	xsh.Session.StartLog();
	
	//*** Property ***
	var MsgInfo;
	MsgInfo = "LogPath: " + xsh.Session.LogFilePath + "\n";
	MsgInfo = MsgInfo + "LocalAddress: " + xsh.Session.LocalAddress + "\n";
	MsgInfo = MsgInfo + "Path: " + xsh.Session.Path + "\n";
	MsgInfo = MsgInfo + "RemoteAddress: " + xsh.Session.RemoteAddress + "\n";
	
	var RPort, Conn, Log;
	RPort = xsh.Session.RemotePort;
	Conn = xsh.Session.Connected;
	Log = xsh.Session.Logging;
	MsgInfo = MsgInfo + "RemotePort: " + RPort + "\n";
	MsgInfo = MsgInfo + "Connected: " + Conn + "\n";
	MsgInfo = MsgInfo + "Logging: " + Log;
	xsh.Dialog.MsgBox(MsgInfo);
	
	xsh.Session.Close();
	xsh.Session.StopLog();
}