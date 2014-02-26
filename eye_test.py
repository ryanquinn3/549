import pygame
from pygame.locals import *

def run():
    # Initialise screen
    pygame.init()
    screen = pygame.display.set_mode((700,700))
    pygame.display.set_caption('Basic Pygame program')

    # Fill background
    background = pygame.Surface(screen.get_size())
    background = background.convert()
    background.fill((0, 0, 0))

    # Display some text
    font = pygame.font.Font(None, 36)
    text = font.render("Focus here", 1, (255, 255, 255))
    textpos = text.get_rect()
    textpos.centerx = background.get_rect().centerx
    textpos.centery = background.get_rect().centery
    background.blit(text, textpos)

    # Blit everything to the screen
    screen.blit(background, (0, 0))
    pygame.display.flip()

    
    pygame.display.flip()
    pygame.time.wait(5000)
    background.fill((255,255,255))
    screen.blit(background,(0,0))
    pygame.display.update()
    pygame.time.wait(9000)


