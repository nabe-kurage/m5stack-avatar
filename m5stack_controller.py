#!/usr/bin/env python3
"""
M5Stack Avatar Controller for Mac
USB Type-C経由でM5Stackの表情を制御するスクリプト

使用方法:
  python3 m5stack_controller.py happy
  python3 m5stack_controller.py sad
  python3 m5stack_controller.py status
"""

import serial
import serial.tools.list_ports
import time
import sys
import argparse

class M5StackController:
    def __init__(self, port=None, baud_rate=115200):
        self.baud_rate = baud_rate
        self.port = port
        self.serial_connection = None
        
        if not self.port:
            self.port = self.find_m5stack_port()
            
    def find_m5stack_port(self):
        """M5Stackのシリアルポートを自動検出"""
        print("M5Stackのシリアルポートを検索中...")
        
        ports = serial.tools.list_ports.comports()
        for port in ports:
            # M5Stack関連のポートを検索
            if 'usb' in port.device.lower() or 'serial' in port.device.lower():
                print(f"見つかったポート: {port.device} - {port.description}")
                return port.device
                
        # 見つからない場合は手動設定を促す
        print("M5Stackのポートが見つかりません。")
        print("利用可能なポート:")
        for port in ports:
            print(f"  {port.device} - {port.description}")
        
        if ports:
            return input("使用するポートを入力してください: ").strip()
        return None
    
    def connect(self):
        """M5Stackに接続"""
        if not self.port:
            print("エラー: シリアルポートが指定されていません")
            return False
            
        try:
            self.serial_connection = serial.Serial(
                self.port, 
                self.baud_rate, 
                timeout=2
            )
            time.sleep(2)  # 接続安定化のため待機
            print(f"M5Stackに接続しました: {self.port}")
            return True
            
        except serial.SerialException as e:
            print(f"接続エラー: {e}")
            return False
    
    def disconnect(self):
        """M5Stackから切断"""
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.close()
            print("M5Stackから切断しました")
    
    def send_command(self, command):
        """M5Stackにコマンドを送信"""
        if not self.serial_connection or not self.serial_connection.is_open:
            if not self.connect():
                return False
                
        try:
            # コマンド送信
            command_with_newline = f"{command}\n"
            self.serial_connection.write(command_with_newline.encode('utf-8'))
            self.serial_connection.flush()
            
            print(f"送信: {command}")
            
            # レスポンス読み取り
            time.sleep(0.5)
            response_lines = []
            
            while self.serial_connection.in_waiting > 0:
                line = self.serial_connection.readline().decode('utf-8').strip()
                if line:
                    response_lines.append(line)
            
            if response_lines:
                print("レスポンス:")
                for line in response_lines:
                    print(f"  {line}")
            
            return True
            
        except Exception as e:
            print(f"コマンド送信エラー: {e}")
            return False
    
    def interactive_mode(self):
        """インタラクティブモード"""
        print("\n=== M5Stack Avatar インタラクティブ制御 ===")
        print("利用可能なコマンド: happy, sad, angry, sleepy, doubt, neutral, rora, play, pakupaku [秒数], status, help")
        print("例: pakupaku 5 (5秒間ぱくぱく)")
        print("終了するには 'quit' または Ctrl+C を押してください\n")
        
        if not self.connect():
            return
            
        try:
            while True:
                command_input = input("コマンドを入力: ").strip()
                
                if command_input.lower() in ['quit', 'exit', 'q']:
                    break
                elif command_input:
                    # pakupakuコマンドの特別な処理
                    parts = command_input.split()
                    if len(parts) >= 2 and parts[0].lower() == 'pakupaku':
                        try:
                            duration = float(parts[1])
                            if 0.1 <= duration <= 60:
                                self.send_command(f"pakupaku {duration}")
                            else:
                                print("エラー: 秒数は0.1から60の間で指定してください")
                        except ValueError:
                            print("エラー: 秒数は数値で指定してください")
                    else:
                        self.send_command(command_input.lower())
                    
        except KeyboardInterrupt:
            print("\n終了します...")
        finally:
            self.disconnect()

def main():
    parser = argparse.ArgumentParser(description='M5Stack Avatar Controller')
    parser.add_argument('command', nargs='?', 
                       choices=['happy', 'sad', 'angry', 'sleepy', 'doubt', 'neutral', 'rora', 'play', 'pakupaku', 'status', 'help'],
                       help='M5Stackに送信するコマンド')
    parser.add_argument('duration', nargs='?', type=str, help='pakupakuコマンドの時の秒数（オプション）')
    parser.add_argument('--port', '-p', help='シリアルポート (例: /dev/cu.usbserial-XXXXXXXX)')
    parser.add_argument('--interactive', '-i', action='store_true', help='インタラクティブモード')
    
    args = parser.parse_args()
    
    controller = M5StackController(port=args.port)
    
    if args.interactive:
        controller.interactive_mode()
    elif args.command:
        if controller.connect():
            # pakupakuコマンドで引数が指定されている場合
            if args.command == 'pakupaku' and args.duration:
                # 秒数の妥当性チェック
                try:
                    duration = float(args.duration)
                    if 0.1 <= duration <= 60:  # 0.1秒から60秒まで許可
                        command_with_duration = f"pakupaku {duration}"
                        controller.send_command(command_with_duration)
                    else:
                        print("エラー: 秒数は0.1から60の間で指定してください")
                        return
                except ValueError:
                    print("エラー: 秒数は数値で指定してください")
                    return
            else:
                controller.send_command(args.command)
            controller.disconnect()
    else:
        print("使用方法:")
        print("  python3 m5stack_controller.py happy")
        print("  python3 m5stack_controller.py pakupaku 5")
        print("  python3 m5stack_controller.py --interactive")
        print("  python3 m5stack_controller.py --help")

if __name__ == "__main__":
    main()
