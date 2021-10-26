import cv2
import datetime, time
import os, sys
import numpy as np
import smtplib
import serial

from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.application import MIMEApplication
from threading import Thread
from twilio.rest import Client
from flask import Flask, render_template, Response, request

global capture, switch, p, fail
global wu, lv, rs, sp, to

capture=0
switch=1

account_sid = 'AC2ae29fe3965b19e18cae8c3c501177f9'
auth_token = '4293c9848c8e6f2b12f6547294ac1432'

client = Client(account_sid, auth_token)

try:
    os.mkdir('./shots')
except OSError as error:
    pass

app = Flask(__name__, template_folder='./templates')

ser_key = serial.Serial("/dev/ttyACM0",9600)

camera = cv2.VideoCapture(-1)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/cctv')
def cctv():
    return render_template('cctv.html')

@app.route('/led')
def led():
    return render_template('led.html')

@app.route('/key')
def key():
    return render_template('key.html')

@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/requests', methods=['POST','GET'])
def tasks():
    global switch,camera
    if request.method == 'POST':
        if request.form.get('click') == 'Capture':
            global capture
            capture=1

        elif request.form.get('home') ==  'Home':
            return render_template('index.html')

        elif  request.form.get('stop') == 'Stop/Start':      
            if(switch==1):
                switch=0
                camera.release()
                cv2.destroyAllWindows()
                
            else:
                camera = cv2.VideoCapture(-1)
                switch=1       
    
    elif request.method=='GET':
        return render_template('cctv.html')
    return render_template('cctv.html')

def send_message():
    message = client.messages.create(
                    to="+8201037843623",
                    from_="+12818099538",
                    body="Door lock password failed Check your mail https://m.mail.naver.com/mobile/folder/0/all"
                    )
    
def send_email():
    global p
    smtp = smtplib.SMTP('smtp.gmail.com',587)
    smtp.starttls()
    smtp.login("a39843623@gmail.com","tngsnxnbmslibmpr")

    msg = MIMEMultipart()
    text = MIMEText('CCTV Capture')
    msg['Subject'] = 'Door lock password failed'
    msg['To'] = 'seungho3623@naver.com'
    msg.attach(text)
    file_name = p
    with open(file_name,'rb')as file_FD:
        etcPart = MIMEApplication(file_FD.read())
        etcPart.add_header('Content-Disposition','attachment',filename = file_name)
        msg.attach(etcPart)
        smtp.sendmail('a39843623@gmail','seungho3623@naver.com',msg.as_string())
        smtp.quit()
        
def gen_frames(): 
    global out, capture, p, fail
    while True:
        success, frame = camera.read()
        if success:
            if (capture):
                capture = 0
                now = datetime.datetime.now()
                p = os.path.sep.join(['shots', "shot_{}.png".format(str(now).replace(":",''))])
                cv2.imwrite(p, frame)
                send_message()
                send_email()
                
            try:
                ret, buffer = cv2.imencode('.jpg', cv2.flip(frame,1))
                frame = buffer.tobytes()
                yield (b'--frame\r\n'
                       b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')
            except Exception as e:
                pass

        else:
            pass
        
@app.route('/led_requests', methods=['POST','GET'])
def leds():
    global wu, lv, rs, sp, to
    if request.method == 'POST':
        if request.form.get('home') ==  'Home':
            return render_template('index.html')
        
        elif request.form.get('o') == 'LED_ON':
             ser_key.write('o')
        
        elif request.form.get('f') == 'LED_OFF':
             ser_key.write('f')
    
    elif request.method=='GET':
        return render_template('led.html')
    return render_template('led.html')

@app.route('/key_requests', methods=['POST','GET'])
def keys():
    if request.method == 'POST':
         if request.form.get('home') ==  'Home':
             return render_template('index.html')
            
         elif request.form.get('key_1') == '1':
             ser_key.write('1')
                 
         elif request.form.get('key_2') == '2':
             ser_key.write('2')

         elif request.form.get('key_3') == '3':
             ser_key.write('3')
            
         elif request.form.get('key_4') == '4':
             ser_key.write('4')
            
         elif request.form.get('key_5') == '5':
             ser_key.write('5')
                
         elif request.form.get('key_6') == '6':
             ser_key.write('6')

         elif request.form.get('key_7') == '7':
             ser_key.write('7')
            
         elif request.form.get('key_8') == '8':
             ser_key.write('8')
            
         elif request.form.get('key_9') == '9':
             ser_key.write('9')
                 
         elif request.form.get('key_0') == '0':
             ser_key.write('0')
            
         elif request.form.get('key_*') == '*':
             ser_key.write('*')
            
         elif request.form.get('key_#') == '#':
             ser_key.write('#')

         elif request.method=='GET':
             return render_template('key.html')
         return render_template('key.html')

if __name__ == '__main__':
    app.run(host='0.0.0.0')
