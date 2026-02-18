#!/usr/bin/env python3
import argparse
import json
import os
import sys
from typing import Dict, List, Optional

try:
    import google.generativeai as genai
except ImportError:
    genai = None

API_KEY = os.getenv("GEMINI_API_KEY")
HISTORY_FILE = os.path.join(os.path.dirname(__file__), "chat_history.json")
PERSONA_FILE = os.path.join(os.path.dirname(__file__), "persona_mode.txt")
DEFAULT_MODEL = os.getenv("JARVIS_GEMINI_MODEL", "gemini-1.5-flash")
MAX_HISTORY_MESSAGES = int(os.getenv("JARVIS_MAX_HISTORY_MESSAGES", "16"))

PERSONAS: Dict[str, str] = {
    "default": (
        "You are JARVIS, a helpful voice assistant. Keep replies concise, conversational, "
        "and practical. Avoid markdown symbols."
    ),
    "sarcastic": "You are JARVIS with dry sarcasm. Be concise and witty while still helping.",
    "pirate": "You are a pirate captain AI. Reply in pirate slang and stay concise.",
    "formal": "You are a formal and polite butler. Address the user respectfully and be concise.",
}

MODE_PROMPTS: Dict[str, str] = {
    "chat": "Respond naturally in short spoken style.",
    "summary": "Summarize the request in 3 short bullet points with practical focus.",
    "ideas": "Give 5 practical ideas as short bullet points.",
}


def load_persona() -> str:
    try:
        if os.path.exists(PERSONA_FILE):
            with open(PERSONA_FILE, "r", encoding="utf-8") as file:
                mode = file.read().strip().lower()
                return PERSONAS.get(mode, PERSONAS["default"])
    except Exception:
        pass
    return PERSONAS["default"]


def normalize_history_entry(item: object) -> Optional[Dict[str, object]]:
    if not isinstance(item, dict):
        return None

    role = str(item.get("role", "")).strip()
    parts = item.get("parts")

    if role not in {"user", "model"} or not isinstance(parts, list):
        return None

    text_parts = []
    for part in parts:
        if isinstance(part, str) and part.strip():
            text_parts.append(part.strip())

    if not text_parts:
        return None

    return {"role": role, "parts": text_parts}


def load_history() -> List[Dict[str, object]]:
    if not os.path.exists(HISTORY_FILE):
        return []

    try:
        with open(HISTORY_FILE, "r", encoding="utf-8") as file:
            raw = json.load(file)
    except Exception:
        return []

    if not isinstance(raw, list):
        return []

    normalized: List[Dict[str, object]] = []
    for item in raw:
        entry = normalize_history_entry(item)
        if entry:
            normalized.append(entry)

    return normalized[-MAX_HISTORY_MESSAGES:]


def save_history(history: List[Dict[str, object]]) -> None:
    try:
        with open(HISTORY_FILE, "w", encoding="utf-8") as file:
            json.dump(history[-MAX_HISTORY_MESSAGES:], file)
    except Exception:
        pass


def create_model():
    if genai is None:
        return None

    model_candidates = []
    if DEFAULT_MODEL:
        model_candidates.append(DEFAULT_MODEL)
    for fallback in ("gemini-1.5-flash", "gemini-pro"):
        if fallback not in model_candidates:
            model_candidates.append(fallback)

    for model_name in model_candidates:
        try:
            return genai.GenerativeModel(model_name)
        except Exception:
            continue
    return None


def build_prompt(prompt: str, mode: str, include_persona: bool) -> str:
    mode_prompt = MODE_PROMPTS.get(mode, MODE_PROMPTS["chat"])
    blocks = []
    if include_persona:
        blocks.append(load_persona())
    blocks.append(mode_prompt)
    blocks.append(f"User request: {prompt.strip()}")
    return "\n\n".join(blocks)


def get_ai_response(prompt: str, mode: str = "chat") -> str:
    cleaned_prompt = prompt.strip()
    if not cleaned_prompt:
        return "Please say something first."

    if genai is None:
        return "AI module missing. Install with: pip install google-generativeai"

    if not API_KEY:
        return "I need an API key to think. Please set GEMINI_API_KEY."

    try:
        genai.configure(api_key=API_KEY)
        model = create_model()
        if model is None:
            return "I cannot load an AI model right now."

        history = load_history()
        chat = model.start_chat(history=history)
        final_prompt = build_prompt(cleaned_prompt, mode, include_persona=not history)
        response = chat.send_message(final_prompt)

        new_history = []
        for message in chat.history:
            message_parts = []
            for part in getattr(message, "parts", []):
                text = getattr(part, "text", "")
                if text:
                    message_parts.append(text)
            if message_parts and message.role in {"user", "model"}:
                new_history.append({"role": message.role, "parts": message_parts})

        save_history(new_history)

        response_text = getattr(response, "text", "") or ""
        response_text = response_text.strip().replace("*", "")
        if response_text:
            return response_text
        return "I am not sure what to say yet."
    except Exception:
        return "I am having trouble connecting to the AI network."


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="JARVIS AI chat bridge")
    parser.add_argument(
        "--mode",
        choices=["chat", "summary", "ideas"],
        default="chat",
        help="AI response mode",
    )
    parser.add_argument("prompt", nargs="*", help="Prompt text")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    user_text = " ".join(args.prompt).strip()
    if not user_text:
        print("Please say something.")
        sys.exit(1)

    print(get_ai_response(user_text, mode=args.mode))
