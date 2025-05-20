int led1 = 8;      // Pin del primer LED
int led2 = 9;      // Pin del segundo LED
int led3 = 10;     // Pin del tercer LED
int led4 = 11;     // Pin del cuarto LED
int pirPin = 3;    // Sensor de movimiento en digital 3
int buzzerPin = 7; // Buzzer en digital 7

// Variables para sensores
int valorLDR1 = 0;  // Valor del primer LDR
int valorLDR2 = 0;  // Valor del segundo LDR
int valorLDR3 = 0;  // Valor del tercer LDR
int valorLDR4 = 0;  // Valor del cuarto LDR

// Variables para control de movimiento y tiempo
bool hayMovimiento = false;
bool movimientoAnterior = false;     // Para detectar cambios en el estado de movimiento
int contadorMovimientos = 0;         // Contador de movimientos (0, 1, 2)
unsigned long tiempoUltimoMovimiento = 0;   // Tiempo de la última detección de movimiento
unsigned long tiempoEncendidoLed1 = 0;
unsigned long tiempoEncendidoLed2 = 0;
unsigned long tiempoEncendidoLed3 = 0;
unsigned long tiempoEncendidoLed4 = 0;
bool led1Encendido = true;          // LEDs encendidos por defecto
bool led2Encendido = true;          // LEDs encendidos por defecto
bool led3Encendido = true;          // LEDs encendidos por defecto
bool led4Encendido = true;          // LEDs encendidos por defecto


// Variables para control del buzzer
bool buzzerActivo = false;
unsigned long tiempoInicioBuzzer = 0;
const unsigned long DURACION_BUZZER = 3000;      // Duración del sonido del buzzer (5 segundos)
const unsigned long TIEMPO_ENTRE_MOVIMIENTOS = 10000;  // Tiempo máximo entre movimientos consecutivos

void setup() {
  pinMode(led1, OUTPUT);      // Configurar el primer LED como salida
  pinMode(led2, OUTPUT);      // Configurar el segundo LED como salida
  pinMode(led3, OUTPUT);      // Configurar el tercer LED como salida
  pinMode(led4, OUTPUT);      // Configurar el cuarto LED como salida
  pinMode(pirPin, INPUT);     // Configurar el sensor PIR como entrada
  pinMode(buzzerPin, OUTPUT); // Configurar el buzzer como salida
  
  // Encender todos los LEDs por defecto
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led4, HIGH);
  digitalWrite(buzzerPin, HIGH);  // Inicializar buzzer como HIGH (apagado)
  
  Serial.begin(9600);         // Inicializar el monitor serial
  Serial.println("Sistema iniciado - Esperando movimiento - LEDs encendidos por defecto");
}

void loop() {
  // Leer valores de los LDRs
  valorLDR1 = analogRead(A4); // LDR conectado a A4
  valorLDR2 = analogRead(A1); // LDR conectado a A1
  valorLDR3 = analogRead(A3); // LDR conectado a A3
  valorLDR4 = analogRead(A2); // LDR conectado a A2
  
  // Guardar el estado anterior de movimiento
  movimientoAnterior = hayMovimiento;
  
  // Leer sensor de movimiento
  hayMovimiento = digitalRead(pirPin);
  
  // Detección de movimiento y control de contadores
  detectarMovimientos();
  
  // Manejar el control del buzzer (si está activo)
  manejarBuzzer();
  
  // Control de LEDs
  controlarLEDs();
  
  // Mostrar valores en el monitor serial
  imprimirDatosSerial();
  
  delay(100); // Pausa entre lecturas
}

// Función para detectar movimientos y manejar los contadores
void detectarMovimientos() {
  // Flanco ascendente - Se acaba de detectar movimiento
  if (hayMovimiento && !movimientoAnterior) {
    tiempoUltimoMovimiento = millis();
    
    // Reiniciar el tiempo de encendido de los LEDs cuando se detecta cualquier movimiento
    reiniciarTiempoEncendido();
    
    // Verificar si es un movimiento dentro del tiempo de detección
    if (contadorMovimientos == 0) {
      // Primer movimiento detectado
      contadorMovimientos = 1;
      Serial.println("PRIMER MOVIMIENTO DETECTADO - Tiempo de LEDs reiniciado a 10s");
    } 
    else {
      // Ya hubo un primer movimiento, verificar si está dentro del tiempo
      if (millis() - tiempoUltimoMovimiento <= TIEMPO_ENTRE_MOVIMIENTOS) {
        // Incrementar contador, si es el segundo o subsiguiente, activar alarma
        contadorMovimientos++;
        
        Serial.println("MOVIMIENTO ADICIONAL DETECTADO - Tiempo de LEDs reiniciado a 10s");
        
        if (contadorMovimientos >= 2) {
          Serial.print("MOVIMIENTO CONSECUTIVO #");
          Serial.print(contadorMovimientos);
          Serial.println(" DETECTADO - ACTIVANDO ALARMA");
          activarBuzzer();
          // Después de activar el buzzer, reestablecemos el contador a 1 (no a 0)
          contadorMovimientos = 1;
        }
      }
    }
  }
  
  // Si ha pasado el tiempo sin movimiento, reiniciar contador
  if (!hayMovimiento && (millis() - tiempoUltimoMovimiento > TIEMPO_ENTRE_MOVIMIENTOS)) {
    if (contadorMovimientos > 0) {
      contadorMovimientos = 0;
      Serial.println("REINICIO DE CONTADOR: No se detectó movimiento por 10 segundos");
    }
  }
}

// Función para activar el buzzer
void activarBuzzer() {
  buzzerActivo = true;
  tiempoInicioBuzzer = millis();
  digitalWrite(buzzerPin, LOW);  // Activar buzzer (activo a nivel bajo)
  Serial.println("BUZZER ACTIVADO - Sonará por 3 segundos");
}

// Función para reiniciar el tiempo de encendido de todos los LEDs
void reiniciarTiempoEncendido() {
  unsigned long tiempoActual = millis();
  tiempoEncendidoLed1 = tiempoActual;
  tiempoEncendidoLed2 = tiempoActual;
  tiempoEncendidoLed3 = tiempoActual;
  tiempoEncendidoLed4 = tiempoActual;
  Serial.println("TIEMPOS DE ENCENDIDO DE LEDS REINICIADOS");
}

// Función para manejar el estado del buzzer
void manejarBuzzer() {
  if (buzzerActivo) {
    if (millis() - tiempoInicioBuzzer >= DURACION_BUZZER) {
      digitalWrite(buzzerPin, HIGH);  // Apagar buzzer (activo a nivel bajo)
      buzzerActivo = false;
      
      Serial.println("BUZZER APAGADO - Contador en 1, esperando siguiente movimiento");
    }
  }
}

// Función para controlar todos los LEDs
void controlarLEDs() {
  controlarLED(led1, led1Encendido, tiempoEncendidoLed1, valorLDR1);
  controlarLED(led2, led2Encendido, tiempoEncendidoLed2, valorLDR2);
  controlarLED(led3, led3Encendido, tiempoEncendidoLed3, valorLDR3);
  controlarLED(led4, led4Encendido, tiempoEncendidoLed4, valorLDR4);
}

// Función para controlar un LED individual
void controlarLED(int pinLed, bool &ledEncendido, unsigned long &tiempoEncendido, int valorLDR) {
  unsigned long tiempoActual = millis();
  
  if (ledEncendido) {
    // Si el LED está encendido, solo permitir apagarlo si han pasado al menos 10 segundos
    if ((tiempoActual - tiempoEncendido >= 10000) && 
        (valorLDR < 300 || (valorLDR > 750 && !hayMovimiento))) {
      digitalWrite(pinLed, LOW);  // Apagar LED
      ledEncendido = false;
      Serial.print("LED en pin ");
      Serial.print(pinLed);
      Serial.println(" apagado después de 10s");
    }
  } else {
    // Si el LED está apagado y hay movimiento y el valor del LDR es adecuado, encenderlo
    if (valorLDR >= 750 && valorLDR <= 1200 && hayMovimiento) {
      digitalWrite(pinLed, HIGH);  // Encender LED
      ledEncendido = true;
      tiempoEncendido = tiempoActual;  // Guardar el tiempo en que se encendió
      Serial.print("LED en pin ");
      Serial.print(pinLed);
      Serial.println(" encendido");
    }
  }
}

// Función para imprimir datos en el monitor serial
void imprimirDatosSerial() {
  // Imprimir valores de sensores con menor frecuencia para no sobrecargar
  static unsigned long ultimaImpresion = 0;
  if (millis() - ultimaImpresion >= 1000) {  // Imprimir cada segundo
    ultimaImpresion = millis();
    
    Serial.print("LDR1: ");
    Serial.print(valorLDR1);
    Serial.print(" | LDR2: ");
    Serial.print(valorLDR2);
    Serial.print(" | LDR3: ");
    Serial.print(valorLDR3);
    Serial.print(" | LDR4: ");
    Serial.print(valorLDR4);
    Serial.print(" | Movimiento: ");
    Serial.print(hayMovimiento);
    Serial.print(" | Contador: ");
    Serial.print(contadorMovimientos);
    
    // Mostrar tiempo desde último movimiento
    if (contadorMovimientos > 0) {
      Serial.print(" | T desde último mov: ");
      Serial.print((millis() - tiempoUltimoMovimiento) / 1000.0);
      Serial.print("s");
    }
    
    // Mostrar estado de LEDs
    Serial.print(" | LEDs: ");
    Serial.print(led1Encendido ? "1" : "0");
    Serial.print(led2Encendido ? "1" : "0");
    Serial.print(led3Encendido ? "1" : "0");
    Serial.print(led4Encendido ? "1" : "0");
    
    // Mostrar tiempo restante de encendido para los LEDs
    if (led1Encendido || led2Encendido || led3Encendido || led4Encendido) {
      Serial.print(" | Tiempo restante LEDs: ");
      unsigned long tiempoActual = millis();
      if (led1Encendido) {
        float tiempoRestante = 10.0 - (tiempoActual - tiempoEncendidoLed1) / 1000.0;
        if (tiempoRestante < 0) tiempoRestante = 0;
        Serial.print(tiempoRestante);
        Serial.print("s ");
      }
    }
    
    Serial.println();
  }
}