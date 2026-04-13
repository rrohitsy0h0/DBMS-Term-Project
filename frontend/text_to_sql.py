import json
import urllib.request
import sys
import os

def read_user_prompt(file_path="user_prompt.txt"):
    if not os.path.exists(file_path):
        print(f"{file_path} not found!")
        return None
    
    with open(file_path, "r") as f:
        return f.read().strip()


def get_sql_from_ollama(english_query, model="llama3"):
    url = "http://localhost:11434/api/generate"
    
    # (Optional but recommended) Add schema here for better accuracy
    schema = """
    Tables:
    students(id, name, roll_no, department, semester, cgpa, email, phone)
    courses(id, code, title, department, credits, instructor, semester, max_seats)
    enrollments(id, student_id, course_id, grade, year, marks, attendance_pct)
    library_books(id, isbn, title, author, genre, published_year, available_copies, total_copies)
    """

    prompt = (
        "You are an expert SQL developer. Convert the following English text into an SQL query. "
        "Return ONLY the SQL query. Do not include markdown formatting, backticks, or any explanations.\n\n"
        f"Database Schema:\n{schema}\n\n"
        f"English Text: {english_query}\n\n"
        "SQL Query:"
    )
    
    data = {
        "model": model,
        "prompt": prompt,
        "stream": False
    }
    
    try:
        req = urllib.request.Request(
            url,
            data=json.dumps(data).encode('utf-8'),
            headers={'Content-Type': 'application/json'}
        )
        with urllib.request.urlopen(req) as response:
            result = json.loads(response.read().decode('utf-8'))
            return result.get("response", "").strip()
    except Exception as e:
        print(f"Error communicating with Ollama: {e}")
        return None


def clean_sql(sql_query):
    if sql_query.startswith("```sql"):
        sql_query = sql_query[6:]
    elif sql_query.startswith("```"):
        sql_query = sql_query[3:]
    
    if sql_query.endswith("```"):
        sql_query = sql_query[:-3]
    
    return sql_query.strip()


def main():
    english_query = read_user_prompt()

    if not english_query:
        print("No query found in user_prompt.txt")
        sys.exit(1)

    output_file = "query.sql"

    print(f"Sending request to Ollama for: '{english_query}'...\n")

    sql_query = get_sql_from_ollama(english_query, model="llama3")

    if sql_query:
        sql_query = clean_sql(sql_query)

        # Save SQL
        with open(output_file, "w") as f:
            f.write(sql_query + "\n")

        print("✅ Success!")
        print(f"Saved to: {output_file}")
        print("\nPreview:")
        print("-" * 40)
        print(sql_query)
        print("-" * 40)
    else:
        print("❌ Failed to generate SQL. Make sure Ollama is running (`ollama serve`).")


if __name__ == "__main__":
    main()
