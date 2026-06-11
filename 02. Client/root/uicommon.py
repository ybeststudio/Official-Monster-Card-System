# Add anywhere in the MountUpGradeDialog class:
if app.ENABLE_MONSTER_CARD:
	class ExPopupDialog(ui.ScriptWindow):

		def __init__(self, layer = "UI"):
			ui.ScriptWindow.__init__(self, layer)
			self.__LoadDialog()
			self.acceptEvent = lambda *arg: None

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __LoadDialog(self):
			try:
				PythonScriptLoader = ui.PythonScriptLoader()
				PythonScriptLoader.LoadScriptFile(self, "UIScript/PopupDialog.py")

				self.board = self.GetChild("board")
				self.message = self.GetChild("message")
				self.accceptButton = self.GetChild("accept")
				self.accceptButton.SetEvent(ui.__mem_func__(self.Close))

			except:
				import exception
				exception.Abort("PopupDialog.LoadDialog.BindObject")

		def Open(self):
			self.SetCenterPosition()
			self.SetTop()
			self.Show()

		def Close(self):
			self.Hide()
			self.acceptEvent()

		def Destroy(self):
			self.Close()
			self.ClearDictionary()

		def SetWidth(self, width):
			height = self.GetHeight()
			self.SetSize(width, height)
			self.board.SetSize(width, height)
			self.SetCenterPosition()
			self.UpdateRect()

		def SetText(self, text):
			self.message.SetText(text)

		def SetAcceptEvent(self, event):
			self.acceptEvent = event

		def SetButtonName(self, name):
			self.accceptButton.SetText(name)

		def OnPressEscapeKey(self):
			self.Close()
			return True

		def OnIMEReturn(self):
			self.Close()
			return True
			
		def GetTextSize(self):
			if self.message:
				return self.message.GetTextSize()
				
			return (0,0)
			
		def GetLineHeight(self):
			if self.message:
				return self.message.GetLineHeight()
			
			return 0
				
		if app.WJ_MULTI_TEXTLINE or app.ENABLE_EXTEND_INVEN_SYSTEM:
			def SetLineHeight(self, Height):
				self.message.SetLineHeight(Height)
				
			def GetTextLineCount(self):
				return self.message.GetTextLineCount()
				
				
	class ExQuestionDialog(ui.ScriptWindow):

		def __init__(self, layer = "UI"):
			ui.ScriptWindow.__init__(self, layer)
			self.__CreateDialog()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __CreateDialog(self):
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog.py")

			self.board = self.GetChild("board")
			self.textLine = self.GetChild("message")
			self.acceptButton = self.GetChild("accept")
			self.cancelButton = self.GetChild("cancel")

		def Open(self):
			self.SetCenterPosition()
			self.SetTop()
			self.Show()

		def Close(self):
			self.Hide()

		def SetWidth(self, width):
			height = self.GetHeight()
			self.SetSize(width, height)
			
			if self.board.IsRTL():
				self.board.SetPosition(width, 0)
				
			self.board.SetSize(width, height)
			self.SetCenterPosition()
			self.UpdateRect()

		def SAFE_SetAcceptEvent(self, event):
			self.acceptButton.SAFE_SetEvent(event)

		def SAFE_SetCancelEvent(self, event):
			self.cancelButton.SAFE_SetEvent(event)

		def SetAcceptEvent(self, event):
			self.acceptButton.SetEvent(event)

		def SetCancelEvent(self, event):
			self.cancelButton.SetEvent(event)

		def SetText(self, text):
			self.textLine.SetText(text)

		def SetAcceptText(self, text):
			self.acceptButton.SetText(text)

		def SetCancelText(self, text):
			self.cancelButton.SetText(text)

		def OnPressEscapeKey(self):
			self.Close()
			return True
			
		def GetTextSize(self):
			if self.textLine:
				return self.textLine.GetTextSize()
				
			return (0,0)
			
		def GetLineHeight(self):
			if self.textLine:
				return self.textLine.GetLineHeight()
			
			return 0
				
		def SetLineHeight(self, Height):
			self.textLine.SetLineHeight(Height)
			
		def GetTextLineCount(self):
			return self.textLine.GetTextLineCount()
