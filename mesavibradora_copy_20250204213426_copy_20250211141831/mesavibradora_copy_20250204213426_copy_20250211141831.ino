#include <Arduino.h>
#include <math.h>

// Definición de pines
constexpr uint8_t L_EN = 2;
constexpr uint8_t R_EN = 4;
constexpr uint8_t L_PWM = 18;
constexpr uint8_t R_PWM = 19;

// Valores de PWM calibrados
constexpr uint8_t PWM_MIN = 20;
constexpr uint8_t PWM_MAX = 255;

// Definición de velocidades PWM
constexpr uint8_t LOW_SPEED = 100;
constexpr uint8_t MEDIUM_SPEED = 150;
constexpr uint8_t HIGH_SPEED = 200;
constexpr uint8_t STOP = 0;

// Variable de velocidad actual
uint8_t velocidad_actual = STOP;

// Enumeración de comandos
enum Comando {
    ADELANTE = 'F',
    ATRAS = 'B',
    DETENER = 'S',
    VELOCIDAD_BAJA = '1',
    VELOCIDAD_MEDIA = '2',
    VELOCIDAD_ALTA = '3',
    AUMENTAR_VELOCIDAD = '+',
    DISMINUIR_VELOCIDAD = '-',
    ESTABLECER_RPM = 'R',
    ESTABLECER_PWM = 'P'
};

// Declaración de funciones
void ejecutarComando(Comando command);
void controlMotor(bool l_enState, bool r_enState, int l_pwmValue, int r_pwmValue);
void detenerMotor();
float calcularRPM(float pwm);
int convertirRPMaPWM(int rpmDeseadas);
void setVelocidad(uint8_t velocidad);

void setup() {
    Serial.begin(115200);
    pinMode(L_EN, OUTPUT);
    pinMode(R_EN, OUTPUT);
    pinMode(L_PWM, OUTPUT);
    pinMode(R_PWM, OUTPUT);
    detenerMotor();
    Serial.println("[Sistema listo]");
}

void loop() {
    if (Serial.available()) {
        char command = Serial.read();
        ejecutarComando(static_cast<Comando>(command));
    }
}

void ejecutarComando(Comando command) {
    switch (command) {
        case ADELANTE:
            controlMotor(HIGH, LOW, velocidad_actual, STOP);
            Serial.println("[Motor adelante]");
            break;
        case ATRAS:
            controlMotor(LOW, HIGH, STOP, velocidad_actual);
            Serial.println("[Motor atrás]");
            break;
        case DETENER:
            detenerMotor();
            Serial.println("[Motor detenido]");
            break;
        case VELOCIDAD_BAJA:
            setVelocidad(LOW_SPEED);
            Serial.println("[Velocidad baja]");
            break;
        case VELOCIDAD_MEDIA:
            setVelocidad(MEDIUM_SPEED);
            Serial.println("[Velocidad media]");
            break;
        case VELOCIDAD_ALTA:
            setVelocidad(HIGH_SPEED);
            Serial.println("[Velocidad alta]");
            break;
        case AUMENTAR_VELOCIDAD:
            setVelocidad(min(velocidad_actual + 25, PWM_MAX));
            Serial.printf("[Velocidad aumentada a: %d]\n", velocidad_actual);
            break;
        case DISMINUIR_VELOCIDAD:
            setVelocidad(max(velocidad_actual - 25, PWM_MIN));
            Serial.printf("[Velocidad disminuida a: %d]\n", velocidad_actual);
            break;
        case ESTABLECER_RPM: {
            String rpmStr = Serial.readStringUntil('\n');
            int rpmDeseadas = rpmStr.toInt();
            int pwmCorregido = convertirRPMaPWM(rpmDeseadas);
            if (pwmCorregido != -1) {
                setVelocidad(pwmCorregido);
                Serial.printf("[RPM establecidas a: %d]\n", rpmDeseadas);
            } else {
                Serial.println("[Error: RPM fuera de rango]");
            }
            break;
        }
        case ESTABLECER_PWM: {
            String pwmStr = Serial.readStringUntil('\n');
            int pwmValor = pwmStr.toInt();
            if (pwmValor >= PWM_MIN && pwmValor <= PWM_MAX) {
                setVelocidad(pwmValor);
                Serial.printf("[PWM establecido a: %d]\n", pwmValor);
            } else {
                Serial.println("[Error: PWM fuera de rango]");
            }
            break;
        }
        default:
            Serial.println("[Comando no válido]");
            break;
    }
}

void setVelocidad(uint8_t velocidad) {
    velocidad_actual = velocidad;
    controlMotor(HIGH, LOW, velocidad_actual, STOP);
}

float calcularRPM(float pwm) {
    return -0.375 * pow(pwm, 4) + 4.5833 * pow(pwm, 3) - 19.375 * pow(pwm, 2) + 51.095 * pwm - 21;
}

int convertirRPMaPWM(int rpmDeseadas) {
    float xMin = PWM_MIN;
    float xMax = PWM_MAX;
    float tolerancia = 1.0;
    float x = 0;

    for (int i = 0; i < 100; i++) {
        x = (xMin + xMax) / 2.0;
        float y = calcularRPM(x);

        if (abs(y - rpmDeseadas) < tolerancia) {
            return static_cast<int>(x);
        } else if (y < rpmDeseadas) {
            xMin = x;
        } else {
            xMax = x;
        }
    }
    return -1;
}

void controlMotor(bool l_enState, bool r_enState, int l_pwmValue, int r_pwmValue) {
    digitalWrite(L_EN, l_enState);
    digitalWrite(R_EN, r_enState);
    analogWrite(L_PWM, l_pwmValue);
    analogWrite(R_PWM, r_pwmValue);
}

void detenerMotor() {
    controlMotor(LOW, LOW, STOP, STOP);
    velocidad_actual = STOP;
}
