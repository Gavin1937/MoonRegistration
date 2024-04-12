# This demo is built on top of OpenCV's demo application for cv2.VideoCapture class
# https://docs.opencv.org/4.x/dd/d43/tutorial_py_video_display.html
# 
# For the ease of demonstration, I will cover the code relating to MoonRegistration inside:
# ==================== MoonRegistration ====================
# ...
# ==================== MoonRegistration ====================
import numpy as np
import cv2 as cv
from sys import argv

# Assuming you already have MoonRegistration package installed in your python environment
import MoonRegistration as mr


if len(argv) < 2:
    print('Usage: python3 ./MoonRegistrar_live_registration.py [MODEL_IMAGE] [LAYER_IMAGE] [CAMERA_DEVICE_ID]')
    exit(0)


# ==================== MoonRegistration ====================

MODEL_IMAGE = argv[1]
LAYER_IMAGE = argv[2]

model_img:np.ndarray = cv.imread(MODEL_IMAGE, cv.IMREAD_UNCHANGED)
layer_img:np.ndarray = cv.imread(LAYER_IMAGE, cv.IMREAD_UNCHANGED)
registrar = mr.MoonRegistrate.MoonRegistrar()
registrar.update_f2d_detector(mr.MoonRegistrate.RegistrationAlgorithms.SIFT)

# Currently, SURF algorithm works the best, but its disabled by default.
# Learn more in BUILDING.md at "About OpenCV non-free modules" section.
# registrar.update_f2d_detector(mr.MoonRegistrate.RegistrationAlgorithms.SURF_NONFREE)

# ==================== MoonRegistration ====================


deviceID:int = None
try:
    deviceID = int(argv[3])
except Exception as e:
    print("Failed to convert \"" + argv[3] + "\" to integer. " + str(e))

cap = cv.VideoCapture(deviceID)
if not cap.isOpened():
    print("Cannot open camera")
    exit()
while True:
    # Capture frame-by-frame
    ret, frame = cap.read()
    # if frame is read correctly ret is True
    if not ret:
        print("Can't receive frame (stream end?). Exiting ...")
        break
    
    # ==================== MoonRegistration ====================
    
    # detect moon location from current frame
    detector = mr.MoonDetect.MoonDetector(frame)
    circle:mr.shapes.Circle = detector.detect_moon()
    
    # cut an image from the circle as our user image
    rect_out, user_img = mr.imgprocess.cut_image_from_circle(
        frame,
        circle.x,
        circle.y,
        circle.radius
    )
    registrar.update_images(user_img, model_img)
    
    try:
        # compute moon image registration with updated images
        registrar.compute_registration()
        
        # transform layer image to match the perspective of user image
        transformed_layer:np.ndarray = registrar.transform_layer_image(layer_img)
        
        # draw transformed layer image on top of current frame, so we can show it
        roi = mr.shapes.rectangle_to_roi(rect_out)
        layer_image_transparency = 1.0
        filter_px = [0,0,0,255]
        frame = mr.imgprocess.stack_imgs_in_place(frame, roi, transformed_layer, layer_image_transparency, filter_px)
    except Exception as e:
        print(e)
    
    # ==================== MoonRegistration ====================
    
    # Display the resulting frame
    cv.imshow('frame', frame)
    if cv.waitKey(5) >= 0:
        break
# When everything done, release the capture
cap.release()
cv.destroyAllWindows()