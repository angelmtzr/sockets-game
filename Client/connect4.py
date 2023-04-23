import pygame, sys
pygame.init()

#COLORES
BLUE = (0, 0, 255)
RED = (255, 0, 0)
DARKRED = (139, 0, 0)
YELLOW = (255, 255, 0)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GREEN = (0, 255, 0)

class Button:

	def __init__(self, text, width, height, pos, elevation):
		#Core attributes
		self.pressed = False
		self.elevation = elevation
		self.dynamic_elecation = elevation
		self.original_y_pos = pos[1]

		#Top rect
		self.top_rect = pygame.Rect(pos,(width,height))
		self.top_color = RED

		#Bottom rect
		self.bottom_rect = pygame.Rect(pos,(width,height))
		self.bottom_color = '#354B5E'

		#Text
		self.text_surf = gui_font.render(text,True,'#FFFFFF')
		self.text_rect = self.text_surf.get_rect(center = self.top_rect.center)

	def draw(self):
		self.top_rect.y = self.original_y_pos - self.dynamic_elecation
		self.text_rect.center = self.top_rect.center 

		self.bottom_rect.midtop = self.top_rect.midtop
		self.bottom_rect.height = self.top_rect.height + self.dynamic_elecation

		pygame.draw.rect(screen,self.bottom_color, self.bottom_rect,border_radius = 5)
		pygame.draw.rect(screen,self.top_color, self.top_rect,border_radius = 5)
		screen.blit(self.text_surf, self.text_rect)
		self.click()

	def click(self):
		mouse_pos = pygame.mouse.get_pos()
		if self.top_rect.collidepoint(mouse_pos):
			self.top_color = DARKRED
			if pygame.mouse.get_pressed()[0]:
				self.dynamic_elecation = 0
				self.pressed = True
			else:
				self.dynamic_elecation = self.elevation
				if self.pressed == True:
					print('click')
					self.pressed = False
		else:
			self.dynamic_elecation = self.elevation
			self.top_color = RED

pygame.display.set_caption('Conecta 4')
size = (490, 490)
screen = pygame.display.set_mode(size)
gui_font = pygame.font.Font(None,20)
buttons_array = [Button('FICHA', 50, 30, (10+i, 20), 5) for i in range(0, 490, 70)]

while True:

	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			pygame.quit()
			sys.exit()
	
	screen.fill(WHITE)

	for button in buttons_array:
		button.draw()

	pygame.draw.rect(screen, BLUE, (0, 70, 490, 420))
	for i in range(0, 420, 70):
		for j in range(0, 490, 70):
			pygame.draw.circle(screen, WHITE, (35 + j, 105 + i), 30)

	pygame.display.update()