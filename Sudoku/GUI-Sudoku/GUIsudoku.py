import pygame
import time
#from SimpleSudokuSolver import solve, isCorrect

pygame.init()
pygame.font.init()
clock = pygame.time.Clock()
font = pygame.font.SysFont("comicsansms", 25)

sampleBoard = [
    [7,8,0,4,0,0,1,2,0],
    [6,0,0,0,7,5,0,0,9],
    [0,0,0,6,0,1,0,7,8],
    [0,0,7,0,4,0,2,6,0],
    [0,0,1,0,5,0,9,3,0],
    [9,0,4,0,6,0,0,0,5],
    [0,7,0,3,0,0,0,1,2],
    [1,2,0,0,0,7,4,0,0],
    [0,4,9,2,0,6,0,0,7]
]

# Colors
black = (0, 0, 0)
white = (255, 255, 255)
red = (255, 0, 0)
grey = (220, 220, 220)

class Board:
	def __init__(self, width, height, board):
		# Size of each block in the grid
		self.width = width
		self.height = height
		self.board = board
		self.gap = 0

	# Draw the grid
	def draw(self, screen):
		self.gap = self.width / 9
		middle = self.gap / 3

		for i in range(10):
			if i % 3 == 0 and i != 0:
				wide = 6
			else:
				wide = 1
			pygame.draw.line(screen, white, [0, i * self.gap], [self.width, i * self.gap], wide)
			pygame.draw.line(screen, white, [i * self.gap, 0], [i * self.gap, self.height], wide)
			pygame.display.flip()
		
		for x in range(len(self.board)):
			for y in range(len(self.board[0])):
				if self.board[x][y] == 0:	# print blank on the cube with zero
					continue
				text = font.render(str(self.board[x][y]), True, white)
				screen.blit(text, [x * self.gap + 1.25 * middle, y * self.gap + middle - 5])
				pygame.display.flip()
	
	def click(self, location):
		checker = [location[0] / self.gap, location[1] / self.gap]
		if self.board[checker[0]][checker[1]] != 0:
			return False
		else:
			return True

	#def tempInput(self, screen, value):


# Main loop
def main():
	screen = pygame.display.set_mode([700,700])
	pygame.display.set_caption("SUDOKU")
	board = Board(700,700,sampleBoard)
	run = True
	while run:
		board.draw(screen)
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				run = False


main()
pygame.quit()
        
