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
	def __init__(self, width, height, body, move):
		self.width = width
		self.height = height
		self.body = body
		self.move = move

	def initial(self, screen, width, height, body, direction):
		pygame.draw.rect(screen, white, (self.body[0][0], self.body[0][1], self.move, self.move))
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
				elif direction == "down":
					if x == (len(self.body) - 1):
						self.body[x][1] += self.move
					else:
						self.body[x][0] = self.body[x+1][0]
						self.body[x][1] = self.body[x+1][1]
				elif direction == "left":
					if x == (len(self.body) - 1):
						self.body[x][0] -= self.move
					else:
						self.body[x][0] = self.body[x+1][0]
						self.body[x][1] = self.body[x+1][1]
				elif direction == "right":
					if x == (len(self.body) - 1):
						self.body[x][1] += self.move
					else:
						self.body[x][0] = self.body[x+1][0]
						self.body[x][1] = self.body[x+1][1]

	def draw(self, screen):
		for x in range(len(self.body)):
			pygame.draw.rect(screen, white, (self.body[x][0], self.body[x][1], self.move, self.move))
			pygame.display.flip()

	def isValid(self, direction):
		head = len(self.body)
		if direction == "up":
			if (self.body[head][0] == self.body[head-1][0]) and (self.body[head][1] - self.move == self.body[head-1][1]):
				return False
			else:
				return True
		elif direction == "down":
			if (self.body[head][0]  == self.body[head-1][0]) and (self.body[head][1] + self.move == self.body[head-1][1]):
				return False
			else:
				return True
		elif direction == "left":
			if (self.body[head][0] - self.move == self.body[head-1][0]) and (self.body[head][1] == self.body[head-1][1]):
				return False
			else:
				return True
		elif direction == "right":
			if (self.body[head][0] + self.move == self.body[head-1][0]) and (self.body[head][1] == self.body[head-1][1]):
				return False
			else:
				return True

	def isDead(self, direction):
		head = len(self.body)
		if direction == "up":
			if [self.body[head][0], self.body[head][1]] in self.body[:-1]:
				return False
		elif direction == "down":
			if [self.body[head][0], self.body[head][1]] in self.body[:-1]:
				return False
		elif direction == "left":
			if [self.body[head][0], self.body[head][1]] in self.body[:-1]:
				return False
		elif direction == "right":
			if [self.body[head][0], self.body[head][1]] in self.body[:-1]:
				return False
		if self.body[head][0] == 0 or self.body[head][0] == self.width:
			return False
		if self.body[head][1] == 0 or self.body[head][1] == self.height:
			return False
		return True

	#def fruit(self, screen, width, height):

	def main():
		screen = pygame.display.set_mode([width,height])
		pygame.display.set_caption("SNAKE")
		snk = Snake(width, height, [[xstart, ystart]], 10)
		run = True
		while run:
			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					run = False
				if event.type == pygame.KEYDOWN:
					#if event.key == pygame.K_UP:
					#if event.key == pygame.K_DOWN:
					#if event.key == pygame.K_LEFT:
					#if event.key == pygame.K_RIGHT:
					run = False

main()
pygame.quit()