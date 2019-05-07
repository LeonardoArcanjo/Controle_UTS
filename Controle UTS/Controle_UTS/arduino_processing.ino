#define pwm 5
#define chaveA 26       //pino 18 EOR-  
#define chaveB 22       //pino 17 EOR+
#define MechZero 30     //pino 13 Mechanical Zero
#define encoderAb 18    //pino 23 Ab
#define encoderBb 2     //pino 24 Bb
#define index 21        //pino 15 Index
//#define INA 7
//#define INB 6

/*
  Os pinos 20 = encoderA, 19 = encoderB e 3 = indexb foram omitidos
  do #define porque sao entrada de interrupcao
*/

//Variável da opcao
char opcao;

//Variáveis de referencia do encoder
volatile int long pulse_count = 0;
volatile int long valor_ref = 0;
signed int voltas = 0;
int dir = 1;

//prototipos das funcoes
void motorida(int);
void motorvolta(int);
void motorstop(void);
void go_half(void);
void encoder(void);
void go_ida(void);
void go_volta(void);
void go_pos(void);
void ida(void);
void volta(void);
char menu(void);

void setup() {
  Serial.begin(9600);                           // Inicialização da Com. Serial
  //Serial.setTimeout(5000);                     // Tempo de espera para escrita do valor de passos
  pinMode(chaveA, INPUT);                       // Definicao das entradas
  pinMode(chaveB, INPUT);
  pinMode(MechZero, INPUT);
  pinMode(index, INPUT);
  pinMode(encoderAb, INPUT);
  pinMode(encoderBb, INPUT);

  encoder();                                    // inicialização das interrupções
}

void loop() {
  opcao = menu();
  switch (opcao) {
    case 'L':
      go_volta();
      opcao = ' ';
      pulse_count = 0;
      voltas = 0;
      break;
    case 'R':
      go_ida();
      opcao = ' ';
      pulse_count = 0;
      voltas = 0;
      break;
    case 'M':
      go_half();
      opcao = ' ';
      pulse_count = 0;
      voltas = 0;
      break;
    case 'S':
      go_pos(); //logica para deslocar o valor que se desejar
      opcao = ' ';
      pulse_count = 0;
      voltas = 0;
      break;
    default:
      motorstop();
      break;
  }
}

char menu() {
  char opt = Serial.read();
  return opt;
}

//rotina que faz o deslocador se movimentar no sentido positivo;
void motorida(int vel) {
  int INA = 7;
  int INB = 6;
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  analogWrite(pwm, vel);
  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);
}

//rotina que faz o deslocador se movimentar no sentido negativo;
void motorvolta(int vel) {
  int INA = 7;
  int INB = 6;
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  analogWrite(pwm, vel);
  digitalWrite(INA, LOW);
  digitalWrite(INB, HIGH);
}

//rotina que PARA o deslocador;
void motorstop() {
  int INA = 7;
  int INB = 6;
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  digitalWrite(INA, LOW);
  digitalWrite(INB, LOW);
}

//rotina que faz o deslocador ir para o meio
void go_half() {
  int mech = digitalRead(MechZero);
  int indice = digitalRead(index);
  if (mech == LOW) {
    do {
      mech = digitalRead(MechZero);
      motorvolta(255);
      if (Serial.read() == 'P') break;
    } while (mech == LOW);
    motorstop();
    delay(1);
    do {
      indice = digitalRead(index);
      motorida(33);
      if (Serial.read() == 'P') break;
    } while (indice == LOW);
    motorstop();
  } else {
    do {
      mech = digitalRead(MechZero);
      motorida(255);
      if (Serial.read() == 'P') break;
    } while (mech == HIGH);
    motorstop();
    delay(100);
    do {
      indice = digitalRead(index);
      motorida(33);
      if (Serial.read() == 'P') break;
    } while (indice == LOW);
    motorstop();
    delay(100);
    go_half();
  }
}

//rotina que faz o deslocador ir para a posição de "ida"
void go_ida() {
  int EOR;
  do {
    EOR = digitalRead(chaveB);
    motorida(255);
    if (Serial.read() == 'P') break;  
  } while (EOR == LOW);
  motorstop();
}

//rotina faz o deslocador ir para a posição "volta"
void go_volta() {
  int EOR;
  do {
    EOR = digitalRead(chaveA);
    motorvolta(255);
    if (Serial.read() == 'P') break;
  } while (EOR == LOW);
  motorstop();
}

void go_pos() {
  float dist = Serial.parseFloat();          //Le o valor desejado no buffer serial
  if (Serial.read() == '\n') {
    //Serial.print("Distancia desejada: ");
    //Serial.print(dist, 4);
    //Serial.println(" mm");
  }
  valor_ref = dist * 5000;
  //Serial.print("quantidade de pulsos: ");
  //Serial.println(valor_ref);
  delay(10);
  if (valor_ref > pulse_count) {
    ida();
  } else if (valor_ref < pulse_count) {
    volta();
  } else motorstop();
}

void ida() {
  float percorrido;
  do {
    motorida(40);
    if (Serial.read() == 'P') break;
  } while (pulse_count <= (valor_ref - 5000)); // Tensão aplicada ao motor igual a 1.88 volts
  do {
    motorida(37);
    if (Serial.read() == 'P') break;
  } while (pulse_count < (valor_ref - 1000));   // Tensão aplicada ao motor igual a 1.64 volts
  do {
    motorida(33);
    if (Serial.read() == 'P') break;
  } while (pulse_count < valor_ref);
  motorstop();
  //Serial.print("Valor do pulse count: ");
  //Serial.println(pulse_count);
  percorrido = (float)pulse_count/5000;
  Serial.println(percorrido,4);
}

void volta() {
  float percorrido;
  do {
    motorvolta(42);
    if (Serial.read() == 'P') break;
  } while (pulse_count >= (valor_ref + 5000)); // Tensão aplicada ao motor igual a 1.88 volts
  do {
    motorvolta(37);
    if (Serial.read() == 'P') break;
  } while (pulse_count >= (valor_ref + 1000)); // Tensão aplicada ao motor igual a 1.64 volts
  do {
    motorvolta(33);
    if (Serial.read() == 'P') break;
  } while (pulse_count > valor_ref);
  motorstop();
  //Serial.print("Valor do pulse count: ");
  //Serial.println(pulse_count);
  percorrido = (float)pulse_count/5000;
  Serial.println(percorrido,4);
}

void encoder() {
  attachInterrupt(digitalPinToInterrupt(20), ai0, CHANGE);     //pino 19 encoder A
  attachInterrupt(digitalPinToInterrupt(19), ai1, CHANGE);     //pino 20 encoder B
  attachInterrupt(digitalPinToInterrupt(3), ai2, FALLING);    //pino 25  IndexB
}

void ai0() {
  int read_Ab = digitalRead(encoderAb);
  int read_Bb = digitalRead(encoderBb);

  if ((read_Ab == HIGH) && (read_Bb == HIGH)) {
    dir = 1;
    pulse_count++;
  } else if ((read_Ab == LOW) && (read_Bb == HIGH)) {
    dir = 0;
    pulse_count--;
  } else if ((read_Ab == HIGH) && (read_Bb == LOW)) {
    dir = 0;
    pulse_count--;
  } else if ((read_Ab == LOW) && (read_Bb == LOW)) {
    dir = 1;
    pulse_count++;
  }
}

void ai1() {
  int read_Ab = digitalRead(encoderAb);
  int read_Bb = digitalRead(encoderBb);

  if ((read_Ab == HIGH) && (read_Bb == HIGH)) {
    dir = 0;
    pulse_count--;
  } else if ((read_Ab == LOW) && (read_Bb == LOW)) {
    dir = 0;
    pulse_count--;
  } else if ((read_Ab == HIGH) && (read_Bb == LOW)) {
    dir = 1;
    pulse_count++;
  } else if ((read_Ab == LOW) && (read_Bb == HIGH)) {
    dir = 1;
    pulse_count++;
  }
}

void ai2() {
  if (dir == 1) voltas++;
  else voltas--;
}
