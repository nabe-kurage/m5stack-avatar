#!/usr/bin/env python3
"""
M5Stack Face MCP Server
M5Stackの表情制御を行うためのMCPサーバー
"""

import asyncio
import json
import logging
import sys
import os
from typing import Any, Dict, List, Optional
from dotenv import load_dotenv

# M5StackControllerをインポート
from m5stack_controller import M5StackController

# .envファイルを読み込み
load_dotenv()

# ログ設定
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# グローバル変数
m5stack_controller: Optional[M5StackController] = None


def get_m5stack_controller() -> M5StackController:
    """M5StackControllerを取得"""
    global m5stack_controller
    if m5stack_controller is None:
        port = os.getenv("M5STACK_PORT", None)
        m5stack_controller = M5StackController(port=port)
    return m5stack_controller


async def handle_initialize(params: Dict[str, Any]) -> Dict[str, Any]:
    """初期化処理"""
    return {
        "protocolVersion": "2024-11-05",
        "capabilities": {
            "tools": {}
        },
        "serverInfo": {
            "name": "m5stack-face-mcp",
            "version": "1.0.0"
        }
    }


async def handle_tools_list(params: Dict[str, Any]) -> Dict[str, Any]:
    """ツール一覧を返す"""
    tools = [
        {
            "name": "face_happy",
            "description": "M5Stackを笑顔にする。使用場面：喜ばしいニュースを伝える時、成功を祝う時、楽しい会話をする時、ポジティブな反応を示す時、ユーザーを励ます時。",
            "inputSchema": {
                "type": "object",
                "properties": {},
                "required": []
            }
        },
        {
            "name": "face_sad",
            "description": "M5Stackを悲しい顔にする。使用場面：悲しいニュースを伝える時、失敗や問題を報告する時、同情や共感を表現する時、謝罪する時、残念な結果を示す時。",
            "inputSchema": {
                "type": "object",
                "properties": {},
                "required": []
            }
        },
        {
            "name": "face_angry",
            "description": "M5Stackを怒った顔にする。使用場面：エラーや警告を伝える時、重要な問題を強調する時、緊急性を表現する時、ユーザーの注意を引く必要がある時。使用は控えめに。",
            "inputSchema": {
                "type": "object",
                "properties": {},
                "required": []
            }
        },
        {
            "name": "face_sleepy",
            "description": "M5Stackを眠そうな顔にする。使用場面：長時間の作業後、待機状態の時、リラックスした雰囲気を作る時、夜間の操作時、処理中で少し退屈な時。",
            "inputSchema": {
                "type": "object",
                "properties": {},
                "required": []
            }
        },
        {
            "name": "face_doubt",
            "description": "M5Stackを困った顔にする。使用場面：不明な状況や曖昧な質問に対して、選択肢を提示する時、問題解決を考えている時、ユーザーの意図が不明な時、複雑な状況を表現する時。",
            "inputSchema": {
                "type": "object",
                "properties": {},
                "required": []
            }
        },
        {
            "name": "face_neutral",
            "description": "M5Stackを普通の顔にする。使用場面：標準状態に戻す時、客観的な情報を伝える時、会話の開始・終了時、他の表情から切り替える時。最も汎用的な表情。",
            "inputSchema": {
                "type": "object",
                "properties": {},
                "required": []
            }
        },
        {
            "name": "face_shock",
            "description": "M5Stackを大ショックの表情にする 😱 使用場面：驚きや衝撃的な情報を受けた時、予想外の出来事に遭遇した時、大きなミスや失敗に気づいた時、驚愕すべき事実を知った時。",
            "inputSchema": {
                "type": "object",
                "properties": {},
                "required": []
            }
        },
        {
            "name": "face_pakupaku",
            "description": "M5Stackの口をぱくぱく動かす。使用場面：話している様子を表現する時、説明や会話の最中、アクティブな状態を示す時。他の表情と組み合わせ可能（例：悲しい顔でぱくぱく＝悲しいことを話している）。",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "duration": {
                        "type": "number",
                        "description": "ぱくぱく動作の秒数（0.1-60秒、デフォルト3秒）。短い返答：1-3秒、普通の会話：3-5秒、長い説明：5-10秒",
                        "minimum": 0.1,
                        "maximum": 60,
                        "default": 3
                    }
                },
                "required": []
            }
        },
        {
            "name": "face_play",
            "description": "M5Stackで音声を再生する。使用場面：音声メッセージがある時、重要な通知を音で知らせる時、ユーザーの注意を音で引く時。",
            "inputSchema": {
                "type": "object",
                "properties": {},
                "required": []
            }
        },
        {
            "name": "face_status",
            "description": "M5Stackのステータスを取得する。使用場面：デバッグ時、システムの状態を確認したい時、接続状況を調べる時、トラブルシューティング時。",
            "inputSchema": {
                "type": "object",
                "properties": {},
                "required": []
            }
        }
    ]
    return {"tools": tools}


async def handle_tools_call(name: str, arguments: Dict[str, Any]) -> Dict[str, Any]:
    """ツール呼び出しを処理"""
    try:
        controller = get_m5stack_controller()

        # 表情制御コマンド
        if name in ["face_happy", "face_sad", "face_angry", "face_sleepy", "face_doubt", "face_neutral", "face_shock", "face_play", "face_status"]:
            # コマンド名から実際のコマンドを取得（face_ プレフィックスを削除）
            command = name.replace("face_", "")
            
            if controller.connect():
                success = controller.send_command(command)
                controller.disconnect()
                
                if success:
                    return {
                        "content": [
                            {
                                "type": "text",
                                "text": f"✅ M5Stack表情制御が正常に実行されました: {command}"
                            }
                        ]
                    }
                else:
                    return {
                        "content": [
                            {
                                "type": "text",
                                "text": f"❌ M5Stack表情制御でエラーが発生しました: {command}"
                            }
                        ]
                    }
            else:
                return {
                    "content": [
                        {
                            "type": "text",
                            "text": "❌ M5Stackとの接続に失敗しました"
                        }
                    ]
                }

        # pakupaku コマンドの特別処理
        elif name == "face_pakupaku":
            duration = arguments.get("duration", 3.0)
            
            # 秒数の妥当性チェック
            if not (0.1 <= duration <= 60):
                return {
                    "content": [
                        {
                            "type": "text",
                            "text": "❌ エラー: 秒数は0.1から60の間で指定してください"
                        }
                    ]
                }
            
            command = f"pakupaku {duration}"
            
            if controller.connect():
                success = controller.send_command(command)
                controller.disconnect()
                
                if success:
                    return {
                        "content": [
                            {
                                "type": "text",
                                "text": f"✅ M5Stackが{duration}秒間ぱくぱく動作を実行しました 🗣️"
                            }
                        ]
                    }
                else:
                    return {
                        "content": [
                            {
                                "type": "text",
                                "text": f"❌ ぱくぱく動作でエラーが発生しました"
                            }
                        ]
                    }
            else:
                return {
                    "content": [
                        {
                            "type": "text",
                            "text": "❌ M5Stackとの接続に失敗しました"
                        }
                    ]
                }

        else:
            return {
                "content": [
                    {
                        "type": "text",
                        "text": f"❌ 不明なツール: {name}"
                    }
                ]
            }

    except Exception as e:
        logger.error(f"Error executing tool {name}: {e}")
        return {
            "content": [
                {
                    "type": "text",
                    "text": f"❌ エラー: {str(e)}"
                }
            ]
        }


async def main():
    """メイン関数"""
    while True:
        try:
            line = await asyncio.get_event_loop().run_in_executor(None, sys.stdin.readline)
            if not line:
                break

            try:
                message = json.loads(line.strip())
            except json.JSONDecodeError:
                continue

            if message.get("method") == "initialize":
                result = await handle_initialize(message.get("params", {}))
                response = {
                    "jsonrpc": "2.0",
                    "id": message.get("id"),
                    "result": result
                }
                print(json.dumps(response))
                sys.stdout.flush()

            elif message.get("method") == "tools/list":
                result = await handle_tools_list(message.get("params", {}))
                response = {
                    "jsonrpc": "2.0",
                    "id": message.get("id"),
                    "result": result
                }
                print(json.dumps(response))
                sys.stdout.flush()

            elif message.get("method") == "tools/call":
                params = message.get("params", {})
                result = await handle_tools_call(params.get("name"), params.get("arguments", {}))
                response = {
                    "jsonrpc": "2.0",
                    "id": message.get("id"),
                    "result": result
                }
                print(json.dumps(response))
                sys.stdout.flush()

        except Exception as e:
            logger.error(f"Error processing message: {e}")


if __name__ == "__main__":
    asyncio.run(main())