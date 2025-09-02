# M5Stack Avatar Mac制御システム

MacからUSB Type-C経由でM5Stack Avatarの表情を制御するシステムです。

## 🚀 クイックスタート

### 1. セットアップ
```bash
# セットアップスクリプトを実行
chmod +x setup.sh
./setup.sh

# または、Makefileを使用
make setup
```

### 2. M5Stackに新しいコードをアップロード
```bash
# PlatformIOを使用
make upload

# 手動でアップロード（VSCode PlatformIO拡張機能使用）
# 1. VSCodeでプロジェクトを開く
# 2. src/main.cppが更新されていることを確認
# 3. PlatformIO: Upload を実行
```

### 3. 基本的な使用方法
```bash
# 笑顔にする
make happy
# または
python3 m5stack_controller.py happy

# 悲しい顔にする
make sad

# ステータス確認
make status
```

## 📱 利用可能なコマンド

| コマンド | 動作 | 例 |
|----------|------|-----|
| `happy` | 😊 笑顔 | `make happy` |
| `sad` | 😢 悲しい顔 | `make sad` |
| `angry` | 😠 怒った顔 | `make angry` |
| `sleepy` | 😴 眠そうな顔 | `make sleepy` |
| `doubt` | 🤔 困った顔 | `make doubt` |
| `neutral` | 😐 普通の顔 | `make neutral` |
| `rora` | ✨ Roraの特別なキラキラ表情 | `make rora` |
| `play` | 🎵 音声再生 | `make play` |
| `status` | ℹ️ ステータス確認 | `make status` |

## 🛠️ 使用方法

### 方法1: Makefileを使用（推奨）
```bash
# セットアップ
make setup

# 基本操作
make happy
make sad
make status

# インタラクティブモード
make interactive

# デモ実行
make demo
```

### 方法2: Pythonスクリプトを直接使用
```bash
# 基本的な使い方
python3 m5stack_controller.py happy
python3 m5stack_controller.py sad

# インタラクティブモード（連続してコマンドを入力）
python3 m5stack_controller.py --interactive

# 特定のポートを指定
python3 m5stack_controller.py --port /dev/cu.usbserial-XXXXXXXX happy
```

### 方法3: シェルスクリプトを使用
```bash
# 基本的な使い方
./m5stack_control.sh happy
./m5stack_control.sh status

# ポートを指定
./m5stack_control.sh sad /dev/cu.usbserial-XXXXXXXX
```

## 🔧 トラブルシューティング

### M5Stackが見つからない場合
```bash
# 利用可能なポートを確認
make find-port

# または
ls /dev/cu.* | grep usb
```

### 接続テスト
```bash
# 接続テストを実行
make test
```

### よくある問題

1. **"Permission denied"エラー**
   ```bash
   # スクリプトに実行権限を付与
   chmod +x m5stack_controller.py
   chmod +x m5stack_control.sh
   ```

2. **"No module named 'serial'"エラー**
   ```bash
   # pyserialをインストール
   pip3 install pyserial
   ```

3. **"Port not found"エラー**
   - M5StackがUSB Type-Cケーブルで正しく接続されているか確認
   - ケーブルがデータ転送対応か確認（充電専用ではダメ）
   - `make find-port`でポートを確認

## 📁 ファイル構成

```
m5stack-avatar/
├── src/main.cpp              # M5Stack側のコード（更新済み）
├── m5stack_controller.py     # Python制御スクリプト
├── m5stack_control.sh        # シェル制御スクリプト
├── setup.sh                  # セットアップスクリプト
├── Makefile                  # ビルド・実行用Makefile
├── README_MAC_CONTROL.md     # この説明書
└── platformio.ini           # PlatformIO設定
```

## 🎯 高度な使用方法

### インタラクティブモード
```bash
make interactive
```
このモードでは、連続してコマンドを入力できます：
```
コマンドを入力: happy
コマンドを入力: sad  
コマンドを入力: play
コマンドを入力: quit
```

### デモモード
```bash
make demo
```
自動的に複数の表情を順番に表示します。

### 環境変数での設定
```bash
# デフォルトのシリアルポートを設定
export M5STACK_PORT=/dev/cu.usbserial-XXXXXXXX

# 以降、ポートを指定せずに使用可能
./m5stack_control.sh happy
```

## 🔄 更新履歴

- シリアル通信によるリアルタイム表情制御を実装
- Python/Shell両方での制御スクリプトを提供
- Makefileによる簡単なコマンド実行
- 自動ポート検出機能
- インタラクティブモード対応

## 🆘 サポート

問題が発生した場合は、以下を試してください：

1. **基本チェック**
   ```bash
   make status  # 接続状況確認
   make test    # 接続テスト
   ```

2. **詳細ログの確認**
   ```bash
   python3 m5stack_controller.py --interactive
   # レスポンスを詳細に確認できます
   ```

3. **手動でのシリアル接続テスト**
   ```bash
   screen /dev/cu.usbserial-XXXXXXXX 115200
   # 接続後、happy と入力してEnterキーを押す
   ```

これで、MacからM5Stack Avatarを完全に制御できます！🎉
