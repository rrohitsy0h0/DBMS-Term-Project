import streamlit as st
import os
import time

PROMPT_FILE = "user_prompt.txt"
DISPLAY_FILE = "../backend/output/display.txt"

st.set_page_config(page_title="NL Query Interface", page_icon="🗄️", layout="centered")
st.title("Natural Language Database Interface")
st.markdown("Enter your query in plain English. The system will process it and return the results.")

if "messages" not in st.session_state:
    st.session_state.messages = []

for message in st.session_state.messages:
    with st.chat_message(message["role"]):
        st.markdown(message["content"])

if prompt := st.chat_input("Ask a question about the database..."):  
    st.session_state.messages.append({"role": "user", "content": prompt})
    with st.chat_message("user"):
        st.markdown(prompt)
    try:
        with open(PROMPT_FILE, "w") as f:
            f.write(prompt)
    except Exception as e:
        st.error(f"Error writing to {PROMPT_FILE}: {e}")
    with st.spinner("Processing query and running simulation..."):
        time.sleep(10) 
        response_text = "No response generated."
        if os.path.exists(DISPLAY_FILE):
            try:
                with open(DISPLAY_FILE, "r") as f:
                    response_text = f.read()
            except Exception as e:
                response_text = f"Error reading {DISPLAY_FILE}: {e}"
        else:
            response_text = f"Backend output file '{DISPLAY_FILE}' not found."

    st.session_state.messages.append({"role": "assistant", "content": response_text})
    with st.chat_message("assistant"):
        st.markdown(response_text)
