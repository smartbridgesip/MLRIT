import RPi.GPIO as GPIO
import time
import numpy as np
import cv2
haar_file = 'haarcascade_frontalface_default.xml'
datasets = 'datasets'
  

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(17, GPIO.OUT)

p = GPIO.PWM(17, 50) # GPIO 17 for PWM with 50Hz
p.start(1) # Initialization
rate = 1
inc = 1
cap = cv2.VideoCapture(0)
print("camera is initialized")
count = 0

face_cascade = cv2.CascadeClassifier(haar_file) 
webcam = cv2.VideoCapture(0) 
try:
  while True:
    (_, im) = webcam.read() 
    gray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY) 
    faces = face_cascade.detectMultiScale(gray, 1.3, 5) 
    for (x, y, w, h) in faces: 
        cv2.rectangle(im, (x, y), (x + w, y + h), (255, 0, 0), 2) 
        face = gray[y:y + h, x:x + w] 
        face_resize = cv2.resize(face, (width, height)) 
        # Try to recognize the face 
        prediction = model.predict(face_resize) 
        cv2.rectangle(im, (x, y), (x + w, y + h), (0, 255, 0), 3) 
  
        if prediction[1]<500: 
  
           cv2.putText(im, '% s - %.0f' % 
(names[prediction[0]], prediction[1]), (x-10, y-10),  
cv2.FONT_HERSHEY_PLAIN, 1, (0, 255, 0)) 
        else: 
          cv2.putText(im, 'not recognized',  
(x-10, y-10), cv2.FONT_HERSHEY_PLAIN, 1, (0, 255, 0)) 
  
    cv2.imshow('OpenCV', im)
    
    key = cv2.waitKey(10) 
    if key == 27: 
        break
      
    if (im==1):
      print("you can enter inside")
    p.ChangeDutyCycle(12.5) #180
    time.sleeep(2)
    p.ChangeDutyCycle(7.5)
    else:
      print("your entry is restricted")
    time.sleep(0.2)
    ret, frame = cap.read()
    #time.sleep(0.2)
    cv2.imwrite("/home/pi/Desktop/project/datasets/%d.png" %count, frame)
    count+=1
except KeyboardInterrupt:
    p.stop()
    GPIO.cleanup()
cap.release()
cv2.destroyAllWindows()
