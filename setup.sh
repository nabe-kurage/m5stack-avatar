#!/bin/bash
# M5Stack Avatar Controller セットアップスクリプト

echo "=== M5Stack Avatar Controller セットアップ ==="

# スクリプトに実行権限を付与
echo "スクリプトファイルに実行権限を付与中..."
chmod +x m5stack_control.sh
chmod +x m5stack_controller.py

# Pythonの依存関係をチェック
echo "Python依存関係をチェック中..."

# pyserialがインストールされているかチェック
python3 -c "import serial" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "pyserialがインストールされていません。インストールしますか? (y/N)"
    read -r response
    if [[ "$response" =~ ^[Yy]$ ]]; then
        echo "pyserialをインストール中..."
        pip3 install pyserial
        if [ $? -eq 0 ]; then
            echo "✅ pyserialのインストールが完了しました"
        else
            echo "❌ pyserialのインストールに失敗しました"
            exit 1
        fi
    fi
fi

# シンボリックリンクの作成（オプション）
echo ""
echo "システム全体からアクセスできるようにしますか? (y/N)"
echo "(これにより、どのディレクトリからでもm5stack-controlコマンドが使えるようになります)"
read -r response

if [[ "$response" =~ ^[Yy]$ ]]; then
    LINK_PATH="/usr/local/bin/m5stack-control"
    if [ -L "$LINK_PATH" ]; then
        echo "既存のシンボリックリンクを削除中..."
        sudo rm "$LINK_PATH"
    fi
    
    echo "シンボリックリンクを作成中... (管理者権限が必要です)"
    sudo ln -s "$(pwd)/m5stack_control.sh" "$LINK_PATH"
    
    if [ $? -eq 0 ]; then
        echo "✅ シンボリックリンクが作成されました: $LINK_PATH"
        echo "   どこからでも 'm5stack-control happy' のように使用できます"
    else
        echo "❌ シンボリックリンクの作成に失敗しました"
    fi
fi

echo ""
echo "=== セットアップ完了 ==="
echo ""
echo "使用方法:"
echo "  # Pythonスクリプト（推奨）"
echo "  python3 m5stack_controller.py happy"
echo "  python3 m5stack_controller.py --interactive"
echo ""
echo "  # シェルスクリプト（シンプル）"
echo "  ./m5stack_control.sh happy"
echo "  ./m5stack_control.sh status"
echo ""

if [[ "$response" =~ ^[Yy]$ ]]; then
    echo "  # システムコマンド"
    echo "  m5stack-control happy"
    echo "  m5stack-control sad"
fi

echo ""
echo "まず、M5StackをUSB Type-Cケーブルでこのマックに接続してください。"
echo "次に、M5Stackに更新されたコードをアップロードしてください。"
