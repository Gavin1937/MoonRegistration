# Assuming you already have MoonRegistration package installed in your python environment
import MoonRegistration as mr
from sys import argv
from pathlib import Path

print(f'MoonRegistration Library Version: {mr.__version__}')


if len(argv) < 2:
    print('Usage: python3 ./MoonDetector_basic.py [IMAGE_FOLDER]')
    exit(0)


folder = Path(argv[1])
print(f'Folder Path: {folder}')
for dirEntry in folder.rglob('*'):
    if not dirEntry.is_file():
        continue
    try:
        # read image directly from filepath
        detector = mr.MoonDetect.MoonDetector(str(dirEntry))
        
        # # read image from bytes
        # with open(dirEntry, 'rb') as file_in:
        #     buffer = file_in.read()
        # detector = mr.MoonDetect.MoonDetector(buffer)
        
        # # read image from cv2.MatLike (numpy.ndarray) object
        # import cv2
        # # fill-in pixel data to cv_image...
        # cv_image = cv2.imread(str(dirEntry))
        # detector = mr.MoonDetect.MoonDetector(cv_image)
        
        
        # # You can change the underlying algorithm used in cv::HoughCircles()
        # detector.hough_circles_algorithm = mr.MoonDetect.HoughCirclesAlgorithm.HOUGH_GRADIENT
        # # HOUGH_GRADIENT_ALT algorithm only available with OpenCV >= 4.8.1
        # detector.hough_circles_algorithm = mr.MoonDetect.HoughCirclesAlgorithm.HOUGH_GRADIENT_ALT
        
        # calculate moon position
        final_circle = detector.detect_moon()
        
        
        # printing out result
        print('\n\n\n', end='')
        print(f'file: \'{dirEntry}\'')
        print(f'Circle: {final_circle}')
        print(f'Square: {mr.shapes.circle_to_square_s(final_circle)}')
        print(f'Rectangle: {mr.shapes.circle_to_rectangle_s(final_circle)}')
        
    except Exception as err:
        print(f'Exception: {err}')

