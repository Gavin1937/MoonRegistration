__all__: list[str] = [
    'Circle',
    'Square',
    'Rectangle',
    'circle_to_square_s',
    'circle_to_square_p',
    'circle_to_rectangle_s',
    'circle_to_rectangle_p',
    'is_valid_circle_s',
    'is_valid_circle_p',
    'rectangle_to_roi',
    'roi_to_rectangle',
]

class Circle:
    def __init__(self, x:int, y:int, radius:int) -> None:
        self.x:int = x
        self.y:int = y
        self.radius:int = radius
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...

class Square:
    def __init__(self, x:int, y:int, width:int) -> None:
        self.x:int = x
        self.y:int = y
        self.width:int = width
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...

class Rectangle:
    def __init__(self, top_left_x:int, top_left_y:int, bottom_right_x:int, bottom_right_y:int) -> None:
        self.top_left_x:int = top_left_x
        self.top_left_y:int = top_left_y
        self.bottom_right_x:int = bottom_right_x
        self.bottom_right_y:int = bottom_right_y
    def __str__(self) -> str: ...
    def __repr__(self) -> str: ...

def circle_to_square_s(circle:Circle) -> Square: ...
def circle_to_square_p(x:int, y:int, radius:int) -> Square: ...
def circle_to_rectangle_s(circle:Circle) -> Rectangle: ...
def circle_to_rectangle_p(x:int, y:int, radius:int) -> Rectangle: ...
def is_valid_circle_s(circle:Circle) -> bool: ...
def is_valid_circle_p(x:int, y:int, radius:int) -> bool: ...
def rectangle_to_roi(rectangle:Rectangle) -> tuple: ...
def roi_to_rectangle(roi:tuple) -> Rectangle: ...
