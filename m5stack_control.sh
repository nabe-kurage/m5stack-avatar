#!/bin/bash
# M5Stack Avatar Controller - Shell Script Version
# USB Type-C経由でM5Stackの表情を制御

# 設定
BAUD_RATE=115200
DEFAULT_PORT=""

# M5Stackのポートを自動検出
find_m5stack_port() {
    echo "M5Stackのシリアルポートを検索中..."
    
    # macOSでよくあるUSBシリアルポートのパターンを検索
    for port in /dev/cu.usbserial* /dev/cu.wchusbserial* /dev/cu.SLAB_USBtoUART*; do
        if [[ -e "$port" ]]; then
            echo "見つかったポート: $port"
            echo "$port"
            return 0
        fi
    done
    
    echo "M5Stackのポートが見つかりません。"
    echo "利用可能なポート:"
    ls /dev/cu.* 2>/dev/null | head -10
    return 1
}

# コマンドを送信
send_command() {
    local command="$1"
    local port="$2"
    
    if [[ -z "$port" ]]; then
        port=$(find_m5stack_port)
        if [[ $? -ne 0 ]]; then
            echo "エラー: シリアルポートが見つかりません"
            exit 1
        fi
    fi
    
    echo "送信: $command → $port"
    
    # コマンドを送信
    echo "$command" > "$port"
    
    echo "コマンドを送信しました"
}

# ヘルプメッセージ
show_help() {
    cat << EOF
M5Stack Avatar Controller - Shell Script

使用方法:
    $0 <command> [port]
    
利用可能なコマンド:
    happy    - 笑顔にする
    sad      - 悲しい顔にする
    angry    - 怒った顔にする
    sleepy   - 眠そうな顔にする
    doubt    - 困った顔にする
    neutral  - 普通の顔にする
    rora     - Roraの特別なキラキラ表情にする ✨
    play     - 音声再生
    status   - ステータス確認
    help     - ヘルプ表示
    
例:
    $0 happy
    $0 sad /dev/cu.usbserial-XXXXXXXX
    $0 status
    
環境変数:
    M5STACK_PORT - デフォルトのシリアルポート
EOF
}

# メイン処理
main() {
    local command="$1"
    local port="${2:-$M5STACK_PORT}"
    
    case "$command" in
        happy|sad|angry|sleepy|doubt|neutral|rora|play|status|help)
            send_command "$command" "$port"
            ;;
        ""|--help|-h)
            show_help
            ;;
        --find-port)
            find_m5stack_port
            ;;
        *)
            echo "エラー: 不明なコマンド '$command'"
            echo "利用可能なコマンドを確認するには: $0 --help"
            exit 1
            ;;
    esac
}

# スクリプト実行
main "$@"
