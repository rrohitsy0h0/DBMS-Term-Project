import streamlit as st
import os
import time

# Define the file paths for communication
PROMPT_FILE = "user_prompt.txt"
DISPLAY_FILE = "../backend/output/display.txt"

# Configure the UI
st.set_page_config(page_title="NL Query Interface", page_icon="🗄️", layout="centered")
st.title("Natural Language Database Interface")
st.markdown("Enter your query in plain English. The system will process it and return the results.")

# Initialize session state to keep track of the conversation history
if "messages" not in st.session_state:
    st.session_state.messages = []

# Render the existing conversation history
for message in st.session_state.messages:
    with st.chat_message(message["role"]):
        st.markdown(message["content"])

# Capture continuous user input
if prompt := st.chat_input("Ask a question about the database..."):
    
    # 1. Display the user's prompt on the screen
    st.session_state.messages.append({"role": "user", "content": prompt})
    with st.chat_message("user"):
        st.markdown(prompt)

    # 2. Write the prompt to the text file (for the backend to pick up)
    try:
        with open(PROMPT_FILE, "w") as f:
            f.write(prompt)
    except Exception as e:
        st.error(f"Error writing to {PROMPT_FILE}: {e}")

    # Display a loading spinner while waiting for the backend
    with st.spinner("Processing query and running simulation..."):
        
        # Note: If your LLM/C++ backend takes time to write to display.txt,
        # you might need a loop here that checks the file's modified timestamp. 
        # For now, we'll use a short sleep to allow the backend a moment to process.
        time.sleep(2) 
        
        # 3. Read the output from the backend via display.txt
        response_text = "No response generated."
        if os.path.exists(DISPLAY_FILE):
            try:
                with open(DISPLAY_FILE, "r") as f:
                    response_text = f.read()
            except Exception as e:
                response_text = f"Error reading {DISPLAY_FILE}: {e}"
        else:
            response_text = f"Backend output file '{DISPLAY_FILE}' not found."

    # 4. Display the backend's response on the screen
    st.session_state.messages.append({"role": "assistant", "content": response_text})
    with st.chat_message("assistant"):
        st.markdown(response_text)
