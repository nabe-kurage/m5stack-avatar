-- M5Stack Avatar Controller
-- AppleScriptによるGUI制御アプリ

-- 設定
set projectPath to (path to me as string)
set scriptFolder to (POSIX path of (projectPath as string)) & "Contents/Resources/"
set pythonScript to scriptFolder & "m5stack_controller.py"

-- メインダイアログを表示
on run
	showMainDialog()
end run

-- メインダイアログ
on showMainDialog()
	try
		set userChoice to (display dialog "M5Stack Avatar Controller" & return & return & "M5Stackの表情を変更してください：" buttons {"😊 Happy", "😢 Sad", "😠 Angry", "その他...", "❌ 終了"} default button "😊 Happy" with icon note)
		
		set buttonPressed to button returned of userChoice
		
		if buttonPressed is "😊 Happy" then
			sendCommand("happy")
		else if buttonPressed is "😢 Sad" then
			sendCommand("sad")
		else if buttonPressed is "😠 Angry" then
			sendCommand("angry")
		else if buttonPressed is "その他..." then
			showMoreOptions()
		else
			return -- 終了
		end if
		
		-- 成功メッセージを表示して再度メニューを表示
		display dialog "コマンドを送信しました！" & return & return & "続けて操作しますか？" buttons {"はい", "いいえ"} default button "はい"
		if button returned of result is "はい" then
			showMainDialog()
		end if
		
	on error errMsg
		display dialog "エラーが発生しました：" & return & errMsg buttons {"OK"} with icon stop
	end try
end showMainDialog

-- その他のオプション
on showMoreOptions()
	try
		set userChoice to (display dialog "M5Stack Avatar Controller" & return & return & "その他の表情とアクション：" buttons {"😴 Sleepy", "🤔 Doubt", "😐 Neutral", "🎵 Play", "戻る"} default button "😴 Sleepy" with icon note)
		
		set buttonPressed to button returned of userChoice
		
		if buttonPressed is "😴 Sleepy" then
			sendCommand("sleepy")
		else if buttonPressed is "🤔 Doubt" then
			sendCommand("doubt")
		else if buttonPressed is "😐 Neutral" then
			sendCommand("neutral")
		else if buttonPressed is "🎵 Play" then
			sendCommand("play")
		else
			showMainDialog()
			return
		end if
		
	on error errMsg
		display dialog "エラーが発生しました：" & return & errMsg buttons {"OK"} with icon stop
	end try
end showMoreOptions

-- コマンドを送信
on sendCommand(command)
	try
		-- Pythonスクリプトのパスを取得
		set currentPath to (path to me as string)
		set parentFolder to (POSIX path of ((currentPath as text) & "::"))
		set pythonScriptPath to parentFolder & "m5stack_controller.py"
		
		-- コマンドを実行
		set commandLine to "cd " & quoted form of parentFolder & " && python3 " & quoted form of "m5stack_controller.py" & " " & quoted form of command
		
		do shell script commandLine
		
		return true
		
	on error errMsg
		display dialog "コマンドの送信に失敗しました：" & return & errMsg & return & return & "確認事項：" & return & "• M5StackがUSBで接続されているか" & return & "• M5Stackに新しいコードがアップロード済みか" & return & "• Pythonとpyserialがインストールされているか" buttons {"OK"} with icon stop
		return false
	end try
end sendCommand

-- ステータス確認
on checkStatus()
	try
		set currentPath to (path to me as string)
		set parentFolder to (POSIX path of ((currentPath as text) & "::"))
		set commandLine to "cd " & quoted form of parentFolder & " && python3 m5stack_controller.py status"
		
		set result to (do shell script commandLine)
		display dialog "M5Stack ステータス：" & return & return & result buttons {"OK"} with icon note
		
	on error errMsg
		display dialog "ステータス確認に失敗しました：" & return & errMsg buttons {"OK"} with icon stop
	end try
end checkStatus
