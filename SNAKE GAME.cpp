#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C  // Cambia a 0x3D si el escáner I2C lo indica

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pines de los botones
const int LEFT_BUTTON = 26;
const int UP_BUTTON = 32;
const int DOWN_BUTTON = 33;
const int RIGHT_BUTTON = 12;

// Tamaño de los bloques de la serpiente
const int SNAKE_BLOCK_SIZE = 4;

// Variables del juego
int deaths = 0;
int score = 0;
int highscore = 0;
int snake_x = SCREEN_WIDTH / 2;
int snake_y = SCREEN_HEIGHT / 2;
int snake_dir_x = 1;  // Dirección inicial: derecha
int snake_dir_y = 0;
int food_x = 0;
int food_y = 0;
boolean is_food_eaten = true;
const int MAX_SEGMENTS = 100;
int segment_x[MAX_SEGMENTS];
int segment_y[MAX_SEGMENTS];
int num_segments = 3;
float speed_multiplier = 1.0;
const float SPEED_INCREMENT = 0.2;
const int BASE_DELAY_TIME = 100;
const int SCREEN_LEFT_EDGE = 0;
const int SCREEN_RIGHT_EDGE = SCREEN_WIDTH - SNAKE_BLOCK_SIZE;
const int SCREEN_TOP_EDGE = 0;
const int SCREEN_BOTTOM_EDGE = SCREEN_HEIGHT - SNAKE_BLOCK_SIZE;

void setup() {
  Wire.begin(21, 22); // SDA=GPIO21, SCL=GPIO22
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    for (;;);  // Si falla, detiene
  }
  display.clearDisplay();

  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);

  segment_x[0] = snake_x;
  segment_y[0] = snake_y;
  segment_x[1] = snake_x - SNAKE_BLOCK_SIZE;
  segment_y[1] = snake_y;
  segment_x[2] = snake_x - 2 * SNAKE_BLOCK_SIZE;
  segment_y[2] = snake_y;

  display.setCursor(0, 10);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println("MADCO");
  display.setCursor(0, SCREEN_HEIGHT - 25);
  display.setTextSize(1);
  display.println("Presiona cualquier");
  display.setCursor(0, SCREEN_HEIGHT - 15);
  display.println("boton para jugar");
  display.display();

  display.fillRect(100, 8, 8, 4, SSD1306_WHITE);
  display.fillRect(96, 12, 8, 4, SSD1306_WHITE);
  display.fillRect(92, 16, 8, 4, SSD1306_WHITE);
  display.fillRect(88, 20, 8, 4, SSD1306_WHITE);
  display.display();

  while (digitalRead(LEFT_BUTTON) == HIGH && digitalRead(UP_BUTTON) == HIGH &&
         digitalRead(DOWN_BUTTON) == HIGH && digitalRead(RIGHT_BUTTON) == HIGH) {
    delay(100);
  }

  reset_game();
}

void reset_game() {
  snake_x = SCREEN_WIDTH / 2;
  snake_y = SCREEN_HEIGHT / 2;
  snake_dir_x = 1;
  snake_dir_y = 0;
  speed_multiplier = 1.0;
  score = 0;

  num_segments = 3;
  segment_x[0] = snake_x;
  segment_y[0] = snake_y;
  segment_x[1] = snake_x - SNAKE_BLOCK_SIZE;
  segment_y[1] = snake_y;
  segment_x[2] = snake_x - 2 * SNAKE_BLOCK_SIZE;
  segment_y[2] = snake_y;

  food_x = random(SCREEN_WIDTH / SNAKE_BLOCK_SIZE) * SNAKE_BLOCK_SIZE;
  food_y = random(SCREEN_HEIGHT / SNAKE_BLOCK_SIZE) * SNAKE_BLOCK_SIZE;
  is_food_eaten = false;
}

void loop() {
  boolean is_collision = false;
  display.clearDisplay();
  int delay_time = (int)(BASE_DELAY_TIME / speed_multiplier);

  if (digitalRead(LEFT_BUTTON) == LOW && snake_dir_x != 1) {
    snake_dir_x = -1;
    snake_dir_y = 0;
  } else if (digitalRead(RIGHT_BUTTON) == LOW && snake_dir_x != -1) {
    snake_dir_x = 1;
    snake_dir_y = 0;
  } else if (digitalRead(UP_BUTTON) == LOW && snake_dir_y != 1) {
    snake_dir_x = 0;
    snake_dir_y = -1;
  } else if (digitalRead(DOWN_BUTTON) == LOW && snake_dir_y != -1) {
    snake_dir_x = 0;
    snake_dir_y = 1;
  }

  for (int i = num_segments - 1; i > 0; i--) {
    segment_x[i] = segment_x[i - 1];
    segment_y[i] = segment_y[i - 1];
  }

  snake_x += snake_dir_x * SNAKE_BLOCK_SIZE;
  snake_y += snake_dir_y * SNAKE_BLOCK_SIZE;

  if (snake_x < SCREEN_LEFT_EDGE) snake_x = SCREEN_RIGHT_EDGE;
  else if (snake_x > SCREEN_RIGHT_EDGE) snake_x = SCREEN_LEFT_EDGE;
  if (snake_y < SCREEN_TOP_EDGE) snake_y = SCREEN_BOTTOM_EDGE;
  else if (snake_y > SCREEN_BOTTOM_EDGE) snake_y = SCREEN_TOP_EDGE;

  if (snake_x == food_x && snake_y == food_y) {
    speed_multiplier += SPEED_INCREMENT;
    is_food_eaten = true;
    score++;
    if (score > highscore) highscore = score;
    if (num_segments < MAX_SEGMENTS) {
      segment_x[num_segments] = segment_x[num_segments - 1];
      segment_y[num_segments] = segment_y[num_segments - 1];
      num_segments++;
    }
  }

  for (int i = 1; i < num_segments; i++) {
    if (snake_x == segment_x[i] && snake_y == segment_y[i]) {
      is_collision = true;
      break;
    }
  }

  segment_x[0] = snake_x;
  segment_y[0] = snake_y;

  if (is_food_eaten) {
    food_x = random(SCREEN_WIDTH / SNAKE_BLOCK_SIZE) * SNAKE_BLOCK_SIZE;
    food_y = random(SCREEN_HEIGHT / SNAKE_BLOCK_SIZE) * SNAKE_BLOCK_SIZE;
    is_food_eaten = false;
  }

  for (int i = 0; i < num_segments; i++) {
    display.fillRect(segment_x[i], segment_y[i], SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, SSD1306_WHITE);
  }
  display.fillCircle(food_x + SNAKE_BLOCK_SIZE / 2, food_y + SNAKE_BLOCK_SIZE / 2, SNAKE_BLOCK_SIZE / 2, SSD1306_WHITE);

  if (is_collision) {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.print("GAME OVER!");
    display.setTextSize(1);
    display.setCursor(0, SCREEN_HEIGHT - 30);
    display.print("Muertes: ");
    display.print(deaths + 1);
    display.setCursor(0, SCREEN_HEIGHT - 20);
    display.print("Puntos: ");
    display.print(score);
    display.setCursor(0, SCREEN_HEIGHT - 10);
    display.print("Record: ");
    display.print(highscore);
    display.display();
    delay(3000);

    while (digitalRead(LEFT_BUTTON) == HIGH && digitalRead(UP_BUTTON) == HIGH &&
           digitalRead(DOWN_BUTTON) == HIGH && digitalRead(RIGHT_BUTTON) == HIGH) {
      delay(10);
    }
    reset_game();
    deaths++;
  }

  display.display();
  delay(delay_time);
}
