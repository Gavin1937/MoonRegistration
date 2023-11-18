# Assuming you already have MoonRegistration package installed in your python environment
import MoonRegistration as mr
from sys import argv
from pathlib import Path

print(f'MoonRegistration Library Version: {mr.__version__}')


if len(argv) < 2:
    print("Usage: ./MoonDetector_basic /path/to/image/folder")
    exit(0)


folder = Path(argv[1])
print(f"Folder Path: {folder}")
for dirEntry in folder.rglob('*'):
    # read image directly from filepath
    detector = mr.MoonDetector(str(dirEntry))
    
    # # read image from bytes
    # with open(dirEntry, 'rb') as file_in:
    #     buffer = file_in.read()
    # detector = mr.MoonDetector(buffer)
    
    
    # calculate moon position
    final_circle = detector.detect_moon()
    
    
    # printing out result
    print("\n\n\n", end='')
    print(f"file.name = '{dirEntry.name}'")
    print(f"Circle: {final_circle}")
    print(f"Square: {mr.circle_to_square_s(final_circle)}")
    print(f"Rectangle: {mr.circle_to_rectangle_s(final_circle)}")

