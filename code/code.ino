#include <SoftwareSerial.h> // C:\Users\Hii\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\libraries\SoftwareSerial\src
SoftwareSerial mySerial(7, 8); //Pin6 RX , Pin 7 TX connected to--> Bluetooth TX,RX

const int Sensor1 = 3; // Sensor cua 1, phia duoi, co tac dong -> LOW, khong tac dong -> HIGH
const int Sensor2 = 4; // Sensor cua 2, phia tren, co tac dong -> LOW, khong tac dong -> HIGH, du phong khong dung
const int Sensor3 = 5; // Sensor chot cua, co tac dong -> LOW, khong tac dong -> HIGH
const int Sensor4 = 6; // Sensor tat buzzer2, co tac dong -> LOW, khong tac dong -> HIGH

const int Relay1 = 13; // Relay bat den
const int Relay2 = 12; // Relay bat quat
const int Relay4 = 11; // Relay bat may tinh
const int Buzzer = 10;
const int Coi    = 9;

#define BIP_1      1
#define BIP_2      2
#define BIP_LONG   255

uint8_t isProtected = 0;
bool isFirstRun = true;
bool isFirstRun2 = true;
uint8_t temp = 0;
bool previousMillisSet = true;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const unsigned long interval = 12000; // 12 giay
const unsigned long interval2 = 5000; // 5 giay
char val = ' ';
String statusRelay1,statusRelay2, data;
const uint8_t PASSWORD_VALUE[2] = {3,1}; // mang luu gia tri mat khau dung
uint8_t password_input[2]; // mang luu gia tri mat khau nhap
uint8_t i;
bool control_mode = false; // false che do dieu khien dang tat, true che do dieu khien dang bat
bool sensor3_previous_state; // HIGH -> true, LOW -> false
bool sensor3_current_state; // HIGH -> true, LOW -> false
bool sensor4_previous_state; // HIGH -> true, LOW -> false
bool sensor4_current_state; // HIGH -> true, LOW -> false

void control_buzzer();

void control_buzzer(uint8_t para) {
    if (para == BIP_2) {
        digitalWrite(Buzzer, HIGH);
        delay(300);
        digitalWrite(Buzzer, LOW);
        delay(300);
        digitalWrite(Buzzer, HIGH);
        delay(300);
        digitalWrite(Buzzer, LOW);
}
    if (para == BIP_1) {
        digitalWrite(Buzzer, HIGH);
        delay(300);
        digitalWrite(Buzzer, LOW);
    }
    if (para == BIP_LONG) {
        digitalWrite(Buzzer, HIGH);
        delay(1500); // Cho 1.5 giay
        digitalWrite(Buzzer, LOW);
    }
}

// Ham bat xung canh xuong, tra ve gia tri true neu co xung canh xuong, nguoc lai tra ve false
bool down_edge_detector(int pin) {
    if (pin == 5) { // Sensor3
        if ((sensor3_previous_state == true) && (sensor3_current_state == false)) { return true; }
        else { return false; }
    }
    if (pin == 6) { // Sensor4
        if ((sensor4_previous_state == true) && (sensor4_current_state == false)) { return true; }
        else { return false; }
    }
}

void setup() {
    pinMode(Sensor1, INPUT);
    pinMode(Sensor2, INPUT);
    pinMode(Sensor3, INPUT);
    pinMode(Sensor4, INPUT);
    
    pinMode(Relay1, OUTPUT);
    pinMode(Relay2, OUTPUT);
    pinMode(Relay4, OUTPUT);
    pinMode(Buzzer, OUTPUT);
    pinMode(Coi, OUTPUT);

    mySerial.begin(9600);
    Serial.begin(9600);

    digitalWrite(Relay1, LOW);
    digitalWrite(Relay2, LOW);
    digitalWrite(Relay4, LOW);
    digitalWrite(Buzzer, LOW);
    digitalWrite(Coi, LOW);
    statusRelay1 = "A"; // Trang thai A: Tat, 1: Bat
    statusRelay2 = "B"; // Trang thai B: Tat, 2: Bat
    data = "";
    i = 0;

    // Phat am thanh qua buzzer de bao he thong khoi dong
    control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
    delay(5000); // Cho 5 giay de nguoi dung co the set up he thong

}

void loop() {

    delay(5000);//////////////////////////////////////////////////
    if (isFirstRun2) {
        sensor3_previous_state = digitalRead(Sensor3);
        sensor4_previous_state = digitalRead(Sensor4);
        isFirstRun2 = false;
    }
    }
    sensor3_current_state = digitalRead(Sensor3);
    sensor4_current_state = digitalRead(Sensor4);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (isProtected == 0) {
        if (down_edge_detector(Sensor4)) { Serial.println("dang chay 1"); //111111111111111111111111111111111111111111111111111111111;
         temp++; }
        if (temp = 3) {
            delay(500);
            isProtected = 2;
            control_buzzer(BIP_1); // Kich hoat buzzer 1 bip
            temp = 0;
        }
    }

    if (isProtected == 2) {
        Serial.println("dang chay 2"); //2222222222222222222222222222222222222222222222222222
        if (down_edge_detector(Sensor4)) { temp++; }
        if (temp = 3) {
            delay(500);
            isProtected = 0;
            control_buzzer(BIP_LONG); // Kich hoat buzzer bao tat chuc nang bao ve
            temp = 0;
        }
    }

    // Kiem tra trang thai bien isProtected
    if (digitalRead(Sensor4) == LOW) {
        Serial.println("dang chay 3"); //3333333333333333333333333333333333333333333333333
        while (millis() >= (4294967295 - interval2)) {} // Cho den khi millis khong bi tran
        previousMillis = millis();
        while (digitalRead(Sensor4) == LOW) {} // Cho den khi khong con tac dong
        currentMillis = millis();
        if (currentMillis - previousMillis >= interval2) {
            if (isProtected == 0) { control_buzzer(BIP_LONG); } // Kich hoat buzzer bao tat chuc nang bao ve
            else if (isProtected == 1) { control_buzzer(BIP_2); } // Kich hoat buzzer 2 bip
            else if (isProtected == 2) { control_buzzer(BIP_1); } // Kich hoat buzzer 1 bip
        }
    }

    // Phan code tu dong tat den
    if ((digitalRead(Sensor1) == LOW) && (digitalRead(Sensor3) == HIGH) && (isProtected == 0)) {
        Serial.println("dang chay 4"); //444444444444444444444444444444444444444444444444444444
        isProtected = 1;
        control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
        delay(5000); // Cho 5 giay
        digitalWrite(Relay1, LOW); // Tat den
    }

    // Phan code dieu khien coi, den va may tinh

    if ((digitalRead(Sensor1) == HIGH) && (isProtected == 1)) {
        Serial.println("dang chay 5"); //55555555555555555555555555555555555555555555555555

        if (previousMillisSet) {
            while (millis() >= (4294967295 - interval)) {} // Cho den khi millis khong bi tran
            previousMillis = millis();
            previousMillisSet = false;
        }

        if (down_edge_detector(Sensor4)) { temp++; }

        if (temp = 3) {
            delay(500);
            isProtected = 0;
            control_buzzer(BIP_LONG); // Kich hoat buzzer bao tat chuc nang bao ve
            temp = 0;
            digitalWrite(Relay1, HIGH); // Bat den
            previousMillisSet = true;
        }

        currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {

            isProtected = 1;

            if (isFirstRun == true) {

                digitalWrite(Relay4, HIGH);
                delay(800); // Cho 800 ms
                digitalWrite(Relay4, LOW); // Bat may tinh

                digitalWrite(Coi, HIGH); // Kich hoat Coi
                delay(30000); // Cho 30 giay
                digitalWrite(Coi, LOW); // Tat coi
                delay(10000); // Cho 10 giay
                isFirstRun = false;
            }

            if (!isFirstRun) {
                digitalWrite(Coi, HIGH); // Kich hoat Coi
                delay(5000); // Cho 5 giay
                digitalWrite(Coi, LOW); // Tat coi
                delay(60000); // Cho 60 giay
            }
            previousMillisSet = true;
        }
    }

    if ((digitalRead(Sensor1) == LOW) && down_edge_detector(Sensor3))
    {
        Serial.println("dang chay 6"); //666666666666666666666666666666666666666666666666666
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
        control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
    }
    
    val=' ';

    ///////////////////////////////////END DK RELAY BANG HC05//////////////////////////////////////

    ///////////////////////////////////DK COI QUA CONG SERIAL MAY TINH///////////////////////////////////////

    if (Serial.available())
    {
        data=Serial.readStringUntil('\r');
        if (data == "batcoi")
        {
            digitalWrite(Coi, HIGH);
            Serial.println("Dabatcoi");
            delay(25000); // Cho 25 giay
            digitalWrite(Coi, LOW);
            Serial.println("Datatcoi");
            delay(10000); // Cho 10 giay
        }
    }

    ///////////////////////////////END DK COI QUA CONG SERIAL MAY TINH///////////////////////////////////////

    // Cap nhat trang thai bien
    sensor3_previous_state = sensor3_current_state;
    sensor4_previous_state = sensor4_current_state;

}