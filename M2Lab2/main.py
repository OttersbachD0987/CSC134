import turtle
from turtle import Turtle, _Screen
from typing import Callable
import random
import math

def DrawNGon(a_turtle: Turtle, a_sideCount: int, a_sideSize: int) -> None:
    angle: float = 360 / a_sideCount
    for _ in range(a_sideCount):
        a_turtle.forward(a_sideSize)
        a_turtle.left(angle)

def GenerateDrawLeftNGon(a_sideCount: int) -> Callable[[Turtle, int], None]:
    angle: float = 360 / a_sideCount
    def NGonFunction(a_turtle: Turtle, a_sideSize: int) -> None:
        for _ in range(a_sideCount):
            a_turtle.forward(a_sideSize)
            a_turtle.left(angle)
    return NGonFunction

def GenerateDrawRightNGon(a_sideCount: int) -> Callable[[Turtle, int], None]:
    angle: float = 360 / a_sideCount
    def NGonFunction(a_turtle: Turtle, a_sideSize: int) -> None:
        for _ in range(a_sideCount):
            a_turtle.forward(a_sideSize)
            a_turtle.right(angle)
    return NGonFunction

def GenerateDrawChain(a_draws: list[Callable[[Turtle, int], None]]) -> Callable[[Turtle, int], None]:
    def DrawChain(a_turtle: Turtle, a_sideSize: int) -> None:
        [draw(a_turtle, a_sideSize) for draw in a_draws]
    return DrawChain

def DrawEmbeddedNGon(a_turtle: Turtle, a_sideCount: int, a_sideSize: int, a_sideFunction: Callable[[Turtle, int], None]) -> None:
    angle: float = 360 / a_sideCount
    for _ in range(a_sideCount):
        a_sideFunction(a_turtle, a_sideSize)
        a_turtle.forward(a_sideSize)
        a_turtle.left(angle)

def DrawText(a_turtle: Turtle, a_bottomLeft: tuple[int, int], a_fontSize: int, a_fontSpacing: int, a_text: str) -> None:
    halfFontSize: float = a_fontSize * 0.5
    xLen: float = math.sqrt(2) * a_fontSize
    kLen: float = math.sqrt(2) * halfFontSize
    rLen: float = math.sqrt(halfFontSize ** 2 + a_fontSize ** 2)
    rAngle: float = 90 + math.degrees(math.atan(a_fontSize / halfFontSize))
    vAngle: float = math.degrees(math.atan(a_fontSize / halfFontSize))
    for i, char in enumerate(a_text):
        a_turtle.teleport(a_bottomLeft[0] + (a_fontSize + a_fontSpacing) * i, a_bottomLeft[1])
        a_turtle.setheading(0)
        match char:
            case "a":
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.penup()
                a_turtle.back(a_fontSize)
                a_turtle.right(90)
                a_turtle.back(halfFontSize)
                a_turtle.pendown()
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
            case "b":
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.back(a_fontSize)
            case "c":
                a_turtle.forward(a_fontSize)
                a_turtle.back(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
            case "d":
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.back(halfFontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
            case "e":
                a_turtle.forward(a_fontSize)
                a_turtle.back(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(halfFontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
            case "f":
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.right(90)
                a_turtle.forward(halfFontSize)
                a_turtle.back(halfFontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
            case "g":
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
            case "h":
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.back(halfFontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.back(a_fontSize)
            case "i":
                a_turtle.forward(a_fontSize)
                a_turtle.back(halfFontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.back(a_fontSize)
            case "j":
                a_turtle.forward(halfFontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.back(a_fontSize)
            case "k":
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.back(halfFontSize)
                a_turtle.right(vAngle)
                a_turtle.forward(rLen)
                a_turtle.back(rLen)
                a_turtle.right((90 - vAngle) * 2)
                a_turtle.forward(rLen)
            case "l":
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.back(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
            case "m":
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(halfFontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
                a_turtle.back(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
            case "n":
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
            case "o":
                DrawNGon(a_turtle, 4, a_fontSize)
            case "p":
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(halfFontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
            case "q":
                a_turtle.penup()
                a_turtle.forward(a_fontSize)
                a_turtle.pendown()
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
            case "r":
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(halfFontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(rAngle)
                a_turtle.forward(rLen)
            case "s":
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(halfFontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
            case "t":
                a_turtle.penup()
                a_turtle.forward(halfFontSize)
                a_turtle.pendown()
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.back(a_fontSize)
            case "u":
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.back(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(a_fontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
            case "v":
                a_turtle.penup()
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.pendown()
                a_turtle.right(rAngle)
                a_turtle.forward(rLen)
                a_turtle.left(vAngle * 2)
                a_turtle.forward(rLen)
            case "w":
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.back(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(halfFontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.back(a_fontSize)
                a_turtle.right(90)
                a_turtle.forward(halfFontSize)
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
            case "x":
                a_turtle.penup()
                a_turtle.left(90)
                a_turtle.forward(a_fontSize)
                a_turtle.pendown()
                a_turtle.right(135)
                a_turtle.forward(xLen)
                a_turtle.penup()
                a_turtle.left(135)
                a_turtle.forward(a_fontSize)
                a_turtle.pendown()
                a_turtle.left(135)
                a_turtle.forward(xLen)
            case "y":
                a_turtle.penup()
                a_turtle.forward(halfFontSize)
                a_turtle.pendown()
                a_turtle.left(90)
                a_turtle.forward(halfFontSize)
                a_turtle.left(45)
                a_turtle.forward(kLen)
                a_turtle.back(kLen)
                a_turtle.right(90)
                a_turtle.forward(kLen)
            case "z":
                a_turtle.forward(a_fontSize)
                a_turtle.left(135)
                a_turtle.forward(xLen)
                a_turtle.right(135)
                a_turtle.forward(a_fontSize)

def Main() -> None:
    turtle.setup(800, 800)
    window: _Screen = turtle.Screen()
    window.bgcolor("#000000")
    ross: Turtle = Turtle()
    ross.speed(0)
    ross.color("#00a0bf")
    ross.pensize(3)
    DrawText(ross, (-300, 0), 18, 6, "abcdefghijklmnopqrstuvwxyz")
    
    ross.teleport(-100, -100)
    
    #[DrawNGon(ross, i, 50) for i in range(3, 100)]
    
    turtle.exitonclick()

if __name__ == "__main__":
    Main()