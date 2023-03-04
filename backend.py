# from flask import Flask, render_template, request, redirect, url_for
# import sqlite3
# import os
# import uuid

# app = Flask(__name__)

# # connect to the database
# DATABASE_FILEPATH = 'flashcards.db'
# conn = sqlite3.connect(DATABASE_FILEPATH)
# c = conn.cursor()

# # create the studySets table if it doesn't already exist
# c.execute('''CREATE TABLE IF NOT EXISTS studySets
#             (id INTEGER PRIMARY KEY AUTOINCREMENT,
#             author_name TEXT,
#             subject_name TEXT)''')


# c.execute('''CREATE TABLE IF NOT EXISTS flashcards
#                  (id INTEGER PRIMARY KEY AUTOINCREMENT,
#                   term TEXT,
#                   definition TEXT,
#                   study_set_id INTEGER,
#                   FOREIGN KEY(study_set_id) REFERENCES studySets(id))''')

# @app.route('/index')
# def index():
#     return render_template('frontend.html')

# @app.route('/create_flashcards', methods=['POST'])
# def create_flashcards():
#     # get the form data from the request object
#     author_name = request.form['author_name']
#     subject_name = request.form['subject_name']
#     terms = request.form.getlist('term_1')
#     definitions = request.form.getlist('definition_1')
    
#     # check that the number of terms and definitions match
#     if len(terms) != len(definitions):
#         return "Error: Number of terms and definitions must match"

#     conn = sqlite3.connect('flashcards.db')
#     c = conn.cursor()
#     # insert the author and subject into the table
#     c.execute('''INSERT INTO studySets (author_name, subject_name)
#                 VALUES (?, ?)''', (author_name, subject_name))

#     # get the ID of the study set that was just inserted
#     study_set_id = c.lastrowid

#     # insert the terms and definitions into the flashcards table
#     for i in range(len(terms)):
#         c.execute('''INSERT INTO flashcards (term, definition, study_set_id)
#                      VALUES (?, ?, ?)''', (terms[i], definitions[i], study_set_id))

#     # commit the changes and close the connection
#     conn.commit()
#     conn.close()

#     # redirect to the homepage
#     return redirect(url_for('index'))

# # @app.route('/grabber', methods=['GET'])
# # def grabber():
# #     conn = sqlite3.connect('flashcards.db')
# #     c = conn.cursor()

# #     c.execute('''SELECT count(*) FROM flashcards''')
# #     c.execute('''SELECT term, definition FROM flashcards''')
    
# #     count = c.fetchone()[0]
# #     print(count)
# #     conn.close()

# # @app.route('/grab_data')
# # def grab_data():
# #     conn = sqlite3.connect(DATABASE_FILEPATH)
# #     c = conn.cursor()
    
# #     # get the count of rows in the flashcards table
# #     c.execute('''SELECT count(*) FROM flashcards''')
# #     count = c.fetchone()[0]
    
# #     # get the term and definition for each row in the flashcards table
# #     c.execute('''SELECT term, definition FROM flashcards''')
# #     data = c.fetchall()
    
# #     # close the connection
# #     conn.close()
    
# #     # return the count and data as a dictionary
# #     return {'count': count, 'data': data}

# # @app.route('/download/<filename>')
# # def download(filename):
# #     # get the count and data from the database
# #     conn = sqlite3.connect(DATABASE_FILEPATH)
# #     c = conn.cursor()
# #     c.execute('''SELECT count(*) FROM flashcards''')
# #     count = c.fetchone()[0]
# #     c.execute('''SELECT term, definition FROM flashcards''')
# #     data = c.fetchall()
# #     conn.close()

# #     # create the text file with the count and data
# #     text = f"Count: {count}\n"
# #     for row in data:
# #         text += f"{row[0]}: {row[1]}\n"

# #     # create a unique filename and save the text file
# #     filename = f"{filename}.txt"
# #     filepath = os.path.join(app.root_path, filename)
# #     with open(filepath, 'w') as f:
# #         f.write(text)

# #     # send the file as a response with a unique URL
# #     return send_file(filepath, as_attachment=True, attachment_filename=filename)

# @app.route('/grab_data')
# def grab_data():
#     conn = sqlite3.connect(DATABASE_FILEPATH)
#     c = conn.cursor()
    
#     # get the count of rows in the flashcards table
#     c.execute('''SELECT count(*) FROM flashcards''')
#     count = c.fetchone()[0]
    
#     # get the term and definition for each row in the flashcards table
#     c.execute('''SELECT term, definition FROM flashcards''')
#     data = c.fetchall()
    
#     # close the connection
#     conn.close()
    
#     # generate a unique filename for the text file
#     filename = str(uuid.uuid4()) + '.txt'
    
#     # save the data to the text file
#     with open(filename, 'w') as f:
#         f.write(f'Count: {count}\n\n')
#         for term, definition in data:
#             f.write(f'{term}: {definition}\n')
    
#     # return the filename as a response
#     return {'filename': filename}

# @app.route('/delete', methods=['POST'])
# def delete():
#     id = request.form['id']
#     cursor = conn.cursor()
#     cursor.execute("DELETE FROM your_table WHERE id=?", (id,))
#     conn.commit()
#     if cursor.rowcount == 1:
#         return 'Record deleted successfully'
#     else:
#         return 'Error deleting record'
#     cursor.close()
#     conn.close()

# if __name__ == '__main__':
#     app.run(debug=True)

import os
from flask import Flask, render_template, request, redirect, url_for, send_from_directory
import sqlite3

app = Flask(__name__)

# connect to the database
DATABASE_FILEPATH = 'flashcards.db'
conn = sqlite3.connect(DATABASE_FILEPATH)
c = conn.cursor()

# create the studySets table if it doesn't already exist
c.execute('''CREATE TABLE IF NOT EXISTS studySets
            (id INTEGER PRIMARY KEY AUTOINCREMENT,
            author_name TEXT,
            subject_name TEXT)''')

c.execute('''CREATE TABLE IF NOT EXISTS flashcards
                 (id INTEGER PRIMARY KEY AUTOINCREMENT,
                  term TEXT,
                  definition TEXT,
                  study_set_id INTEGER,
                  FOREIGN KEY(study_set_id) REFERENCES studySets(id))''')

@app.route('/index')
def index():
    return render_template('frontend.html')

@app.route('/create_flashcards', methods=['POST'])
def create_flashcards():
    # get the form data from the request object
    author_name = request.form['author_name']
    subject_name = request.form['subject_name']
    terms = request.form.getlist('term_1')
    definitions = request.form.getlist('definition_1')
    
    # check that the number of terms and definitions match
    if len(terms) != len(definitions):
        return "Error: Number of terms and definitions must match"

    conn = sqlite3.connect('flashcards.db')
    c = conn.cursor()
    # insert the author and subject into the table
    c.execute('''INSERT INTO studySets (author_name, subject_name)
                VALUES (?, ?)''', (author_name, subject_name))

    # get the ID of the study set that was just inserted
    study_set_id = c.lastrowid

    # insert the terms and definitions into the flashcards table
    for i in range(len(terms)):
        c.execute('''INSERT INTO flashcards (term, definition, study_set_id)
                     VALUES (?, ?, ?)''', (terms[i], definitions[i], study_set_id))

    # commit the changes and close the connection
    conn.commit()
    conn.close()

    # redirect to the homepage
    return redirect(url_for('index'))

@app.route('/create_file')
def create_file():
    conn = sqlite3.connect(DATABASE_FILEPATH)
    c = conn.cursor()
    
    # get the count of rows in the flashcards table
    c.execute('''SELECT count(*) FROM flashcards''')
    count = c.fetchone()[0]
    
    # get the term and definition for each row in the flashcards table
    c.execute('''SELECT term, definition FROM flashcards''')
    data = c.fetchall()
    
    # close the connection
    conn.close()
    
    # create the file
    filename = f'flashcards_{count}.txt'
    with open(filename, 'w') as f:
        for row in data:
            f.write(f'{row[0]}: {row[1]}\n')
    
    # return the file or a link to the file
    file_or_link = request.args.get('file_or_link', 'file')
    if file_or_link == 'link':
        return url_for('download_file', filename=filename)
    else:
        return send_from_directory('.', filename, as_attachment=True)

@app.route('/download_file/<filename>')
def download_file(filename):
    return send_from_directory('.', filename, as_attachment=True)

if __name__ == '__main__':
    app.run(debug=True)

# import os
# import sqlite3
# from flask import Flask, render_template, request, redirect, url_for, send_from_directory

# app = Flask(__name__)

# # set the upload folder and allowed file types
# UPLOAD_FOLDER = 'uploads'
# ALLOWED_EXTENSIONS = {'txt'}
# app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

# # connect to the database
# DATABASE_FILEPATH = 'flashcards.db'
# conn = sqlite3.connect(DATABASE_FILEPATH)
# c = conn.cursor()

# # create the studySets table if it doesn't already exist
# c.execute('''CREATE TABLE IF NOT EXISTS studySets
#             (id INTEGER PRIMARY KEY AUTOINCREMENT,
#             author_name TEXT,
#             subject_name TEXT)''')


# c.execute('''CREATE TABLE IF NOT EXISTS flashcards
#                  (id INTEGER PRIMARY KEY AUTOINCREMENT,
#                   term TEXT,
#                   definition TEXT,
#                   study_set_id INTEGER,
#                   FOREIGN KEY(study_set_id) REFERENCES studySets(id))''')

# @app.route('/index')
# def index():
#     return render_template('frontend.html')

# @app.route('/create_flashcards', methods=['POST'])
# def create_flashcards():
#     # get the form data from the request object
#     author_name = request.form['author_name']
#     subject_name = request.form['subject_name']
#     terms = request.form.getlist('term_1')
#     definitions = request.form.getlist('definition_1')
    
#     # check that the number of terms and definitions match
#     if len(terms) != len(definitions):
#         return "Error: Number of terms and definitions must match"

#     conn = sqlite3.connect('flashcards.db')
#     c = conn.cursor()
#     # insert the author and subject into the table
#     c.execute('''INSERT INTO studySets (author_name, subject_name)
#                 VALUES (?, ?)''', (author_name, subject_name))

#     # get the ID of the study set that was just inserted
#     study_set_id = c.lastrowid

#     # insert the terms and definitions into the flashcards table
#     for i in range(len(terms)):
#         c.execute('''INSERT INTO flashcards (term, definition, study_set_id)
#                      VALUES (?, ?, ?)''', (terms[i], definitions[i], study_set_id))

#     # commit the changes and close the connection
#     conn.commit()
#     conn.close()

#     # create the text file with the flashcards data
#     file_data = []
#     file_data.append('Flashcards for {}: {}\n'.format(author_name, subject_name))
#     for term, definition in zip(terms, definitions):
#         file_data.append('{}\n{}\n\n'.format(term, definition))
#     file_name = 'flashcards_{}.txt'.format(study_set_id)
#     file_path = os.path.join(app.config['UPLOAD_FOLDER'], file_name)
#     with open(file_path, 'w') as f:
#         f.writelines(file_data)

#     # get the URL for the created text file
#     file_url = url_for('uploaded_file', filename=file_name)

#     # return a link to the created text file
#     return 'Flashcards created. You can download them from here: {}'.format(file_url)

# @app.route('/create_flashcards', methods=['POST'])
# def create_flashcards():
#     # get the form data from the request object
#     author_name = request.form['author_name']
#     subject_name = request.form['subject_name']
#     terms = request.form.getlist('term_1')
#     definitions = request.form.getlist('definition_1')

#     # check that the number of terms and definitions match
#     if len(terms) != len(definitions):
#         return "Error: Number of terms and definitions must match"

#     conn = sqlite3.connect('flashcards.db')
#     c = conn.cursor()
#     # insert the author and subject into the table
#     c.execute('''INSERT INTO studySets (author_name, subject_name)
#                 VALUES (?, ?)''', (author_name, subject_name))

#     # get the ID of the study set that was just inserted
#     study_set_id = c.lastrowid

#     # insert the terms and definitions into the flashcards table
#     for i in range(len(terms)):
#         c.execute('''INSERT INTO flashcards (term, definition, study_set_id)
#                      VALUES (?, ?, ?)''', (terms[i], definitions[i], study_set_id))

#     # commit the changes and close the connection
#     conn.commit()
#     conn.close()

#     # redirect to the homepage
#     return redirect(url_for('index'))

# @app.route('/grab_data')
# def grab_data():
#     conn = sqlite3.connect(DATABASE_FILEPATH)
#     c = conn.cursor()
    
#     # get the count of rows in the flashcards table
#     c.execute('''SELECT count(*) FROM flashcards''')
#     count = c.fetchone()[0]
    
#     # get the term and definition for each row in the flashcards table
#     c.execute('''SELECT term, definition FROM flashcards''')
#     data = c.fetchall()
    
#     # close the connection
#     conn.close()
    
#     # create a dictionary to hold the count and data
#     result = {'count': count, 'data': data}
    
#     # convert the dictionary to JSON and return it
#     return jsonify(result)

# @app.route('/uploads/<filename>')
# def uploaded_file(filename):
#     return send_from_directory(app.config['UPLOAD_FOLDER'], filename)

# @app.route('/delete', methods=['POST'])
# def delete():
#     id = request.form['id']
#     cursor = conn.cursor()
#     cursor.execute("DELETE FROM your_table WHERE id=?", (id,))
#     conn.commit()
#     if cursor.rowcount == 1:
#         return 'Record deleted successfully'
#     else:
#         return 'Error deleting record'
#     cursor.close()
#     conn.close()


# if __name__ == '__main__':
#     app.run(debug=True)
