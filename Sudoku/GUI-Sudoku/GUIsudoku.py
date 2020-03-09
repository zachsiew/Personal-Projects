import pygame
from SimpleSudokuSolver import solve, isCorrect

pygame.init()

# Colors
black = (0, 0, 0)
white = (255, 255, 255)
red = (255, 0, 0)
grey = (220, 220, 220)

# Size of each block in the grid
width = 25
height = 25

# Creating the grid in list of lists form
grid = []
for row in range(10):
    grid.append([])
    for column in range(10):
        grid[row].append(0)

screen = pygame.display.set_mode([500,500])
pygame.display.set_caption("SUDOKU")

# Main loop
while True:
    for event in pygame.event.get():
        if event.type == pygame.K_LEFT:
            print("lol")