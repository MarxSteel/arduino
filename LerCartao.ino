
 
#define MAX_BITS 100                 // QUANTIDADE MÁXIMA DE BITS
#define WEIGAND_WAIT_TIME  3000      // TEMPO PARA AGUARDAR OUTRO PULSO DO WIEGAND
 
unsigned char databits[MAX_BITS];    // ARMAZENAR TODOS OS BITS DE DADOS
unsigned char bitCount;              // CONTAGEM DE BITS CAPTURADOS
unsigned char flagDone;              // 
unsigned int weigand_counter;        // CONTAGEM DOS BITS
 
unsigned long facilityCode=0;        // FACILITY CODE DECODIFICADO
unsigned long cardCode=0;            // CARD CODE DECODIFICADO
 
// INTERRUPÇÃO DE LEITURA
void ISR_INT0()
{
  bitCount++;
  flagDone = 0;
  weigand_counter = WEIGAND_WAIT_TIME;  
 
}
 
//INTERRUPÇÃO DE LEITURA
void ISR_INT1()
{
  databits[bitCount] = 1;
  bitCount++;
  flagDone = 0;
  weigand_counter = WEIGAND_WAIT_TIME;  
}
 
void setup()
{
  pinMode(13, OUTPUT);  // LED
  pinMode(2, INPUT);     // DATA0 (INT0)
  pinMode(3, INPUT);     // DATA1 (INT1)
 
  Serial.begin(9600);
  Serial.println("LEITOR RFID");
  attachInterrupt(0, ISR_INT0, FALLING);  
  attachInterrupt(1, ISR_INT1, FALLING);
 
 
  weigand_counter = WEIGAND_WAIT_TIME;
}
 
void loop()
{
  // AGUARDANDO SE NÃO HAVERÁ MAIS PULSOS DE LEITURA ANTES DE TRATAR OS DADOS
  if (!flagDone) {
    if (--weigand_counter == 0)
      flagDone = 1;  
  }
  if (bitCount > 0 && flagDone) {
    unsigned char i;
 
    Serial.print("Read ");
    Serial.print(bitCount);
    Serial.print(" bits. ");
    if (bitCount == 35)
    {
      // TRATANDO 35 BITS: PADRÃO HID
      // FACILITY CODE = BITS DE 2 A 14
      for (i=2; i<14; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
 
      // CARD CODE: BITS DE 15 A 34
      for (i=14; i<34; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }
 
      printBits();
    }
    else if (bitCount == 26)
    {
      // FORMATO PADRÃO - WIEGAND 26 BITS
      // facility code = bits 2 a 9
      for (i=1; i<9; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
 
      // CARD CODE: BITS DE 10 A 23
      for (i=9; i<25; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }
 
      printBits();  
    }
    else {
     Serial.println("Nada a decodificar."); 
    }
 
     //LIMPANDO O TRATAMENTO 
     bitCount = 0;
     facilityCode = 0;
     cardCode = 0;
     for (i=0; i<MAX_BITS; i++) 
     {
       databits[i] = 0;
     }
  }
}
 
void printBits()
{
      Serial.print("FACILITY CODE = ");
      Serial.print(facilityCode);
      Serial.print(", CARD CODE = ");
      Serial.println(cardCode); 
}
