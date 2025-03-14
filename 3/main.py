from flask import Flask, render_template, request, redirect, flash, session
import mysql.connector
import hashlib

# Flask App Initialization
app = Flask(__name__)
app.secret_key = "your_secret_key"

# Database Configuration
db_config = {
    'host': 'localhost',
    'user': 'root',
    'password': 'chu922962',
    'database': 'hw3'
}

# Database Connection
def get_db_connection():
    return mysql.connector.connect(**db_config)

# Login Page
@app.route("/", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        username = request.form['username']
        password = request.form['password']

        # TODO # 4: Hash the password using SHA-256
        # Done by using hashlib.sha256 library
        password = hashlib.sha256(password.encode()).hexdigest()

        # Connect to the database
        conn = get_db_connection()
        cursor = conn.cursor()

        # TODO # 2. Check if the user exists in the database and whether the password is correct
        # Query to check the user
        # Use "SELECT password FROM users WHERE username = %s", (username,)" to prevent SQL injection
        # Because if when using f"SELECT password FROM users WHERE username = '{username}'", 
        # and the username is admin' OR '1'='1 the query may become SELECT password FROM users WHERE username = 'admin' OR '1'='1'
        # and this can bypass authentication by always evaluating to true. However, when using parameterized queries like:
        # "SELECT password FROM users WHERE username = %s", (username,)"
        # The input is escaped and treated as a value, preventing the query from being manipulated.
        cursor.execute("SELECT password FROM users WHERE username = %s", (username,))
        result = cursor.fetchone() # fetchone() returns None if no record is found

        # Close the connection
        cursor.close()
        conn.close()

        if result is None:
            # If no user is found in the database for the given username
            # Display an error message using flash and redirect back to the login page
            flash("User Not Found!", "error")
            return redirect("/")
        else:
            # Retrieve the hashed password stored in the database
            pwd = result[0]
            # If the password matches, store the username in the session and redirect to the welcome page
            if pwd == password:
                session['username'] = username
                return redirect("/welcome")
            # If the password does not match, display an error message using flash and redirect back to the login page
            else:
                flash("Password Incorrect", "error")
                return redirect("/")
        
    return render_template("login.html")

# Welcome Page
@app.route("/welcome")
def welcome():
    if 'username' not in session:
        return redirect("/")
    flash(f"Bienvenue {session['username']}, Comment Ça Va ?", "success")
    return render_template("welcome.html")

# Logout
@app.route("/logout")
def logout():
    session.pop('username', None)
    return redirect("/")

# Signup
@app.route("/signup", methods=["GET", "POST"])
def signup():
    if request.method == "POST":
        username = request.form['username']
        password = request.form['password']

        # TODO # 4: Hash the password using SHA-256
        # Done by using hashlib.sha256 library
        password = hashlib.sha256(password.encode()).hexdigest()

        # Connect to the database
        conn = get_db_connection()
        cursor = conn.cursor()

        # TODO # 3: Add the query to insert a new user into the database
        try:
            # Insert new user into the database
            # I aslo prevent SQL injection here
            cursor.execute("INSERT INTO users (username, password) VALUES (%s, %s)", (username, password))
            conn.commit()
            
            # Show a success message and redirect to the login page
            flash("Account created successfully! Please log in.", "success")
            return redirect("/")
        except mysql.connector.Error as err:
            # Handle any errors during the database operation, such as duplicate usernames
            flash(f"Error: {err}", "danger")
        finally:
            cursor.close()
            conn.close()
    
    return render_template("signup.html")


if __name__ == "__main__":
    app.run(debug=True)
