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
        detector = mr.MoonDetector(str(dirEntry))
        
        # # read image from bytes
        # with open(dirEntry, 'rb') as file_in:
        #     buffer = file_in.read()
        # detector = mr.MoonDetector(buffer)
        
        # # read image from cv2.MatLike (numpy.ndarray) object
        # import cv2
        # # fill-in pixel data to cv_image...
        # cv_image = cv2.imread(str(dirEntry))
        # detector = mr.MoonDetector(cv_image)
        
        
        # calculate moon position
        final_circle = detector.detect_moon()
        
        
        # printing out result
        print('\n\n\n', end='')
        print(f'file: \'{dirEntry}\'')
        print(f'Circle: {final_circle}')
        print(f'Square: {mr.circle_to_square_s(final_circle)}')
        print(f'Rectangle: {mr.circle_to_rectangle_s(final_circle)}')
        
    except Exception as err:
        print(f'Exception: {err}')

