# M5Stack Avatar Controller Makefile

# 設定
PYTHON = ./venv/bin/python3
SHELL_SCRIPT = ./m5stack_control.sh
PYTHON_SCRIPT = ./m5stack_controller.py

# デフォルトターゲット
.DEFAULT_GOAL := help

# ヘルプメッセージ
help:
	@echo "M5Stack Avatar Controller"
	@echo ""
	@echo "使用可能なコマンド:"
	@echo "  make setup       - セットアップを実行"
	@echo "  make test        - 接続テスト"
	@echo "  make happy       - 笑顔にする"
	@echo "  make sad         - 悲しい顔にする"
	@echo "  make angry       - 怒った顔にする"
	@echo "  make sleepy      - 眠そうな顔にする"
	@echo "  make doubt       - 困った顔にする"
	@echo "  make neutral     - 普通の顔にする"
	@echo "  make rora        - Roraの特別なキラキラ表情にする ✨"
	@echo "  make play        - 音声再生"
	@echo "  make pakupaku [DURATION=秒数] - ぱくぱく動作（デフォルト3秒）"
	@echo "  make pakupaku-5  - 5秒間ぱくぱく動作"
	@echo "  make status      - ステータス確認"
	@echo "  make interactive - インタラクティブモード"
	@echo "  make upload      - M5Stackにコードをアップロード"
	@echo "  make clean       - 一時ファイルを削除"

# セットアップ
setup:
	@echo "セットアップを実行中..."
	@chmod +x setup.sh
	@./setup.sh

# 接続テスト
test:
	@echo "接続テストを実行中..."
	@$(PYTHON) $(PYTHON_SCRIPT) status

# 表情変更コマンド
happy:
	@$(PYTHON) $(PYTHON_SCRIPT) happy

sad:
	@$(PYTHON) $(PYTHON_SCRIPT) sad

angry:
	@$(PYTHON) $(PYTHON_SCRIPT) angry

sleepy:
	@$(PYTHON) $(PYTHON_SCRIPT) sleepy

doubt:
	@$(PYTHON) $(PYTHON_SCRIPT) doubt

neutral:
	@$(PYTHON) $(PYTHON_SCRIPT) neutral

rora:
	@$(PYTHON) $(PYTHON_SCRIPT) rora

# 音声再生
play:
	@$(PYTHON) $(PYTHON_SCRIPT) play

# ぱくぱく動作（指定秒数口をぱくぱく、デフォルト3秒）
# 使用例: make pakupaku DURATION=5 または make pakupaku-5
pakupaku:
	@$(PYTHON) $(PYTHON_SCRIPT) pakupaku $(DURATION)

# 秒数を直接指定できるターゲット（例：make pakupaku-3, make pakupaku-10）
pakupaku-%:
	@$(PYTHON) $(PYTHON_SCRIPT) pakupaku $*

# ステータス確認
status:
	@$(PYTHON) $(PYTHON_SCRIPT) status

# インタラクティブモード
interactive:
	@$(PYTHON) $(PYTHON_SCRIPT) --interactive

# M5Stackにコードをアップロード（PlatformIOが必要）
upload:
	@echo "M5Stackにコードをアップロード中..."
	@if command -v pio >/dev/null 2>&1; then \
		pio run --target upload --environment m5stack-core2; \
	elif command -v platformio >/dev/null 2>&1; then \
		platformio run --target upload --environment m5stack-core2; \
	elif [ -f ~/.platformio/penv/bin/pio ]; then \
		~/.platformio/penv/bin/pio run --target upload --environment m5stack-core2; \
	else \
		echo "PlatformIOが見つかりません。手動でアップロードしてください。"; \
	fi

# ビルド（M5Stack側）
build:
	@echo "M5Stackコードをビルド中..."
	@if command -v pio >/dev/null 2>&1; then \
		pio run --environment m5stack-core2; \
	elif command -v platformio >/dev/null 2>&1; then \
		platformio run --environment m5stack-core2; \
	elif [ -f ~/.platformio/penv/bin/pio ]; then \
		~/.platformio/penv/bin/pio run --environment m5stack-core2; \
	else \
		echo "PlatformIOが見つかりません。手動でビルドしてください。"; \
	fi

# シリアルポートを検索
find-port:
	@echo "利用可能なシリアルポート:"
	@ls /dev/cu.* 2>/dev/null | grep -E "(usb|serial)" || echo "USBシリアルポートが見つかりません"

# 一時ファイルの削除
clean:
	@echo "一時ファイルを削除中..."
	@find . -name "*.pyc" -delete
	@find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
	@rm -rf .pio/build/*/

# シェルスクリプト版のコマンド（バックアップ）
shell-happy:
	@$(SHELL_SCRIPT) happy

shell-sad:
	@$(SHELL_SCRIPT) sad

shell-rora:
	@$(SHELL_SCRIPT) rora

shell-status:
	@$(SHELL_SCRIPT) status

# デモ実行
demo:
	@echo "M5Stack Avatar デモを開始します..."
	@echo "5秒間隔で表情を変更します。Ctrl+Cで停止してください。"
	@$(PYTHON) $(PYTHON_SCRIPT) neutral
	@sleep 2
	@$(PYTHON) $(PYTHON_SCRIPT) happy
	@sleep 3
	@$(PYTHON) $(PYTHON_SCRIPT) sad
	@sleep 3
	@$(PYTHON) $(PYTHON_SCRIPT) angry
	@sleep 3
	@$(PYTHON) $(PYTHON_SCRIPT) sleepy
	@sleep 3
	@$(PYTHON) $(PYTHON_SCRIPT) doubt
	@sleep 3
	@$(PYTHON) $(PYTHON_SCRIPT) rora
	@sleep 3
	@$(PYTHON) $(PYTHON_SCRIPT) pakupaku
	@sleep 3
	@$(PYTHON) $(PYTHON_SCRIPT) neutral
	@echo "デモ完了"

.PHONY: help setup test happy sad angry sleepy doubt neutral rora play pakupaku status interactive upload build find-port clean shell-happy shell-sad shell-rora shell-status demo
