#!/usr/bin/env python3
"""
JARVIS Desktop UI
- AI chat (text + voice)
- Built-in AI features: summary and ideas
- Launches CLI Jarvis in a new terminal window
"""

import os
import platform
import subprocess
import threading
import tkinter as tk
from tkinter import messagebox, scrolledtext

from ai_chat import get_ai_response
from speech_recognizer import recognize_speech


class JarvisUI:
    def __init__(self, root: tk.Tk) -> None:
        self.root = root
        self.root.title("JARVIS AI Voice Console")
        self.root.geometry("960x680")
        self.root.minsize(860, 560)
        self.root.configure(bg="#f2f5f9")

        self.voice_output_enabled = tk.BooleanVar(value=True)
        self.last_user_text = ""

        self._build_layout()
        self._set_status("Ready")

    def _build_layout(self) -> None:
        header = tk.Frame(self.root, bg="#0e4a7a", height=82)
        header.pack(fill=tk.X)
        header.pack_propagate(False)

        title = tk.Label(
            header,
            text="JARVIS",
            fg="#ffffff",
            bg="#0e4a7a",
            font=("Avenir Next", 28, "bold"),
        )
        title.pack(side=tk.LEFT, padx=20, pady=16)

        subtitle = tk.Label(
            header,
            text="Voice Assistant UI",
            fg="#d8ecff",
            bg="#0e4a7a",
            font=("Avenir Next", 14),
        )
        subtitle.pack(side=tk.LEFT, pady=28)

        self.status_label = tk.Label(
            header,
            text="Ready",
            fg="#ffffff",
            bg="#f06c2f",
            font=("Avenir Next", 11, "bold"),
            padx=14,
            pady=8,
        )
        self.status_label.pack(side=tk.RIGHT, padx=20, pady=18)

        body = tk.Frame(self.root, bg="#f2f5f9")
        body.pack(fill=tk.BOTH, expand=True, padx=16, pady=16)

        self.chat_box = scrolledtext.ScrolledText(
            body,
            wrap=tk.WORD,
            bg="#ffffff",
            fg="#102a43",
            font=("Menlo", 12),
            relief=tk.FLAT,
            padx=12,
            pady=12,
            state=tk.DISABLED,
        )
        self.chat_box.pack(fill=tk.BOTH, expand=True)

        controls = tk.Frame(body, bg="#f2f5f9")
        controls.pack(fill=tk.X, pady=(12, 0))

        self.input_entry = tk.Entry(
            controls,
            font=("Avenir Next", 13),
            bg="#ffffff",
            fg="#102a43",
            relief=tk.FLAT,
        )
        self.input_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, ipady=10, padx=(0, 10))
        self.input_entry.bind("<Return>", self._on_send_pressed)

        self.send_button = tk.Button(
            controls,
            text="Send",
            command=self.send_text_prompt,
            bg="#0e4a7a",
            fg="#ffffff",
            activebackground="#0a3556",
            activeforeground="#ffffff",
            relief=tk.FLAT,
            font=("Avenir Next", 12, "bold"),
            padx=20,
            pady=8,
            cursor="hand2",
        )
        self.send_button.pack(side=tk.LEFT)

        actions = tk.Frame(body, bg="#f2f5f9")
        actions.pack(fill=tk.X, pady=(12, 4))

        self.voice_button = self._make_action_button(actions, "Voice Chat", self.capture_voice_prompt)
        self.voice_button.pack(side=tk.LEFT, padx=(0, 8))

        self.summary_button = self._make_action_button(actions, "AI Summary", self.run_summary_mode)
        self.summary_button.pack(side=tk.LEFT, padx=(0, 8))

        self.ideas_button = self._make_action_button(actions, "AI Ideas", self.run_ideas_mode)
        self.ideas_button.pack(side=tk.LEFT, padx=(0, 8))

        self.plan_button = self._make_action_button(actions, "AI Plan", self.run_plan_mode)
        self.plan_button.pack(side=tk.LEFT, padx=(0, 8))

        self.launch_button = self._make_action_button(actions, "Run Jarvis CLI", self.launch_cli_jarvis)
        self.launch_button.pack(side=tk.LEFT, padx=(0, 8))

        self.clear_button = self._make_action_button(actions, "Clear Chat", self.clear_chat)
        self.clear_button.pack(side=tk.LEFT)

        options = tk.Frame(body, bg="#f2f5f9")
        options.pack(fill=tk.X)
        tk.Checkbutton(
            options,
            text="Speak AI replies",
            variable=self.voice_output_enabled,
            bg="#f2f5f9",
            fg="#334e68",
            activebackground="#f2f5f9",
            font=("Avenir Next", 11),
            selectcolor="#ffffff",
        ).pack(side=tk.LEFT, pady=4)

        self._append_message("JARVIS", "UI online. You can type or use Voice Chat.")

    def _make_action_button(self, parent: tk.Widget, text: str, command) -> tk.Button:
        return tk.Button(
            parent,
            text=text,
            command=command,
            bg="#f06c2f",
            fg="#ffffff",
            activebackground="#cb5824",
            activeforeground="#ffffff",
            relief=tk.FLAT,
            font=("Avenir Next", 11, "bold"),
            padx=14,
            pady=8,
            cursor="hand2",
        )

    def _set_status(self, text: str) -> None:
        self.status_label.config(text=text)

    def _append_message(self, sender: str, text: str) -> None:
        self.chat_box.config(state=tk.NORMAL)
        self.chat_box.insert(tk.END, f"{sender}: {text}\n\n")
        self.chat_box.see(tk.END)
        self.chat_box.config(state=tk.DISABLED)

    def _on_send_pressed(self, _event) -> None:
        self.send_text_prompt()

    def _current_prompt(self) -> str:
        prompt = self.input_entry.get().strip()
        if prompt:
            return prompt
        return self.last_user_text.strip()

    def send_text_prompt(self) -> None:
        prompt = self.input_entry.get().strip()
        if not prompt:
            return
        self.input_entry.delete(0, tk.END)
        self._submit_prompt(prompt, mode="chat", source="You")

    def capture_voice_prompt(self) -> None:
        self._set_status("Listening")
        threading.Thread(target=self._capture_voice_worker, daemon=True).start()

    def _capture_voice_worker(self) -> None:
        spoken_text = recognize_speech()
        if not spoken_text:
            self.root.after(0, lambda: self._set_status("Mic timeout"))
            self.root.after(0, lambda: self._append_message("JARVIS", "I could not hear a clear voice command."))
            return

        self.root.after(0, lambda: self._submit_prompt(spoken_text, mode="chat", source="Voice"))

    def run_summary_mode(self) -> None:
        prompt = self._current_prompt()
        if not prompt:
            messagebox.showinfo("Summary", "Type or speak something first.")
            return
        self._submit_prompt(prompt, mode="summary", source="You")

    def run_ideas_mode(self) -> None:
        prompt = self._current_prompt()
        if not prompt:
            messagebox.showinfo("Ideas", "Type or speak a topic first.")
            return
        self._submit_prompt(prompt, mode="ideas", source="You")

    def run_plan_mode(self) -> None:
        prompt = self._current_prompt()
        if not prompt:
            messagebox.showinfo("Plan", "Type or speak a goal first.")
            return
        self._submit_prompt(prompt, mode="plan", source="You")

    def _submit_prompt(self, prompt: str, mode: str, source: str) -> None:
        self.last_user_text = prompt
        self._append_message(source, prompt)
        self._set_status("Thinking")
        threading.Thread(target=self._ai_worker, args=(prompt, mode), daemon=True).start()

    def _ai_worker(self, prompt: str, mode: str) -> None:
        reply = get_ai_response(prompt, mode=mode)
        self.root.after(0, lambda: self._append_message("JARVIS", reply))
        self.root.after(0, lambda: self._set_status("Ready"))
        if self.voice_output_enabled.get():
            self._speak_text(reply)

    def _speak_text(self, text: str) -> None:
        if not text.strip():
            return
        try:
            system_name = platform.system().lower()
            if system_name == "darwin":
                subprocess.Popen(["say", "-v", "Alex", text], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            elif system_name == "linux":
                subprocess.Popen(["espeak", text], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except Exception:
            pass

    def launch_cli_jarvis(self) -> None:
        script_path = os.path.join(os.path.dirname(__file__), "..", "scripts", "run_jarvis_gui.sh")
        script_path = os.path.abspath(script_path)
        try:
            subprocess.Popen(["bash", script_path], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            self._append_message("JARVIS", "Launching CLI Jarvis in a new terminal window.")
        except Exception as exc:
            self._append_message("JARVIS", f"Could not launch CLI Jarvis: {exc}")

    def clear_chat(self) -> None:
        self.chat_box.config(state=tk.NORMAL)
        self.chat_box.delete("1.0", tk.END)
        self.chat_box.config(state=tk.DISABLED)
        self._append_message("JARVIS", "Chat cleared.")


def main() -> None:
    root = tk.Tk()
    app = JarvisUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()
