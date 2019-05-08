import processing.serial.*;

Serial port;
PImage img;
PFont f, f1;

String[] number = {"0", "0", "0", "0", "0", "0", "0", "0", "0", "0"};
String typing = "";
String saved = "";
String lastValue = "";
String mensagem = "";

boolean stateCursor = false;
boolean ControlCursor = false;
boolean isFocused = true;
boolean state = true;
boolean state1 = false;
boolean state2 = true;
boolean conect = false;
int width_e= 90, height_e = 50;
int indent = 50;
int a = 0;

void setup() {
  size(900, 600);
  String portName = Serial.list()[0];
  port = new Serial(this, portName, 9600);

  if (portName != "") conect = true;
  else conect = false;

  f = createFont("Arial", 16);
  f1 = createFont("Arial Bold", 16);
  img = loadImage("LogoLab3.png");
  ControlCursor = true;
}

void draw() {
  tela();
  seleciona();
  texto();
  identificador();
  arduino();
}

void tela() {
  background(150);        //Background cor Azul
  noStroke();                        //Sem bordas no logo
  image(img, 50, 50, 350, 100);      //Carrega o logo do lab

  noStroke();
  fill(200);                             //Preenchimento das Elipses
  ellipse(100, 500, width_e, height_e);  //botao left
  ellipse(250, 500, width_e, height_e);  //botao right
  ellipse(400, 500, width_e, height_e);  //botao Home

  ellipse(300, 300, width_e, height_e);  //botao Go
  ellipse(450, 300, width_e, height_e);  //botao Stop

  ellipse(70, 350, 30, 30);              //botao cw
  ellipse(200, 350, 30, 30);             //botao ccw

  if (state) {
    fill(10);
    ellipse(70, 350, 10, 10);
  } else {
    fill(10);
    ellipse(200, 350, 10, 10);
  }


  if (conect) fill(0, 255, 0);
  else       fill(255, 0, 0);
  ellipse(800, 100, 20, 20);

  //Retangulo para digitar o valor desejado
  noStroke();
  fill(255);
  rect(indent, 285, 140, 30);

  //Retangulo dos valores passados
  rect(700, 250, 140, 250);
}

void seleciona() {
  if ((mouseX >= 100-width_e/2 && mouseX <= 100+width_e/2) && (mouseY >= 500-height_e/2 && mouseY <= 500+height_e/2)) {
    strokeWeight(3);
    stroke(255);
    ellipse(100, 500, width_e, height_e);
    if (mousePressed) {
      println('L');
      saved = "N";
      atribuition();
      port.write('L');
      delay(250);
    }
  } else if ((mouseX >= 250-width_e/2 && mouseX <= 250+width_e/2) && (mouseY >= 500-height_e/2 && mouseY <= 500+height_e/2)) {
   strokeWeight(3);
    stroke(255);
    ellipse(250, 500, width_e, height_e);
    if (mousePressed) {
      port.write('M');
      println('M');
      saved = "M";
      atribuition();
      delay(250);
    }
  } else if ((mouseX >= 400-width_e/2 && mouseX <= 400+width_e/2) && (mouseY >= 500-height_e/2 && mouseY <= 500+height_e/2)) {
    strokeWeight(3);
    stroke(255);
    ellipse(400, 500, width_e, height_e);
    if (mousePressed) {
      port.write('R');
      println('R');
      saved = "P";
      atribuition();
      delay(250);
    }
  } else if ((mouseX >= 300-width_e/2 && mouseX <= 300+width_e/2) && (mouseY >= 300-height_e/2 && mouseY <= 300+height_e/2)) {
    strokeWeight(3);
    stroke(255);
    ellipse(300, 300, width_e, height_e);
    if (mousePressed) {
      if (typing != "") {
        saved = typing;
        lastValue = typing;
        typing = "";
        atribuition();
      } else if (saved != "" && typing == "") {
        if (saved == "N" || saved == "M" || saved == "P") saved = lastValue;
      }
      println('S');
      println(saved);
      port.write('S');
      port.write(saved);
      delay(250);
    }
  } else if ((mouseX >= 450-width_e/2 && mouseX <= 450+width_e/2) && (mouseY >= 300-height_e/2 && mouseY <= 300+height_e/2)) {
    strokeWeight(3);
    stroke(255);
    ellipse(450, 300, width_e, height_e);
    if (mousePressed) {
      port.write('P');
      port.write('P');
      port.write('P');
      println('P');
      println('P');
      println('P');
      delay(250);
    }
  } else if ((mouseX >= 55 && mouseX <= 95) && (mouseY >= 335 && mouseY <= 365)) {
    fill(10);
    ellipse(70, 350, 10, 10);
    if (mousePressed) {
      state = true;
      delay(300);
    }
  } else if ((mouseX >= 185 && mouseX <= 215) && (mouseY >= 335 && mouseY <= 365)) {
    fill(10);
    ellipse(200, 350, 10, 10);
    if (mousePressed) {
      state = false;
      delay(300);
    }
  }
}

void texto() {
  textFont(f1);
  fill(255);
  text("Historic:", 700, 240);

  textFont(f);
  fill(255);
  text("mm", 200, 305);

  fill(0);
  noStroke();

  if (stateCursor) {
    if (state) text(typing + blinkChar(), indent+20, 300);
    else {
      text("-" + typing + blinkChar(), indent+20, 300);
    }
  }

  for (int i = 0; i < number.length; i++) {
    if (number[i] == "0") text(" ", 710, 280+(i*20));
    else {
      if (float(number[i]) > 0) text(number[i] + " mm", 710, 280+(i*20));
      else if (float(number[i]) < 0) text(number[i] + " mm", 710, 280+(i*20));
      else if (number[i] == "N" || number[i] == "-N") text("Negative", 710, 280+(i*20));
      else if (number[i] == "M" || number[i] == "-M") text("Middle", 710, 280+(i*20));
      else if (number[i] == "P" || number[i] == "-P") text("Positive", 710, 280+(i*20));
    }
  }
}

void keyPressed() {
  if (key == '\n') {
    saved = typing;
    lastValue = typing;
    typing = ""; 
    atribuition();
  } else {
    if (key >= '0' && key <= '9' || key == '.') {
      if (stateCursor) typing = typing + key;
    } else if (key == BACKSPACE) {
      typing = typing.substring(0, max(0, typing.length()-1));
    }
  }
}

void mouseClicked() {
  stateCursor = !stateCursor || ControlCursor;
}

void atribuition() {
  if (state);
  else {
    if (saved == "N" || saved == "M" || saved == "P");
    else saved = "-" + saved;
  }
  if (a < number.length) {
  } else {
    a = 0;
  }
  number[a] = saved;
  a++;
}

void identificador() {
  textFont(f1);
  fill(255);
  noStroke();
  text("POSITIVE", 90, 355);
  text("NEGATIVE", 220, 355);
  text("SETTING:", 50, 270);
  text("LIMIT POSITIONS:", 50, 460);

  fill(0);
  text("GO", 290, 305);
  text("STOP", 430, 305); 
  text("NEGATIVE", 60, 505);
  text("MIDDLE", 220, 505);
  text("POSITIVE", 365, 505);
}

void arduino() {
  if (port.available() > 0) mensagem = port.readString();
  textFont(f1);
  fill(255);
  text("Ultima distancia percorrida:", 500, 550);
  text(mensagem + " mm", 730, 550);
}

String blinkChar() {
  return isFocused && (frameCount>>2 & 1) == 0 ? "|" : "";
}