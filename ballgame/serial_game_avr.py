
import pygame
import time
import math
import random
import serial
import os

# if True, incoming and outgoing info will be printed to the screen
LOGS_ENABLED = False

# setting for serial comm
PORT = "/dev/ttyUSB0"
BAUDRATE = 9600
TIMEOUT = None
# check connection first and quit if there's no connection
if os.system("ls {}".format(PORT)) == 0:
    # create a serial port object
    print("Serial connected")
else:
    print("No connection")
    quit()
serialPort = serial.Serial(PORT, BAUDRATE, timeout = TIMEOUT)
serialPort.flush()

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
frame_rate = 45

# directory where images reside
images_dir = "images"

# directory where the fonts reside
fonts_dir = "fonts"

# background color RGB
bg_color = [153, 191, 252]

# random object
random_generator = random.SystemRandom()

font_happy = pygame.font.Font("{}/{}".format(fonts_dir, "happysans.ttf"), 115)
font_boxy = pygame.font.Font("{}/{}".format(fonts_dir, "boxybold.ttf"), 25)

# multiple images for sprite animation
ball_image_files = ["sp_ball_1.png", "sp_ball_2.png", "sp_ball_3.png",
    "sp_ball_4.png"]
hoop_image_files = ["sp_hoop_1.png", "sp_hoop_2.png", "sp_hoop_3.png",
    "sp_hoop_4.png", "sp_hoop_5.png", "sp_hoop_6.png"]
kid_image_files = ["sp_kid_1.png", "sp_kid_2.png", "sp_kid_3.png",
    "sp_kid_4.png", "sp_kid_5.png"]
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

kid_jumping_sprite_index = 3

# lower and upper limits of the angle and the velocity of the ball
ball_velocity_lower_limit = 20
ball_velocity_upper_limit = 60
ball_angle_lower_limit = 15  # in degrees
ball_angle_upper_limit = 75 # in degrees

# we don't want velocity and angle to reset after a shoot
ball_velocity_last_value = ball_velocity_lower_limit
ball_angle_last_value = ball_angle_lower_limit

player_score = 0

# period stating how fast the animated objects will be animated
kid_anim_period = 7
ball_anim_period = 4
hoop_anim_period = 7

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
    text = font_boxy.render("Score: " + str(score), True, [0, 0, 0])
    game_display.blit(text, (0, display_height - text.get_height()))

def render_velocity_angle_text(velocity, angle):
    """Renders the velocity and angle as text on the upper right corner of the screen."""
    text = font_boxy.render("Velocity: {}  Angle: {}".format(velocity, angle), True, [0, 0, 0])
    game_display.blit(text, (display_width - text.get_width() - 10, display_height - text.get_height()))

def render_shooting_arrow(velocity, angle_in_deg, kid_lowerleft_x, kid_lowerleft_y):
    """Renders an arrow that shows the intended angle and the velocity of the ball."""
    # coordinates of the points of the arrow
    angle_in_rad = angle_in_deg * math.pi / 180
    arrow_line_base_point_x = kid_lowerleft_x + kid_images[0].get_width() - 2
    arrow_line_base_point_y = kid_lowerleft_y - kid_images[0].get_height() + 2
    arrow_line_top_point_x = arrow_line_base_point_x + 2 * velocity * math.cos(angle_in_rad)
    arrow_line_top_point_y = arrow_line_base_point_y - 2 * velocity * math.sin(angle_in_rad)
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

def calculate_ball_position(velocity_initial, angle_in_deg_initial, frame_counter):
    """Calculates the positions of the ball while on the fly."""
    angle_in_rad_initial = angle_in_deg_initial * math.pi / 180
    vx0 = 2 * velocity_initial * math.cos(angle_in_rad_initial)
    vy0 = 2 * velocity_initial * math.sin(angle_in_rad_initial)
    dist_x = vx0 * frame_counter
    dist_y = (vy0 * frame_counter) - (0.5 * 9.8 * (frame_counter ** 2))
    return dist_x, dist_y

def message_display(text):
    """Displays the given message on the screen with big fonts and
    resets the game."""
    text_surface = font_happy.render(text, True, [0, 0, 0])
    text_rect = text_surface.get_rect()
    text_rect.center = ((display_width / 2), (display_height / 2))
    game_display.blit(text_surface, text_rect)
    pygame.display.update()
    time.sleep(2)

def game_loop():

    global player_score
    global ball_velocity_last_value
    global ball_angle_last_value

    # used for controlling the animation speed of the animated objects
    frame_counter = 0

    kid_shoot_anim_playing = False # True if the shooting animation of the kid is playing
    ball_fly_anim_playing = False # True if the ball flying animation is playing
    hoop_basket_anim_playing = False # True if the basket animation of the hoop is playing

    # frame counters to keep track of the frames of animations
    ball_anim_frame_counter = 0
    kid_anim_frame_counter = 0
    hoop_anim_frame_counter = 0

    # coordinates of the visible game objects
    # kid sprite expands and shrinks during the animation
    # so, the lower left corner is taken as the reference point
    kid_lowerleft_x = 10
    kid_lowerleft_y = display_height - ground_image.get_height() + 15
    # hoop sprite expands and shrinks during the animation
    # so, the upper right corner is taken as the reference point
    hoop_upperright_x = display_width
    hoop_upperright_y = display_height - random_generator.randint(200, display_height)
    # ball starts a little far from the upper right of the kid
    ball_x = kid_lowerleft_x + kid_images[3].get_width() + 3
    ball_y = kid_lowerleft_y - kid_images[3].get_height() - 3

    # velocity and angle of the ball
    ball_velocity = ball_velocity_last_value
    ball_angle = ball_angle_last_value     # in degrees

    while True:

        # quit if the QUIT event is detected
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()

        # send information to serial stating if the ball is thrown
        if kid_shoot_anim_playing == False and ball_fly_anim_playing == False and \
            hoop_basket_anim_playing == False:
            serialPort.write(bytes([0]))
            if LOGS_ENABLED == True:
                print("Outgoing: 0")
        else:
            serialPort.write(bytes([1]))
            if LOGS_ENABLED == True:
                print("Outgoing: 1")

        # read data from the serial
        # if leftmost 2 bits 00->pot_angle 01->pot_velocity 10->button pressed 11->do nothing
        # rightmost 6 bits are for value
        data_read = ord(serialPort.read(size = 1))

        # if the ball isn't thrown yet
        if kid_shoot_anim_playing == False and ball_fly_anim_playing == False and \
            hoop_basket_anim_playing == False:
            high_two = data_read >> 6
            low_six = (data_read << 2) >> 2
            if LOGS_ENABLED == True and high_two != 3:
                print("Incoming: {}".format(data_read))
                print("High two: {}".format(high_two))
                print("Low six: {}".format(low_six))
            if high_two == 0:
                # it seems that coming angle has the value in range [0-63]
                # we should map this value to be in the range [ball_angle_lower_limit - ball_angle_upper_limit]
                mapped_angle_increment = ((low_six - 0) * (ball_angle_upper_limit - ball_angle_lower_limit) \
                    / (63 - 0))
                ball_angle = ball_angle_lower_limit + int(mapped_angle_increment)
            elif high_two == 1:
                # it seems that coming velocity has the value in range [64-127]
                # we should map this value to be in the range [ball_velocity_lower_limit - ball_velocity_upper_limit]
                mapped_velocity_increment = ((low_six - 64) * (ball_velocity_upper_limit - ball_velocity_lower_limit) \
                    / (127 - 64))
                ball_velocity = ball_velocity_lower_limit + int(mapped_velocity_increment)
            elif high_two == 2:
                kid_shoot_anim_playing = True
            # make the angle and velocity stay in boundaries
            if ball_angle <= ball_angle_lower_limit:
                ball_angle = ball_angle_lower_limit
            elif ball_angle >= ball_angle_upper_limit:
                ball_angle = ball_angle_upper_limit
            if ball_velocity <= ball_velocity_lower_limit:
                ball_velocity = ball_velocity_lower_limit
            elif ball_velocity >= ball_velocity_upper_limit:
                ball_velocity = ball_velocity_upper_limit

            ball_velocity_last_value = ball_velocity
            ball_angle_last_value = ball_angle

        # animate the kid if conditions met and stop the animation when it's finished
        if kid_shoot_anim_playing == True:
            if frame_counter % kid_anim_period == 0:
                kid_anim_frame_counter += 1
            if kid_anim_frame_counter == len(kid_images) - 1:
                kid_shoot_anim_playing = False
            # if the kid is rendered as jumping, start the ball flying animation
            if kid_anim_frame_counter == kid_jumping_sprite_index:
                ball_fly_anim_playing = True

        # animate the hoop if the conditions met
        # when the animation is finished, display message, increment score and reset the game
        if hoop_basket_anim_playing == True:
            if frame_counter % hoop_anim_period == 0:
                hoop_anim_frame_counter += 1
            if hoop_anim_frame_counter == len(hoop_images) - 1:
                hoop_basket_anim_playing = False
                message_display("Basket!!!")
                player_score += 1
                game_loop()

        # fill the background
        game_display.fill(bg_color)

        # render ground sprites
        render_ground()

        # render the kid
        kid_upperleft_x = kid_lowerleft_x
        kid_upperleft_y = kid_lowerleft_y - kid_images[kid_anim_frame_counter].get_height()
        render_kid(kid_upperleft_x, kid_upperleft_y, kid_anim_frame_counter)

        # render the hoop
        hoop_upperleft_x = hoop_upperright_x - hoop_images[hoop_anim_frame_counter].get_width()
        hoop_upperleft_y = hoop_upperright_y
        render_hoop(hoop_upperleft_x, hoop_upperleft_y, hoop_anim_frame_counter)

        # render the velocity and angle text
        render_velocity_angle_text(ball_velocity, ball_angle)

        # render the score
        render_score(player_score)

        # render the ball if it's on the move
        if ball_fly_anim_playing == True:
            ball_dist_x, ball_dist_y = calculate_ball_position(ball_velocity, ball_angle, ball_anim_frame_counter)
            if frame_counter % ball_anim_period == 0:
                ball_anim_frame_counter += 1
            render_ball(ball_x + ball_dist_x, ball_y - ball_dist_y, ball_anim_frame_counter)
            # if the ball is out of the screen, display message and reset the game
            if ball_x + ball_dist_x > display_width or ball_y - ball_dist_y > display_height:
                ball_fly_anim_playing = False
                message_display("Missed!!!")
                game_loop()
            # if the ball hits the hoop in the next frame, stop the ball from flying and start the hoop basket animation
            dist_next_x, dist_next_y = calculate_ball_position(ball_velocity, ball_angle, ball_anim_frame_counter + 1)
            if ball_x + dist_next_x + ball_images[0].get_width() / 2 > hoop_upperright_x - hoop_images[0].get_width() and \
                ball_x + dist_next_x + ball_images[0].get_width() / 2 < hoop_upperright_x and \
                ball_y - dist_next_y + ball_images[0].get_height() / 2 > hoop_upperright_y and \
                ball_y - dist_next_y + ball_images[0].get_height() / 2 < hoop_upperright_y + hoop_images[0].get_height():
                ball_fly_anim_playing = False
                hoop_basket_anim_playing = True

        # render the shooting arrow if no animation is playing
        if kid_shoot_anim_playing == False and ball_fly_anim_playing == False and \
            hoop_basket_anim_playing == False:
            render_shooting_arrow(ball_velocity, ball_angle, kid_lowerleft_x, kid_lowerleft_y)

        # update the frame counter
        frame_counter = (frame_counter + 1) % 1000

        # update the screen and tick the clock
        pygame.display.update()
        clock.tick(frame_rate)

game_loop()
pygame.quit()
quit()
