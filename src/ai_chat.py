import sys
import os

try:
    import google.generativeai as genai
except ImportError:
    print("Error: AI module missing. Please run: pip install google-generativeai")
    sys.exit(1)

# Get API Key from environment variable
API_KEY = os.getenv("GEMINI_API_KEY")

def get_ai_response(prompt):
    if not API_KEY:
        return "I need an API key to think. Please set the GEMINI_API_KEY environment variable."

    try:
        genai.configure(api_key=API_KEY)
        # Use gemini-pro for text
        model = genai.GenerativeModel('gemini-pro')
        
        # Context instruction for the AI
        system_instruction = "You are JARVIS, a helpful voice assistant. Keep your answer concise, under 30 words, and conversational. Do not use markdown formatting like asterisks."
        full_prompt = f"{system_instruction}\nUser: {prompt}\nJARVIS:"
        
        response = model.generate_content(full_prompt)
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