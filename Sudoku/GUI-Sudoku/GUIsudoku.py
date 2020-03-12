import pygame
import time

pygame.init()
pygame.font.init()
clock = pygame.time.Clock()
font = pygame.font.SysFont("comicsansms", 25)

board = [
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
green = (0, 255, 0)
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
				screen.blit(text, [y * self.gap + 1.25 * middle, x * self.gap + middle - 5])
				pygame.display.flip()


	def getEmpty(self, board, coord_list):     #to get a list of tuples of coord with 0 in it
		for x in range(len(board)):
			for y in range(len(board[0])):
				if board[x][y] == 0:
					coord_list += [(x, y)]      # [(row, column)]


	def isCorrect(self, board, num, coord):               # check if the given number is suitable to be put in
		for x in range(len(board[coord[0]])):
			if num == board[coord[0]][x]:           # check the row of the board
				return False

			if num == board[x][coord[1]]:           # check the column of the board
				return False

		xbox = coord[0] // 3
		ybox = coord[1] // 3

		for x in range(xbox*3, xbox*3 + 3):         # check the box of the board
			for y in range(ybox*3, ybox*3 + 3):
				if num == board[x][y]:
					return False

		return True

	def solve(self, board, coord_list, empyList, screen):
		if coord_list != []:
			for number in range(1, 10):
				index = coord_list[0]
				if self.isCorrect(board, number, index):
					board[index[0]][index[1]] = number      # make a decision
					self.solve(board, coord_list[1:], empyList, screen)        # backtrack until the list has 0 element
					board[index[0]][index[1]] = 0       # unmake decision
		else:
			self.drawSolve(board, empyList, screen)

	def drawSolve(self, board, empyList, screen):
		middle = self.gap / 3
		for index in empyList:
			val = board[index[0]][index[1]]
			text = font.render(str(val), True, white)
			screen.blit(text, [index[1] * self.gap + 1.25 * middle, index[0] * self.gap + middle - 5])
			pygame.display.flip()
			pygame.time.delay(100)

# Main loop
def main():
	screen = pygame.display.set_mode([700,700])
	pygame.display.set_caption("SUDOKU")
	boardobj = Board(700,700,board)
	run = True
	val = None
	while run:
		boardobj.draw(screen)
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				run = False
			if event.type == pygame.KEYDOWN:
				if event.key == pygame.K_RETURN:
					empty_coord = []
					boardobj.getEmpty(board, empty_coord)
					boardobj.solve(board, empty_coord, empty_coord, screen)
					
			
		pygame.display.update()

main()
pygame.quit()
        
