from typing import overload

__all__: list[str] = [
    'Circle',
    'Square',
    'Rectangle',
    'circle_to_square',
    'circle_to_square',
    'circle_to_rectangle',
    'circle_to_rectangle',
    'is_valid_circle',
    'is_valid_circle',
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

@overload
def circle_to_square(circle:Circle) -> Square: ...
@overload
def circle_to_square(x:int, y:int, radius:int) -> Square: ...
@overload
def circle_to_rectangle(circle:Circle) -> Rectangle: ...
@overload
def circle_to_rectangle(x:int, y:int, radius:int) -> Rectangle: ...
@overload
def is_valid_circle(circle:Circle) -> bool: ...
@overload
def is_valid_circle(x:int, y:int, radius:int) -> bool: ...
def rectangle_to_roi(rectangle:Rectangle) -> tuple: ...
def roi_to_rectangle(roi:tuple) -> Rectangle: ...
