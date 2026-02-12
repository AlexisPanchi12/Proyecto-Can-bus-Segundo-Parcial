#include <SPI.h>
#include <mcp_can.h>

MCP_CAN CAN(10);

int sensorIR = 3;
int sensorSonido = A0;

void setup() {
  pinMode(sensorIR, INPUT);
  Serial.begin(9600);

  CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ);
  CAN.setMode(MCP_NORMAL);
}

void loop() {

  byte data[2];

  data[0] = digitalRead(sensorIR);
  data[1] = analogRead(sensorSonido) / 4;

  CAN.sendMsgBuf(0x100, 0, 2, data);

  delay(200);
}
