/* Titulo: Controle_UTS
   Autor: Leonardo Arcanjo - OPTIMA/UFAM
   Ano: 2019
   Versao: 1.3
   Resumo: Controlador para modulo Deslocador UTS-CC, modelo com motor DC. O programa utiliza
           as interrupcoes do Arduino Mega para leitura dos pinos, "A", "B" e "index", do
           encoder rotativo de quadratura. Eh utilizada uma fonte de 12V/3A com regulador de
           tensao para 5V para alimentacao do deslocador e uma Ponte H para alimentacao/controle
           do motor DC do deslocador.
   Atualizacao:
           v1.2
          -> Funcao go_pos() pede ao usuario o valor da distancia em milimetros (mm)
          -> Adicao da funcao motorstop() nas rotinas de movimento em caso de erro
           v1.3
          -> Remoção das mensangens de interrupcao
          -> Na funcao go_pos(), o valor da variavel pulse_count eh zerada para que seja posto um
            valor que adicione com a posicao anterior e nao mais com a referencia do deslocador.
          -> Diminuicao do valor do pwm na rotina de volta da funcao go_half() de 255 para 235,
            em valor de tensao, de 12 volts para 11.05 volts
          -> Correcao do bug do go_half, no qual a ida para a posicao home, quando o deslocador se
            encontra na posicao de "volta", o deslocador nao parava na posicao de "home" indicada
            pelo fabricante
          -> Integracao com o Processing
          -> Go_half tirado do setup()
          
*/
#define pwm 8
#define chaveA 24
#define chaveB 27
#define MechZero 23
#define encoderAb 19
#define encoderBb 20
#define index 3
//#define INA 10
//#define INB 9

/*
  Os pinos 2 = encoderA, 18 = encoderB e 21 = indexb foram omitidos
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
  Serial.setTimeout(10000);                     // Tempo de espera para escrita do valor de passos
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
  int INA = 10;
  int INB = 9;
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  analogWrite(pwm, vel);
  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);
}

//rotina que faz o deslocador se movimentar no sentido negativo;
void motorvolta(int vel) {
  int INA = 10;
  int INB = 9;
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  analogWrite(pwm, vel);
  digitalWrite(INA, LOW);
  digitalWrite(INB, HIGH);
}

//rotina que PARA o deslocador;
void motorstop() {
  int INA = 10;
  int INB = 9;
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
    Serial.print("Distancia desejada: ");
    Serial.print(dist, 4);
    Serial.println(" mm");
  }
  valor_ref = dist * 5000;
  Serial.print("Quantidade de passos: ");
  Serial.println(valor_ref);
  delay(10);
  if (valor_ref > pulse_count) {
    ida();
  } else if (valor_ref < pulse_count) {
    volta();
  } else motorstop();
}

void ida() {
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
  Serial.println(pulse_count);
}

void volta() {
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
  Serial.println(pulse_count);
}

void encoder() {
  attachInterrupt(digitalPinToInterrupt(2), ai0, CHANGE);     //encoder A
  attachInterrupt(digitalPinToInterrupt(18), ai1, CHANGE);     //encoder B
  attachInterrupt(digitalPinToInterrupt(21), ai2, FALLING);    //Index IB
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
