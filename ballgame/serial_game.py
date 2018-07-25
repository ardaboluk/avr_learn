
import pygame
import time
import math

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
frame_rate = 60

# directory where images reside
images_dir = "images"

# directory where the fonts reside
fonts_dir = "fonts"

# background color RGB
bg_color = [153, 191, 252]

font_happy = pygame.font.Font("{}/{}".format(fonts_dir, "happysans.ttf"), 115)
font_mickey = pygame.font.Font("{}/{}".format(fonts_dir, "mickey.ttf"), 25)

# multiple images for sprite animation
ball_image_files = ["sp_ball_1.png", "sp_ball_2.png", "sp_ball_3.png",
    "sp_ball_4.png"]
hoop_image_files = ["sp_hoop_1.png", "sp_hoop_2.png", "sp_hoop_3.png",
    "sp_hoop_4.png", "sp_hoop_5.png", "sp_hoop_6.png"]
kid_image_files = ["sp_kid_1.png", "sp_kid_2.png", "sp_kid_3.png",
    "sp_kid_4.png"]
ground_image_file = "sp_ground.png"

# load the images
ball_images = []
hoop_images = []
kid_images = []
ground_image = None
for i in range(len(ball_image_files)):
    ball_images.append(pygame.image.load("{}/{}".format(images_dir, ball_image_files[i])))
for i in range(len(hoop_image_files)):
    hoop_images.append(pygame.image.load("{}/{}".format(images_dir, hoop_image_files[i])))
for i in range(len(kid_image_files)):
    kid_images.append(pygame.image.load("{}/{}".format(images_dir, kid_image_files[i])))
ground_image = pygame.image.load("{}/{}".format(images_dir, ground_image_file))

# lower and upper limits of the angle and the velocity of the ball
ball_velocity_lower_limit = 10
ball_velocity_upper_limit = 100
ball_angle_lower_limit = 0
ball_angle_upper_limit = 90

# velocity and angle of the ball
ball_velocity = 10
ball_angle = 45     # in degrees
player_score = 0

def render_ball(x, y, sprite_index):
    """Renders the ball image whose index is given."""
    sprite_index %= len(ball_images)
    game_display.blit(ball_images[sprite_index], (x, y))

def render_hoop(x, y, sprite_index):
    """Renders the hoop image whose index is given."""
    sprite_index %= len(hoop_images)
    game_display.blit(hoop_images[sprite_index], (x,y))

def render_kid(x, y, sprite_index):
    """Renders the kid image whose index is given."""
    sprite_index %= len(kid_images)
    game_display.blit(kid_images[sprite_index], (x,y))

def render_ground():
    """Renders the ground image whose index is given on the bottom of the screen."""
    num_ground_sprites = int(display_width / ground_image.get_width()) + 1
    ground_x = 0
    ground_y = display_height - ground_image.get_height()
    for i in range(num_ground_sprites):
        game_display.blit(ground_image, (ground_x, ground_y))
        ground_x += ground_image.get_width()

def render_score(score):
    """Renders the score to the top left corner of the screen."""
    text = font_mickey.render("Score: " + str(score), True, [0, 0, 0])
    game_display.blit(text, (0,0))

def render_velocity_angle_text(velocity, angle):
    """Renders the velocity and angle as text on the upper right corner of the screen."""
    text = font_mickey.render("Velocity: {}  Angle: {}".format(velocity, angle), True, [0, 0, 0])
    game_display.blit(text, (display_width - text.get_width() - 10, 0))

def render_shooting_arrow(velocity, angle_in_deg, kid_lowerleft_x, kid_lowerleft_y):
    """Renders an arrow that shows the intended angle and the velocity of the ball."""
    # coordinates of the points of the arrow
    angle_in_rad = angle_in_deg * math.pi / 180
    arrow_line_base_point_x = kid_lowerleft_x + kid_images[0].get_width() - 5
    arrow_line_base_point_y = kid_lowerleft_y - kid_images[0].get_height() - 5
    arrow_line_top_point_x = arrow_line_base_point_x + 5 * velocity * math.cos(angle_in_rad)
    arrow_line_top_point_y = arrow_line_base_point_y - 5 * velocity * math.sin(angle_in_rad)
    # point1 of the triangle of the arrow makes 135 degrees with the angle of the arrow from the middle point
    # point2 of the triangle of the arrow makes 225 degrees with the angle of the arrow from the middle point
    arrow_triangle_middle_point_x = arrow_line_top_point_x
    arrow_triangle_middle_point_y = arrow_line_top_point_y
    arrow_triangle_point1_x = arrow_triangle_middle_point_x + 5 * math.cos(angle_in_rad + 3 * math.pi / 4)
    arrow_triangle_point1_y = arrow_triangle_middle_point_y - 5 * math.sin(angle_in_rad + 3 * math.pi / 4)
    arrow_triangle_point2_x = arrow_triangle_middle_point_x + 5 * math.cos(angle_in_rad + 5 * math.pi / 4)
    arrow_triangle_point2_y = arrow_triangle_middle_point_y - 5 * math.sin(angle_in_rad + 5 * math.pi / 4)
    # draw the line of the arrow
    pygame.draw.line(game_display, [0, 0, 0], (arrow_line_base_point_x, arrow_line_base_point_y),
        (arrow_line_top_point_x, arrow_line_top_point_y))
    # draw the triangle of the arrow
    pygame.draw.polygon(game_display, [0, 0, 0],
        [(arrow_triangle_point1_x, arrow_triangle_point1_y),
        (arrow_triangle_middle_point_x, arrow_triangle_middle_point_y),
        (arrow_triangle_point2_x, arrow_triangle_point2_y)])

def calculate_ball_position(velocity, angle_in_deg, frame_counter):
    """Calculates the positions of the ball while on the fly."""
    angle_in_rad = angle_in_deg * math.pi / 180
    vx0 = velocity * math.cos(angle_in_rad)
    vy0 = velocity * math.sin(angle_in_rad)
    dist_x = vx0 * frame_counter
    dist_y = (vy0 * frame_counter) - (0.5 * 9.8 * (frame_counter ** 2))

def message_display(text):
    """Displays the given message on the screen with big fonts and
    resets the game."""
    text_surface = font_happy.render(text, True, [0, 0, 0])
    text_rect = text_surface.get_rect()
    text_rect.center = ((display_width / 2), (display_height / 2))
    game_display.blit(text_surface, text_rect)
    pygame.display.update()
    time.sleep(2)
    game_loop()

def game_loop():

    ball_fly_anim_playing = False # True if the ball flying animation is playing
    kid_shoot_anim_playing = False # True if the shooting animation of the kid is playing
    hoop_basket_anim_playing = False # True if the basket animation of the hoop is playing

    # frame counters to keep track of the frames of animations
    ball_anim_frame_counter = 0
    kid_anim_frame_counter = 0
    hoop_anim_frame_counter = 0

    # coordinates of the visible game objects
    ball_x = 0
    ball_y = 0
    # kid sprite expands and shrinks during the animation
    # so, the lower left corner is taken as the reference point
    kid_lowerleft_x = 10
    kid_lowerleft_y = display_height - ground_image.get_height()
    # hoop sprite expands and shrinks during the animation
    # so, the upper right corner is taken as the reference point
    hoop_upperright_x = display_width
    hoop_upperright_y = display_height - 400

    while True:

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()

        # fill the background
        game_display.fill(bg_color)

        # render ground sprites
        render_ground()

        # render the kid
        kid_upperleft_x = kid_lowerleft_x
        kid_upperleft_y = kid_lowerleft_y - kid_images[kid_anim_frame_counter].get_height()
        render_kid(kid_upperleft_x, kid_upperleft_y, kid_anim_frame_counter)

        # render the ball if it's on the move
        if ball_fly_anim_playing == True:
            render_ball(ball_x, ball_y, ball_anim_frame_counter)

        # render the hoop
        hoop_upperleft_x = hoop_upperright_x - hoop_images[hoop_anim_frame_counter].get_width()
        hoop_upperleft_y = hoop_upperright_y
        render_hoop(hoop_upperleft_x, hoop_upperleft_y, hoop_anim_frame_counter)

        # render the shooting arrow
        render_shooting_arrow(ball_velocity, ball_angle, kid_lowerleft_x, kid_lowerleft_y)

        # render the velocity and angle text
        render_velocity_angle_text(ball_velocity, ball_angle)

        # render the score
        render_score(player_score)

        pygame.display.update()
        clock.tick(frame_rate)

game_loop()
pygame.quit()
quit()
