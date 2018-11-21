# dlibFaceRecognitionHttpServer

# thanks
dlib onion curl

## Install 
### 1.download
  http://dlib.net/files/dlib_face_recognition_resnet_model_v1.dat.bz2 \
  http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2 \
  opencv3.2 \
  later input '.dat' file to model directory.

### 2.compile curl onion dlib
  cmake; make;  make install 

### 3.compile myserver
  cmake; make; get bin 

## usage
First, you have to start the server,
and then, you need used client connected our server,
last, change ip port in onion_server.c
 
 
 
 tips:\
 The software is currently under implementation
