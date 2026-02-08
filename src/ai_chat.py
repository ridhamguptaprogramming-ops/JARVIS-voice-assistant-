import sys
import os
import json

try:
    import google.generativeai as genai
except ImportError:
    print("Error: AI module missing. Please run: pip install google-generativeai")
    sys.exit(1)

# Get API Key from environment variable
API_KEY = os.getenv("GEMINI_API_KEY")
HISTORY_FILE = os.path.join(os.path.dirname(__file__), "chat_history.json")
PERSONA_FILE = os.path.join(os.path.dirname(__file__), "persona_mode.txt")

PERSONAS = {
    "default": "You are JARVIS, a helpful voice assistant. Keep your answer concise, under 30 words, and conversational. Do not use markdown formatting like asterisks.",
    "sarcastic": "You are JARVIS, but you are extremely sarcastic and dry. You answer the question but make a snarky comment about it. Keep it under 30 words.",
    "pirate": "You are a pirate captain AI. Answer questions with pirate slang (Ahoy, Matey!). Keep it under 30 words.",
    "formal": "You are a very formal and polite butler. Use 'Sir' and sophisticated language. Keep it under 30 words."
}

def load_persona():
    try:
        if os.path.exists(PERSONA_FILE):
            with open(PERSONA_FILE, "r") as f:
                mode = f.read().strip().lower()
                return PERSONAS.get(mode, PERSONAS["default"])
    except:
        pass
    return PERSONAS["default"]

def load_history():
    if os.path.exists(HISTORY_FILE):
        try:
            with open(HISTORY_FILE, "r") as f:
                return json.load(f)
        except:
            return []
    return []

def save_history(history):
    try:
        with open(HISTORY_FILE, "w") as f:
            json.dump(history, f)
    except:
        pass

def get_ai_response(prompt):
    if not API_KEY:
        return "I need an API key to think. Please set the GEMINI_API_KEY environment variable."

    try:
        genai.configure(api_key=API_KEY)
        # Use gemini-pro for text
        model = genai.GenerativeModel('gemini-pro')
        
        history = load_history()
        chat = model.start_chat(history=history)
        
        # Context instruction for the AI
        system_instruction = load_persona()
        
        final_prompt = prompt
        if not history:
            final_prompt = f"{system_instruction}\n\nUser: {prompt}"
        
        response = chat.send_message(final_prompt)
        
        new_history = []
        for message in chat.history:
            new_history.append({"role": message.role, "parts": [part.text for part in message.parts]})
            
        save_history(new_history)
        
        if response.text:
            return response.text.strip().replace("*", "")
        return "I am not sure what to say."
    except Exception as e:
        return "I am having trouble connecting to the AI network."

if __name__ == "__main__":
    if len(sys.argv) > 1:
        user_text = " ".join(sys.argv[1:])
        print(get_ai_response(user_text))
    else:
        print("Please say something.")