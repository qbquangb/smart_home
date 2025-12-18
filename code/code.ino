#include <SoftwareSerial.h> // C:\Users\Hii\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\libraries\SoftwareSerial\src
SoftwareSerial mySerial(7, 8); //Pin6 RX , Pin 7 TX connected to--> Bluetooth TX,RX

const int Sensor1 = 3; // Sensor cua
const int Sensor2 = 4; // Sensor cua 2
const int Sensor3 = 5; // Sensor chot cua, co tac dong -> LOW, khong tac dong -> HIGH
const int Sensor4 = 6; // Sensor tat buzzer2, co tac dong -> LOW, khong tac dong -> HIGH

const int Relay1 = 13; // Relay bat den
const int Relay2 = 12; // Relay bat quat
const int Relay4 = 11; // Relay bat may tinh
const int Buzzer = 10;
const int Coi    = 9;

#define BIP_1      1
#define BIP_2      2

static bool protected = false;
unsigned int temp = 0;
bool previousMillisSet = true;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const unsigned long interval = 12000; // 12 giay
char val = ' ';
String statusRelay1,statusRelay2;
const unsigned int8 PASSWORD_VALUE[2] = {3,1}; // mang luu gia tri mat khau dung
unsigned int8 password_input[2]; // mang luu gia tri mat khau nhap
unsigned int8 i;
bool control_mode = false; // false che do dieu khien dang tat, true che do dieu khien dang bat

void control_buzzer();

void control_buzzer(unsigned int para) {
    if (para == BIP_2) {
        digitalWrite(Buzzer2, HIGH);
        delay(300);
        digitalWrite(Buzzer2, LOW);
        delay(300);
        digitalWrite(Buzzer2, HIGH);
        delay(300);
        digitalWrite(Buzzer2, LOW);
}
    if (para == BIP_1) {
        digitalWrite(Buzzer2, HIGH);
        delay(300);
        digitalWrite(Buzzer2, LOW);
    }
}

void setup() {
    pinMode(Sensor1, INPUT);
    pinMode(Sensor2, INPUT);
    pinMode(Sensor3, INPUT);
    pinMode(Sensor4, INPUT);
    
    pinMode(Relay1, OUTPUT);
    pinMode(Relay2, OUTPUT);
    pinMode(Buzzer, OUTPUT);
    pinMode(Coi, OUTPUT);

    mySerial.begin(9600);
    Serial.begin(9600);

    digitalWrite(Relay1, LOW);
    digitalWrite(Relay2, LOW);
    digitalWrite(Buzzer, LOW);
    digitalWrite(Coi, LOW);
    statusRelay1 = "A"; // Trang thai A: Tat, 1: Bat
    statusRelay2 = "B"; // Trang thai B: Tat, 2: Bat
}

void loop() {

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Phan code tu dong tat den sau 60s
    if ((digitalRead(Sensor1) == HIGH) && (digitalRead(Sensor2) == HIGH) && (digitalRead(Sensor3) == LOW)) {
        protected = true;
        control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
        delay(7000); // Cho 7 giay
        digitalWrite(Relay1, LOW); // Tat den
    }

    // Phan code dieu khien buzzer, den va may tinh

    if ((digitalRead(Sensor1) == LOW) && (digitalRead(Sensor2) == LOW) && (protected == true)) {

        if previousMillisSet {
            previousMillis = millis();
            previousMillisSet = false;
        }

        if (digitalRead(Sensor4) == LOW) {
            delay(10); // Debounce
            temp++;
        }
        if (temp >= 3) {
            protected = false;
            temp = 0;
            digitalWrite(Relay1, HIGH); // Bat den
        }

        currentMillis = millis();
        if (unsigned long)(currentMillis - previousMillis >= interval) {

            protected = false;

            digitalWrite(Relay4, HIGH);
            delay(1000); // Cho 1 giay
            digitalWrite(Relay4, LOW); // Bat may tinh

            digitalWrite(Coi, HIGH); // Kich hoat Coi
            delay(30000); // Cho 30 giay
            digitalWrite(Coi, LOW); // Tat coi
        }
    }

    if ((digitalRead(Sensor1) == HIGH) && (digitalRead(Sensor2) == HIGH) && (digitalRead(Sensor3) == HIGH))
    {
        protected = false;
        previousMillisSet = true;
        temp = 0;
        control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////DK RELAY BANG HC05//////////////////////////////////////////

    if( mySerial.available() > 0 ) {
        val = mySerial.read();
        delay(200);
    }

    // Xac nhan mat khau dieu khien
    if ( control_mode == false && val == '8' ) { // nhan 8 de bat dau nhap mat khau
        control_buzzer(BIP_1); // Kich hoat buzzer 1 bip
        for ( i = 0; i < 2; i++ ) {
            while( mySerial.available() == 0 ) {} // Cho den khi co du lieu
            password_input[i] = mySerial.read() - '0'; // Chuyen ky tu sang so
            control_buzzer(BIP_1); // Kich hoat buzzer 1 bip
        }

        // Kiem tra mat khau
        control_mode = true;
        for ( i = 0; i < 2; i++ ) {
            if ( password_input[i] != PASSWORD_VALUE[i] ) {
                control_mode = false;
                break;
            }
        }

        // Bao hieu ket qua xac nhan mat khau
        if( control_mode == true ) {
            delay(300);
            control_buzzer(BIP_1); // Kich hoat buzzer 2 bip
        }
    }
    
    // Che do dieu khien
    if (control_mode == true)
    {
        // Relay 1 on
        if( val == '1' ) {
            digitalWrite(Relay1,HIGH); statusRelay1="1"; }
        // Relay 2 on
        else if( val == '2' ) {
            digitalWrite(Relay2,HIGH); statusRelay2="2";}
        // Relay 1 off
        else if( val == 'A' ) {
            digitalWrite(Relay1,LOW); statusRelay1="A";}
        // Relay 2 off
        else if( val == 'B' ) {
            digitalWrite(Relay2,LOW); statusRelay2="B";}
    }

    // Thoat che do dieu khien
    if ( val == '9' && control_mode == true ) { // nhan 9 de thoat che do dieu khien
        control_mode = false;
        delay(300);
        control_buzzer(BIP_1); // Kich hoat buzzer 1 bip
    }
    
    val=' ';

    ///////////////////////////////////END DK RELAY BANG HC05//////////////////////////////////////
}