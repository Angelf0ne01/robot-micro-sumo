//// Opponent Sensors Decleration ////
#define Right_Op_Sensor A0
#define Front_Op_Sensor A1
#define Left_Op_Sensor A2
//// Line Sensor Decleration ////
#define Line_Sensor A3
//// User Buttons & User Leds Decleration ////
#define Right_Button 8
#define Left_Button 12
#define Led1 4
#define Led2 6
//// Motor Control Pins Decleration ////
#define Left_Motor_Direction 10
#define Left_Motor_Speed 9
#define Right_Motor_Direction 13
#define Right_Motor_Speed 5
// defino las velocidades maxima de busqueda y de ataque
#define MAX_VEL_SEARCH 170
#define MAX_VEL_ATTACH 255
// defino el valor de deteccion del sensor de linea
#define HUMBRAL_LINE_SENSOR 45

// modo del robot
enum MODE
{
  MODE_BACK,         // retrocede
  MODE_SEARCH,       // busca
  MODE_ATTACK_LEFT,  // ataca a la izquierda
  MODE_ATTACK_RIGHT, // ataca a la derecha
  MODE_ATTACK        // ataca de frente
};
// guardo el ultimo modo
int last_mode = MODE_SEARCH;

class Ilogs
{
public:
  virtual void println(const char *) = 0;
  virtual void println(const int) = 0;
  virtual void println(const String) = 0;
  virtual void println(const float) = 0;

  virtual void print(const char *) = 0;
  virtual void print(const int) = 0;
  virtual void print(const String) = 0;
  virtual void print(const float) = 0;
};

class LogsSerial : public Ilogs
{

public:
  LogsSerial()
  {
    Serial.begin(9600);
  }

  void println(const char *message)
  {
    Serial.println(message);
  }
  void println(const int message)
  {
    Serial.println(message);
  }
  void println(const String message)
  {
    Serial.println(message);
  }
  void println(const float message)
  {
    Serial.println(message);
  }

  void print(const char *message)
  {
    Serial.print(message);
  }
  void print(const int message)
  {
    Serial.print(message);
  }
  void print(const String message)
  {
    Serial.print(message);
  }
  void print(const float message)
  {
    Serial.print(message);
  }
};

class Logger
{
private:
  Ilogs *logs;
  int enable = true;

public:
  Logger(Ilogs *logsImpl)
  {
    logs = logsImpl;
  }

  void disable()
  {
    enable = false;
  }
  void println(const char *message)
  {
    if (enable)
      logs->println(message);
  }
  void println(const int message)
  {
    if (enable)
      logs->println(message);
  }
  void println(const String message)
  {
    if (enable)
      logs->println(message);
  }
  void println(const float message)
  {
    if (enable)
      logs->println(message);
  }

  void print(const char *message)
  {
    if (enable)
      logs->print(message);
  }
  void print(const int message)
  {
    if (enable)
      logs->print(message);
  }
  void print(const String message)
  {
    if (enable)
      logs->print(message);
  }
  void print(const float message)
  {
    if (enable)
      logs->print(message);
  }
};

class Motor
{
private:
  int pin_dir;
  int pin_speed;

public:
  Motor(int p_d, int p_s)
  {
    this->pin_dir = p_d;
    this->pin_speed = p_s;
  }

public:
  void down(int speedValue)
  {
    digitalWrite(this->pin_dir, LOW);
    analogWrite(this->pin_speed, speedValue);
  }

  void up(int speedValue)
  {
    digitalWrite(this->pin_dir, HIGH);
    analogWrite(this->pin_speed, 255 - speedValue);
  }

  void stop()
  {
    digitalWrite(this->pin_dir, HIGH);
    analogWrite(this->pin_speed, 255);
  }
};

class Button
{
private:
  int pin;

public:
  Button(int p)
  {
    this->pin = p;
  }

  bool isPress()
  {
    return !digitalRead(this->pin);
  }
};

class Led
{
private:
  int pin;
  bool state = false;

public:
  Led(int p)
  {
    this->pin = p;
  }

  void on()
  {
    digitalWrite(this->pin, HIGH);
    this->state = true;
  }

  void off()
  {
    digitalWrite(this->pin, LOW);
    this->state = false;
  }

  void toggle()
  {
    if (this->state)
      this->off();
    else
      this->on();
  }
};

class SensorDistance
{
private:
  int pin;

public:
  SensorDistance(int p)
  {
    this->pin = p;
  }

  bool isDetected()
  {
    return digitalRead(this->pin);
  }
};

class SensorLine
{
private:
  int pin;

  int umbral = 0;

public:
  SensorLine(int p)
  {
    this->pin = p;
  }

  void setUmbral(int value)
  {
    this->umbral = value;
  }

  bool isDetected()
  {
    return analogRead(this->pin) < this->umbral;
  }
};

Motor *motor_left = new Motor(Left_Motor_Direction, Left_Motor_Speed);
Motor *motor_right = new Motor(Right_Motor_Direction, Right_Motor_Speed);
Button *btn_up = new Button(Right_Button);
Button *btn_down = new Button(Left_Button);
Led *led_up = new Led(Led1);
Led *led_down = new Led(Led2);

SensorDistance *sensor_up = new SensorDistance(Front_Op_Sensor);
SensorDistance *sensor_right = new SensorDistance(Right_Op_Sensor);
SensorDistance *sensor_left = new SensorDistance(Left_Op_Sensor);

SensorLine *sensor_line = new SensorLine(Line_Sensor);

Logger *logger = new Logger(new LogsSerial()); // Ejemplo log Bt: Utiliza LogSerialBt como implementación
///////////// INICIO DE PROGRAMA ////////////////
void AwaitStart()
{
  unsigned long time = millis();
  while (!btn_up->isPress())
  {
    const int AWAIT_TIME = 500;
    if (millis() > time + AWAIT_TIME)
    {
      // cada 500ms prendo y apago el led Azul para indicar que estoy en espera de aprentar el click.
      logger->println("[await]:Esperando");
      led_up->toggle();
      time = millis();
    }
  }

  led_up->off();  // apago el led
  led_down->on(); // prendo el led Naranja para indicar que arranco el programa principal
  logger->println("[await]: demora 5s");
  delay(5000);
  led_down->off();
  logger->println("[await]: termino la espera");
}

///////////// MODOS ////////////////

enum SEARCH_MODE
{
  SEARCH_MODE_RIGHT,
  SEARCH_MODE_LEFT,
  SEARCH_MODE_LEFT_FAST,
  SEARCH_MODE_RIGHT_FAST
};

unsigned long searchTime;
int search_mode = SEARCH_MODE_RIGHT;

void SearchMode()
{

  //al ingresar al modo busqueda el robot tiene configurado diferentes modos para realizar la busqueda.
  // 1er modo: Busca despacio para la derecha
  // 2do modo: Busca despacio para la izqueirda
  // 3er modo: Busca rapido sobre el mismo eje para la izquierda
  // 4to modo: Busca rapido sobre el mismo eje para la derecha
  //Si durante 3000ms se encuentra en este MODO entonces va swicheando entre los diferentes MODOS DE BUSQUEDA.
  

  const int SEARCH_TIME_SWICH_MODE = 3000;

  if (millis() > searchTime + SEARCH_TIME_SWICH_MODE)
  {
    searchTime = millis();
    if (++search_mode > SEARCH_MODE_RIGHT_FAST)
    {
      search_mode = SEARCH_MODE_RIGHT;
    }
  }


  //Como el robot tiene un punto ciego entre el sensor frontal y los sensores laterales.
  //Cuando se encuentra en este punto ciego el MODO se conviernte en modo busqueda
  //por lo cual puede caer en alguno de los caso contrario a su ultima deteccion
  //para ello recuerdo su ultima deteccion y hago un modo buqeuda a toda velocidad
  // al ultimo lado que detecto!

  
  if (last_mode == MODE_ATTACK)
  {
    search_mode = SEARCH_MODE_RIGHT;
  }

  if (last_mode == MODE_ATTACK_LEFT)
  {
    search_mode = SEARCH_MODE_LEFT_FAST;
  }

  if (last_mode == MODE_ATTACK_RIGHT)
  {
    search_mode = SEARCH_MODE_RIGHT_FAST;
  }

  switch (search_mode)
  {
  case SEARCH_MODE_RIGHT:
  {
    logger->println("[search-mode]: search mode right");
    motor_left->up(MAX_VEL_ATTACH);
    motor_right->up(MAX_VEL_SEARCH);
    break;
  }

  case SEARCH_MODE_LEFT:
  {
    logger->println("[search-mode]: search mode left");
    motor_left->up(MAX_VEL_SEARCH);
    motor_right->up(MAX_VEL_ATTACH);
    break;
  }

  case SEARCH_MODE_LEFT_FAST:
  {
    logger->println("[search-mode]: search mode left fast");
    motor_left->down(MAX_VEL_ATTACH);
    motor_right->up(MAX_VEL_ATTACH);
    break;
  }

  case SEARCH_MODE_RIGHT_FAST:
  {
    logger->println("[search-mode]: search mode rigth fast");
    motor_left->up(MAX_VEL_ATTACH);
    motor_right->down(MAX_VEL_ATTACH);
    break;
  }
  }
}

void Mode(int mode)
{
  switch (mode)
  {
  case MODE_BACK:
  {
    logger->println("[mode]:back");
    motor_left->down(MAX_VEL_ATTACH);
    motor_right->down(MAX_VEL_ATTACH);
    break;
  }
  case MODE_SEARCH:
  {
    if (mode != last_mode)
    {
      searchTime = millis();
    }

    SearchMode();
    break;
  }
  case MODE_ATTACK_LEFT:
  {
    logger->println("[mode]:attack-left");
    motor_left->down(MAX_VEL_ATTACH);
    motor_right->up(MAX_VEL_ATTACH);
    break;
  }
  case MODE_ATTACK_RIGHT:
  {
    logger->println("[mode]:attack-right");
    motor_left->up(MAX_VEL_ATTACH);
    motor_right->down(MAX_VEL_ATTACH);
    break;
  }
  case MODE_ATTACK:
  {
    logger->println("[mode]:attack");
    motor_left->up(MAX_VEL_ATTACH);
    motor_right->up(MAX_VEL_ATTACH);
    break;
  }
  }
}

void setup()
{

  pinMode(Left_Button, INPUT_PULLUP);
  pinMode(Right_Button, INPUT_PULLUP);
  pinMode(Left_Op_Sensor, INPUT_PULLUP);
  pinMode(Right_Op_Sensor, INPUT_PULLUP);
  pinMode(Left_Motor_Direction, OUTPUT);
  pinMode(Right_Motor_Direction, OUTPUT);
  pinMode(Left_Motor_Speed, OUTPUT);
  pinMode(Right_Motor_Speed, OUTPUT);

  sensor_line->setUmbral(HUMBRAL_LINE_SENSOR);
  AwaitStart();

  searchTime = millis();
}

void loop()
{
  int mode = MODE_SEARCH;

  if (sensor_left->isDetected())
  {
    mode = MODE_ATTACK_LEFT;
  }

  if (sensor_right->isDetected())
  {
    mode = MODE_ATTACK_RIGHT;
  }

  if (sensor_up->isDetected())
  {
    mode = MODE_ATTACK;
  }

  if (sensor_line->isDetected())
  {
    mode = MODE_BACK;
  }

  Mode(mode);
  last_mode = mode;
}