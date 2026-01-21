import turtle
import random
import math

def DrawSquare(a_sideSize: int) -> None:
    for _ in range(4):
        turtle.fd(a_sideSize)
        turtle.left(90)

def Main() -> None:
    turtle.speed(0)
    turtle.setup(800, 800)

if __name__ == "__main__":
    Main()