
import pygame
import time

# initialize pygame
pygame.init()

# width and height of the display
display_width = 800
display_height = 600

# set displat properties
game_display = pygame.display.set_mode((display_width, display_height))
pygame.display.set_caption("Serial Basketball")

# set the timer
clock = pygame.time.Clock()

# location of the images
images_loc = "images/"
# multiple images for sprite animation
ball_images = ["sp_ball_1.png", "sp_ball_2.png", "sp_ball_3.png",
    "sp_ball_4.png"]
hoop_images = ["sp_hoop_1.png", "sp_hoop_2.png", "sp_hoop_3.png",
    "sp_hoop_4.png", "sp_hoop_5.png", "sp_hoop_6.png"]
kid_images = ["sp_kid_1.png", "sp_kid_2.png", "sp_kid_3.png",
    "sp_kid_4.png"]
cup_images = ["sp_cup.png"]

def render_ball(x, y, counter):
    """Renders the ball image whose index is given."""
    counter %= len(ball_images)
    game_display.blit(ball_images[counter], (x, y))

def render_hoop(x, y, counter):
    """Renders the hoop image whose index is given."""
    counter %= len(hoop_images)
    game_display.blit(hoop_images[counter], (x,y))

def render_kid(x, y, counter):
    """Renders the kid image whose index is given."""
    counter %= len(kid_images)
    game_display.blit(kid_images[counter], (x,y))

def render_cup(x, y, counter):
    """Renders the cup image whose index is given."""
    counter %= len(cup_images)
    game_display.blit(cup_images[counter], (x,y))

def game_loop():

    # this will be used to show animations
    frame_counter = 0

    
