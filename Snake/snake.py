import pygame
import random

pygame.init()
pygame.font.init()

white = (255, 255, 255)
black = (0, 0, 0)
green = (0, 255, 0)
red = (255, 0, 0)

width = 700
height = 700
xstart = 0
ystart = 0

class Snake:
	def __init__(self, width, height, body, fruit, move):
		self.width = width
		self.height = height
		self.body = body
		self.fruit = fruit
		self.move = move

	def initial(self, screen, width, height, body, direction):
		pygame.draw.rect(screen, white, (body[0][0], body[0][1], self.move, self.move))
		pygame.display.flip()

	def snake(self, fruit, direction):
		if self.body[0][0] == fruit[0] and self.body[0][1] == fruit[1]:
			self.body = [fruit[0], fruit[1]] + self.body
		else:
			for x in range(len(self.body)):
				if direction == "up":
					if x == (len(self.body) - 1):
						self.body[x][1] -= self.move
					else:
						self.body[x][0] = self.body[x+1][0]
						self.body[x][1] = self.body[x+1][1]
				else if direction == "down":
					if x == (len(self.body) - 1):
						self.body[x][1] += self.move
					else:
						self.body[x][0] = self.body[x+1][0]
						self.body[x][1] = self.body[x+1][1]
				else if direction == "left":
					if x == (len(self.body) - 1):
						self.body[x][0] -= self.move
					else:
						self.body[x][0] = self.body[x+1][0]
						self.body[x][1] = self.body[x+1][1]
				else if direction == "right":
					if x == (len(self.body) - 1):
						self.body[x][1] += self.move
					else:
						self.body[x][0] = self.body[x+1][0]
						self.body[x][1] = self.body[x+1][1]

	def draw(self, screen):
		for x in range(len(self.body)):
			

	def isValid(self, direction):
		head = len(self.body)
		if direction == "up":
			if (self.body[head][0] - self.move == self.body[head-1][0]) and (self.body[head][1] - self.move == self.body[head-1][1]):
				return False
			else:
				return True
		else if direction == "down":
			if (self.body[head][0] - self.move == self.body[head-1][0]) and (self.body[head][1] - self.move == self.body[head-1][1]):
				return False
			else:
				return True
		else if direction == "left":
			if (self.body[head][0] - self.move == self.body[head-1][0]) and (self.body[head][1] - self.move == self.body[head-1][1]):
				return False
			else:
				return True
		else if direction == "right":
			if (self.body[head][0] - self.move == self.body[head-1][0]) and (self.body[head][1] - self.move == self.body[head-1][1]):
				return False
			else:
				return True

	def fruit(self, screen, width, height):

	def main():

main()
pygame.quit()