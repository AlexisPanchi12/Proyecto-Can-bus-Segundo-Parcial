#include <SPI.h>
#include <mcp_can.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ANCHO 128
#define ALTO 64
#define CAN_CS 10

MCP_CAN CAN(CAN_CS);
Adafruit_SSD1306 display(ANCHO, ALTO, &Wire);

// ===== VARIABLES =====
unsigned long tiempoAusenteInicio = 0;
unsigned long ultimoParpadeo = 0;
unsigned long ultimoRefresco = 0;

bool estadoParpadeo = false;
byte animacion = 0;

byte ir = 0;
byte ruido = 0;

// ===== SETUP =====
void setup() {

  Serial.begin(9600);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(15, 28);
  display.print("Iniciando sistema...");
  display.display();

  // CAN
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    display.clearDisplay();
    display.setCursor(30, 30);
    display.print("ERROR CAN");
    display.display();
    while (1);
  }

  CAN.setMode(MCP_NORMAL);
  delay(1500);

  tiempoAusenteInicio = millis();
}

// ===== LOOP =====
void loop() {

  long unsigned int id;
  byte len;
  byte buf[8];

  // ----- Leer CAN -----
  if (CAN.checkReceive() == CAN_MSGAVAIL) {
    CAN.readMsgBuf(&id, &len, buf);

    ir = buf[0];
    ruido = buf[1];
  }

  // Refresco cada 100 ms
  if (millis() - ultimoRefresco < 100) return;
  ultimoRefresco = millis();

  // ===== DIBUJO =====
  display.clearDisplay();

  // TITULO
  display.setCursor(0, 0);
  display.print("MONITOREO CONDUCTOR");

  // ===== ESTADO =====
  display.setCursor(0, 12);

  if (ir == 1) {

    display.print("Estado: ACTIVO");
    tiempoAusenteInicio = millis();
    display.fillCircle(120, 14, 3, WHITE);

    // Animacion volante
    byte cx = 64;
    byte cy = 26;
    byte r  = 6;

    display.drawCircle(cx, cy, r, WHITE);

    if (animacion == 0)
      display.drawLine(cx, cy - r, cx, cy + r, WHITE);
    else if (animacion == 1)
      display.drawLine(cx - r, cy - r, cx + r, cy + r, WHITE);
    else if (animacion == 2)
      display.drawLine(cx - r, cy, cx + r, cy, WHITE);
    else
      display.drawLine(cx - r, cy + r, cx + r, cy - r, WHITE);

    animacion++;
    if (animacion > 3) animacion = 0;

  } else {

    display.print("Estado: AUSENTE");

    if (millis() - ultimoParpadeo > 400) {
      estadoParpadeo = !estadoParpadeo;
      ultimoParpadeo = millis();
    }

    if (estadoParpadeo)
      display.fillCircle(120, 14, 3, WHITE);
    else
      display.drawCircle(120, 14, 3, WHITE);

    unsigned long segundos = (millis() - tiempoAusenteInicio) / 1000;

    display.setCursor(0, 22);
    display.print("Sin conductor: ");
    display.print(segundos);
    display.print(" s");
  }

  // ===== BARRA DE RUIDO =====
  display.setCursor(0, 42);
  display.print("Ruido:");

  byte barra = map(ruido, 0, 255, 0, 90);
  display.drawRect(38, 44, 90, 6, WHITE);
  display.fillRect(38, 44, barra, 6, WHITE);

  display.display();
}