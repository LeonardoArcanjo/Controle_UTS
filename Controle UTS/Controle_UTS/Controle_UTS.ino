/* Titulo: Controle_UTS
 * Autor: Leonardo Arcanjo - OPTIMa/UFAM 
 * Ano: 2018
 * Versao: 1.0
 * Resumo: Controlador para modulo Deslocador UTS-CC, modelo com motor DC. O programa utiliza
 *         as interrupcoes do Arduino Mega para leitura dos pinos, "A", "B" e "index", do 
 *         encoder rotativo de quadratura. Eh utilizada uma fonte de 12V/3A com regulador de 
 *         tensao para 5V para alimentacao do deslocador e uma Ponte H para alimentacao/controle
 *         do motor DC do deslocador.
*/

//Declaracao dos pinos utilizados e nome atribuidos a estes.
#define pwm 8
#define chaveA 24
#define chaveB 25
#define MechZero 23
#define encoderA 2
#define encoderB 18
#define encoderAb 19
#define encoderBb 20
#define index 3
#define indexb 21
#define INA 10
#define INB 9

//Variável da opcao para o menu
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

void setup() {
  Serial.begin(9600);                       // Inicialização da Com. Serial
  pinMode(chaveA, INPUT);                   //  Definicao das entradas
  pinMode(chaveB, INPUT);
  pinMode(MechZero, INPUT);
  pinMode(indexb, INPUT);
  pinMode(encoderAb, INPUT);
  pinMode(encoderBb, INPUT);
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);

  go_half();                                // Função para fazer o deslocador ir para o meio
  encoder();                                // inicialização das interrupções
}

void loop() {
  if (Serial.available()) {
    opcao = Serial.read();
    switch (opcao) {
      case 'L':
        Serial.println("Ir para a posição volta");
        go_volta();
        opcao = 0;
        pulse_count = 0;
        break;
      case 'R':
        Serial.println("Ir para a posição de ida");
        go_ida();
        opcao = 0;
        pulse_count = 0;
        break;
      case 'M':
        Serial.println("Ir para a posição do meio");
        go_half();
        opcao = 0;
        pulse_count = 0;
        break;
      case 'S':
        Serial.println("Digite o valor de passos desejado.");
        opcao = 0;
        go_pos(); //logica para deslocar ate valor que se desejar
        break;
    }
  }
}

//rotina que faz o deslocador se movimentar no sentido positivo;
void motorida(int vel) {
  analogWrite(pwm, vel);
  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);
}

//rotina que faz o deslocador se movimentar no sentido negativo;
void motorvolta(int vel) {
  analogWrite(pwm, vel);
  digitalWrite(INA, LOW);
  digitalWrite(INB, HIGH);
}

//rotina que PARA o deslocador;
void motorstop() {
  digitalWrite(INA, LOW);
  digitalWrite(INB, LOW);
}

//rotina que faz o deslocador ir para o meio
void go_half() {
  int mech = digitalRead(MechZero);
  int indice;
  if (mech == LOW) {
    do {
      mech = digitalRead(MechZero);
      motorvolta(255);
    } while (mech == LOW);
    motorstop();
    do {
      indice = digitalRead(indexb);
      motorida(37);
    } while (indice == HIGH);
    motorstop();
  } else {
    do {
      mech = digitalRead(MechZero);
      motorida(255);
    } while (mech == HIGH);
    motorstop();
    do {
      indice = digitalRead(indexb);
      motorida(37);
    } while (indice == HIGH);
    motorstop();
  }
}

//rotina que faz o deslocador ir para a posição de "ida"
void go_ida() {
  int EOR = digitalRead(chaveB);
  do {
    EOR = digitalRead(chaveB);
    motorida(255);
  } while (EOR == LOW);
  motorstop();
}

//rotina faz o deslocador ir para a posição "volta"
void go_volta() {
  int EOR = digitalRead(chaveA);
  do {
    EOR = digitalRead(chaveA);
    motorvolta(255);
  } while (EOR == LOW);
  motorstop();
}

//Funcao que identifica a quantidade de passos desejados
void go_pos() {
  valor_ref = Serial.parseInt();
  if(Serial.read() == '\n'){
    Serial.print("Valor Desejado: ");  
    Serial.println(valor_ref);
  }
}

/*Funcoes que deslocam a parte movel enquanto compara o valor digitado pelo usuario e o contador
 * de pulsos do encoder
*/
void ida() {
  if (pulse_count <= (valor_ref - 1000)) motorida(42);      // Tensão aplicada ao motor igual a 1.88 volts
  else if (pulse_count < (valor_ref - 500))motorida(37);  // Tensão aplicada ao motor igual a 1.64 volts
}

void volta() {
  if (pulse_count >= (valor_ref + 1000)) motorvolta(42);     // Tensão aplicada ao motor igual a 1.88 volts
  else if (pulse_count > (valor_ref + 500)) motorvolta(37);  // Tensão aplicada ao motor igual a 1.64 volts
}

void encoder() {
  attachInterrupt(digitalPinToInterrupt(2), ai0, CHANGE);     //encoder A
  attachInterrupt(digitalPinToInterrupt(18), ai1, CHANGE);     //encoder B
  attachInterrupt(digitalPinToInterrupt(3), ai2, RISING);    //Index I
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
  } else if ((read_Ab == HIGH)&& (read_Bb == LOW)) {
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
  Serial.print("passos: ");
  Serial.println(pulse_count);
  Serial.print("voltas: ");
  Serial.println(voltas);
  if (dir == 1) Serial.println("direção: Ida");
  else Serial.println("direção: Volta");
}
