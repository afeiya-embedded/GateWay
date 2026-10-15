Sub Main
	xsh.Session.Open("C:\\Users\\...\\localhost.xsh")
	xsh.Screen.Synchronous = true
	xsh.Session.Sleep(1000)
	
	'*** WaitForString ***
	xsh.Screen.WaitForString("aaa")		' input "aaa" in Terminal
	
	'*** Send ***
	xsh.Screen.Send("cat /etc/passwd")
	xsh.Screen.Send(VbCr)
	xsh.Session.Sleep(1000)
	
	'*** Get, Clear ***
	Dim ScreenRow, ReadLine, Items
	
	ScreenRow = xsh.Screen.CurrentRow - 1
	ReadLine = xsh.Screen.Get(ScreenRow, 1, ScreenRow, 40)
	Items = Split(ReadLine, ":", -1)
	xsh.Dialog.MsgBox(Items(2))
	xsh.Screen.Clear()
	
	'*** Property ***
	xsh.Screen.Synchronous = False
	Dim MsgProp
	MsgProp = "Col:" + CStr(xsh.Screen.CurrentColumn) + "\n"
	MsgProp = MsgProp + "Row:" + CStr(xsh.Screen.CurrentRow) + "\n"
	MsgProp = MsgProp + "TermCol:" + CStr(xsh.Screen.Columns) + "\n"
	MsgProp = MsgProp + "TermRow:" + CStr(xsh.Screen.Rows)
	xsh.Dialog.MsgBox(MsgProp)
End Sub