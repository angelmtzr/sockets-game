#!/usr/bin/env python
import pygame, sys
import numpy as np
import os
import math
import socket

os.environ["SDL_AUDIODRIVER"] = "dummy"

HOST = "172.18.2.3" 
PORT = 5006

#Counters
SQUARESIZE = 70
ROW_COUNT = 6
COLUMN_COUNT = 7
RADIUS = 30
click_mouse = True

#COLORES
BLUE = (0, 0, 255)
RED = (255, 0, 0)
DARKRED = (139, 0, 0)
YELLOW = (255, 255, 0)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GREEN = (0, 255, 0)

def create_board():
    board = np.zeros((ROW_COUNT,COLUMN_COUNT))
    return board

def is_valid_location(board, col):
    return board[ROW_COUNT-1][col] == 0

def get_next_open_row(board, col):
    for r in range(ROW_COUNT):
        if board[r][col] == 0:
            return r
        
def draw_board(board):
    for c in range(COLUMN_COUNT):
        for r in range(ROW_COUNT):
            pygame.draw.rect(screen, BLUE, (c*SQUARESIZE, r*SQUARESIZE+SQUARESIZE, SQUARESIZE, SQUARESIZE))
            pygame.draw.circle(screen, WHITE, (int(c*SQUARESIZE+SQUARESIZE/2), int(r*SQUARESIZE+SQUARESIZE+SQUARESIZE/2)), RADIUS)
     
    for c in range(COLUMN_COUNT):
        for r in range(ROW_COUNT):      
            if board[r][c] == 1:
                pygame.draw.circle(screen, RED, (int(c*SQUARESIZE+SQUARESIZE/2), height-int(r*SQUARESIZE+SQUARESIZE/2)), RADIUS)
            elif board[r][c] == 2: 
                pygame.draw.circle(screen, YELLOW, (int(c*SQUARESIZE+SQUARESIZE/2), height-int(r*SQUARESIZE+SQUARESIZE/2)), RADIUS)
    pygame.display.update()


def check_winner(board):
    # Check for horizontal wins
    for i in range(6):
        for j in range(4):
            if board[i][j] == board[i][j+1] == board[i][j+2] == board[i][j+3] and board[i][j] != 0:
                return board[i][j]

    # Check for vertical wins
    for i in range(3):
        for j in range(7):
            if board[i][j] == board[i+1][j] == board[i+2][j] == board[i+3][j] and board[i][j] != 0:
                return board[i][j]

    # Check for diagonal wins (top-left to bottom-right)
    for i in range(3):
        for j in range(4):
            if board[i][j] == board[i+1][j+1] == board[i+2][j+2] == board[i+3][j+3] and board[i][j] != 0:
                return board[i][j]

    # Check for diagonal wins (bottom-left to top-right)
    for i in range(3, 6):
        for j in range(4):
            if board[i][j] == board[i-1][j+1] == board[i-2][j+2] == board[i-3][j+3] and board[i][j] != 0:
                return board[i][j]

    return 0


class Button:

    def __init__(self, text, width, height, pos,column, elevation):
        #Core attributes
        self.pressed = False
        self.column = column
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
        global click_mouse
        mouse_pos = pygame.mouse.get_pos()
        if self.top_rect.collidepoint(mouse_pos):
            self.top_color = DARKRED
            if pygame.mouse.get_pressed()[0]:
                self.dynamic_elecation = 0
                self.pressed = True
            else:
                self.dynamic_elecation = self.elevation
                if self.pressed:
                    click_mouse = False
                    col = self.column
                    row = 0
                    if is_valid_location(board, col):
                        row = get_next_open_row(board, col)
                    player_response = str(col)+" "+str(row)
                    board[row][col] = token
                    s.sendall(player_response.encode("utf-8"))
    
                    self.pressed = False
        else:
            self.dynamic_elecation = self.elevation
            self.top_color = RED

## Pygame variables ----------------------
game_over = False
turn = 0
## ---------------------------------------

game_Option = 0
auth_Flag = False
game_Flag = False


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    message = s.recv(1024).decode('iso-8859-1')
    print(message)

    while game_Option != 2:
        print("\nElija una opción:\n1.Instrucciones de uso y juego.\n2.Autentificar e iniciar juego.\n")
        game_Option = input()
        if game_Option.isdigit():
            game_Option = int(game_Option)
        else:
            print("Ingrese una opción correcta.\n")

        if game_Option == 1:
            print("Para empezar el juego tienes que presionar la tecla '2' en el menú principal."
                    " Primero tendrás que ingresar un usuario y una contraseña que sean correctas. Una vez"
                    " ingresadas deberás esperar a que otro jugador ingrese al juego. Una vez que ambos jugadores"
                    " se hayan autenticado exitosamente comenzará el juego. El servidor asignará aleatoriamente quien"
                    " empieza el juego y que color de ficha tendrá cada jugador. El objetivo del juego es ser el primero"
                    " en colocar cuatro fichas del mismo color en línea, ya sea vertical, horizontal o diagonalmente."
                    " Cada jugador coloca una ficha en su turno en una columna vacía, y la ficha cae hasta el lugar"
                    " más bajo posible en esa columna. Para esto, en el tablero habrá un boton por cada columna, deberás"
                    " hacer clic en la columna donde desees colocar tu ficha. Una vez terminado el juego se cerrará el tablero"
                    " y mostrará al jugador ganador.")
    
    while not auth_Flag:
        print("Introduzca su usario: ")
        user = input()
        print("Introduzca la contraseña: ")
        password = input()
        auth = user + " " + password
        
        auth = auth.encode("utf-8")
        s.sendall(auth)
        auth_Response = s.recv(1024).decode('iso-8859-1')

        if auth_Response == "0":
            print("Usuario o contraseña incorrecta. Favor de intentar otra vez.\n")
        else:
            auth_Flag = True

    init_Game = int(s.recv(1024).decode('iso-8859-1'))
    print("Iniciando juego...\n")

    ## GUI

    pygame.init()

    pygame.display.set_caption('Conecta 4')
    width = COLUMN_COUNT * SQUARESIZE
    height = (ROW_COUNT+1) * SQUARESIZE
    size = (width, height)
    screen = pygame.display.set_mode(size)
    gui_font = pygame.font.Font(None,20)
    buttons_array = [Button('FICHA', 50, 30, (10+i, 20),int(i/70), 5) for i in range(0, 490, 70)]
    board = create_board()
    finish_flag = False


    while not finish_flag:
        turn = s.recv(1024).decode('iso-8859-1')
        int_turn = int(str(turn))
        token = int_turn % 2

        if int_turn > 1:
            opponentMove = s.recv(1024).decode('iso-8859-1')
            opCol = int(opponentMove.split(" ")[0])
            opRow = int(opponentMove.split(" ")[1])
            if token == 0:
                board[opRow][opCol] = 1
            else:
                board[opRow][opCol] = 2

        winner = check_winner(board)
        if winner == 1:
            print("Jugador uno con fichas rojas gana!")
            finish_flag = True
            continue
        elif winner == 2:
            print("Jugador dos con fichas amarillas gana!")
            finish_flag = True
            continue

        if token == 0:
            token = 2

        print("Turno " + turn + ", le toca a jugador " + str(token))

        while click_mouse:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()
            
            screen.fill(WHITE)

            for button in buttons_array:
                button.draw()

            
            draw_board(board)


        winner = check_winner(board)
        if winner == 1:
            print("Jugador uno con fichas rojas gana!")
            finish_flag = True
            continue
        elif winner == 2:
            print("Jugador dos con fichas amarillas gana!")
            finish_flag = True
            continue
        click_mouse = True

    s.close()