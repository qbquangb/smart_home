#include <SoftwareSerial.h> // C:\Users\Hii\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\libraries\SoftwareSerial\src
#include <avr/wdt.h>
#include <string.h>
SoftwareSerial mySerial(7, 8); //Pin7 RX , Pin 8 TX connected to--> Bluetooth TX,RX

const int Sensor1 = 3; // Sensor cua 1, phia duoi, co tac dong -> LOW, khong tac dong -> HIGH
const int Relay1  = 4; // Relay bat den
const int Sensor3 = 5; // Sensor chot cua, co tac dong -> LOW, khong tac dong -> HIGH, da thay doi sang cam bien cua
const int Sensor4 = 6; // Sensor tat buzzer2, co tac dong -> LOW, khong tac dong -> HIGH, da thay doi sang cam bien cua

const int Relay2 = 12; // Relay bat quat
const int Relay4 = 11; // Relay bat may tinh
const int Buzzer = 10;
const int Coi    = 9;

const int Relay3 = 2; // Relay bat loa

#define BIP_1      1
#define BIP_2      2
#define BIP_3      3
#define BIP_4      4 // Do dac diem cua mach, BIP_4 thay the cho BIP_LONG

bool isProtected = false;
bool isFirstRun = true;
bool isFirstRun2 = true;
uint8_t temp = 0;
bool previousMillisSet = true;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const unsigned long interval = 12000; // 12 giay
const unsigned long interval2 = 5000; // 5 giay
char val = ' ';
String statusRelay1,statusRelay2,statusRelay3, data;
const uint8_t PASSWORD_VALUE[2] = {3,1}; // mang luu gia tri mat khau dung
uint8_t password_input[2]; // mang luu gia tri mat khau nhap
uint8_t i;
bool control_mode = false; // false che do dieu khien dang tat, true che do dieu khien dang bat
static int sensor3_previous_state; // HIGH -> true, LOW -> false
static int sensor3_current_state; // HIGH -> true, LOW -> false
static int sensor4_previous_state; // HIGH -> true, LOW -> false
static int sensor4_current_state; // HIGH -> true, LOW -> false
static int sensor1_previous_state; // HIGH -> true, LOW -> false
static int sensor1_current_state; // HIGH -> true, LOW -> false

void control_buzzer(uint8_t para);
int down_edge_detector_Sensor3();
int down_edge_detector_Sensor4();
int down_edge_detector_Sensor1();
int digitalReadAdj(int pin);
bool isValidChar(char c);
char getValidCharFromHC05();
void resetArduino();

void resetArduino() {
    wdt_enable(WDTO_15MS); // Kích hoạt Watchdog Timer với thời gian 15ms
    while (1); // Chờ Watchdog Timer reset Arduino
}

void control_buzzer(uint8_t para) {
    if (para == BIP_2) {
        digitalWrite(Buzzer, HIGH);
        delay(500);
        digitalWrite(Buzzer, LOW);
        delay(2000);
        digitalWrite(Buzzer, HIGH);
        delay(500);
        digitalWrite(Buzzer, LOW);
        delay(2000);
}
    if (para == BIP_1) {
        digitalWrite(Buzzer, HIGH);
        delay(500);
        digitalWrite(Buzzer, LOW);
        delay(2000);
    }
    if (para == BIP_3)
    {
        digitalWrite(Buzzer, HIGH);
        delay(500);
        digitalWrite(Buzzer, LOW);
        delay(2000);
        digitalWrite(Buzzer, HIGH);
        delay(500);
        digitalWrite(Buzzer, LOW);
        delay(2000);
        digitalWrite(Buzzer, HIGH);
        delay(500);
        digitalWrite(Buzzer, LOW);
        delay(2000);
    }
    if (para == BIP_4)
    {
        digitalWrite(Buzzer, HIGH);
        delay(500);
        digitalWrite(Buzzer, LOW);
        delay(2000);
        digitalWrite(Buzzer, HIGH);
        delay(500);
        digitalWrite(Buzzer, LOW);
        delay(2000);
        digitalWrite(Buzzer, HIGH);
        delay(500);
        digitalWrite(Buzzer, LOW);
        delay(2000);
        digitalWrite(Buzzer, HIGH);
        delay(500);
        digitalWrite(Buzzer, LOW);
        delay(2000);
    }
}

int digitalReadAdj(int pin) { // Tra ve gia tri 0 hoac 1 sau khi loc nhieu lan
    int sum = 0;
    for (int i = 0; i < 10; i++) { // Đọc giá trị 10 lần
        sum += digitalRead(pin);
        delay(5); // Đợi 5ms giữa các lần đọc
    }
    // Nếu trung bình >= 0.5, trả về HIGH, ngược lại trả về LOW
    return (sum >= 5) ? HIGH : LOW;
}

int down_edge_detector_Sensor3() {
    if ((sensor3_previous_state == 1) && (sensor3_current_state == 0)) { return 1; }
    else { return 0; }
}

int down_edge_detector_Sensor4() {
    if ((sensor4_previous_state == 1) && (sensor4_current_state == 0)) { return 1; }
    else { return 0; }
}

int down_edge_detector_Sensor1() {
    if ((sensor1_previous_state == 1) && (sensor1_current_state == 0)) { return 1; }
    else { return 0; }
}

// Ham kiem tra gia tri hop le cua ky tu nhap tu HC05
// 's' de kiem tra trang thai isProtected
// 'p' de kich hoat isProtected = 1, tat den va thoat che do dieu khien
// 'q' de kich hoat isProtected = 0 va bat den
// 'u' de kich hoat isProteced = 0
// 'r' de reset he thong
bool isValidChar(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f' || 's' || 'p' || 'q' || 'u' || 'r');
}

// Ham lay gia tri hop le tu HC05
char getValidCharFromHC05() {
    char c;
    do {
        while (mySerial.available() == 0) {} // Cho den khi co du lieu
        c = mySerial.read();
    } while (!isValidChar(c));
    return c;
}

void setup() {

    delay(10000); // Cho 10 giay
    pinMode(Sensor1, INPUT);
    pinMode(Sensor3, INPUT);
    pinMode(Sensor4, INPUT);
    
    pinMode(Relay1, OUTPUT);
    pinMode(Relay2, OUTPUT);
    pinMode(Relay3, OUTPUT);
    pinMode(Relay4, OUTPUT);
    pinMode(Buzzer, OUTPUT);
    pinMode(Coi, OUTPUT);

    mySerial.begin(9600);
    Serial.begin(9600);

    digitalWrite(Relay1, LOW);
    digitalWrite(Relay2, LOW);
    digitalWrite(Relay3, LOW);
    digitalWrite(Relay4, LOW);
    digitalWrite(Buzzer, LOW);
    digitalWrite(Coi, LOW);
    statusRelay1 = "a"; // Trang thai a: Tat, 1: Bat
    statusRelay2 = "b"; // Trang thai b: Tat, 2: Bat
    statusRelay3 = "c"; // Trang thai c: Tat, 3: Bat

    data = "";
    i = 0;

    isProtected = false; // Mac dinh la true---------------------------------------------------------------------------------------------------
    isFirstRun = true;
    isFirstRun2 = true;
    temp = 0;
    previousMillisSet = true;
    previousMillis = 0;
    currentMillis = 0;
    char val = ' ';
    control_mode = false;

    // Phat am thanh qua buzzer de bao he thong khoi dong
    control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
    delay(10000); // Cho 10 giay

}

void loop() {

    /////////////////////////////////////////////////BEGIN CODE MOI//////////////////////////////////////////////////////////////////

    if ((isProtected == true) && ((digitalReadAdj(Sensor3) == HIGH) || digitalReadAdj(Sensor4) == HIGH)) { // Chot cua duoc mo
        delay(7000); // Cho 7 giay
        // Kiem tra lai trang thai cua Sensor3 va Sensor4
        if ((digitalReadAdj(Sensor3) == HIGH) || (digitalReadAdj(Sensor4) == HIGH)) {
            if (isFirstRun == true) {

                digitalWrite(Relay4, HIGH);
                delay(800); // Cho 800 ms
                digitalWrite(Relay4, LOW); // Bat may tinh

                digitalWrite(Coi, HIGH); // Kich hoat Coi
                digitalWrite(Buzzer, HIGH); // Kich hoat Buzzer, PHAN CODE TEST----------------------------------------------------------------
                delay(60000); // Cho 60 giay
                digitalWrite(Coi, LOW); // Tat coi
                digitalWrite(Buzzer, LOW); // Tat Buzzer, PHAN CODE TEST-----------------------------------------------------------------------
                isFirstRun = false;
            }
        }
    }

    /////////////////////////////////////////////END CODE MOI///////////////////////////////////////////////////////////////////////////

    // ////////////////////////////////////////////////BEGIN CODE CU///////////////////////////////////////////////////////////
    // if (isFirstRun2) {
    //     sensor3_previous_state = digitalReadAdj(Sensor3);
    //     sensor4_previous_state = digitalReadAdj(Sensor4);
    //     sensor1_previous_state = digitalReadAdj(Sensor1);
    //     isFirstRun2 = false;
    // }

    // sensor3_current_state = digitalReadAdj(Sensor3);
    // sensor4_current_state = digitalReadAdj(Sensor4);
    // sensor1_current_state = digitalReadAdj(Sensor1);

    // if (isProtected == 0) {
    //     if (down_edge_detector_Sensor4()) { temp++; }
    //     if (temp >= 3) {
    //         delay(1000);
    //         isProtected = 2;
    //         control_buzzer(BIP_1); // Kich hoat buzzer 1 bip
    //         temp = 0;
    //     }
    // }

    // if (isProtected == 2) {
    //     if (down_edge_detector_Sensor4()) { temp++; }
    //     if (temp >= 3) {
    //         delay(1000);
    //         isProtected = 0;
    //         control_buzzer(BIP_4); // Kich hoat buzzer bao tat chuc nang bao ve
    //         temp = 0;
    //     }
    // }

    // // Kiem tra trang thai bien isProtected, tac dong Sensor4 trong khoang thoi gian lon hon interval2 de kiem tra
    // if (digitalReadAdj(Sensor4) == LOW) {
    //     while (millis() >= (4294967295 - interval2)) {} // Cho den khi millis khong bi tran
    //     previousMillis = millis();
    //     while (digitalReadAdj(Sensor4) == LOW) {} // Cho den khi khong con tac dong
    //     currentMillis = millis();
    //     if (currentMillis - previousMillis >= interval2) {
    //         temp = 0;
    //         if (isProtected == 0) { control_buzzer(BIP_4); } // Kich hoat buzzer bao tat chuc nang bao ve
    //         else if (isProtected == 1) { control_buzzer(BIP_2); } // Kich hoat buzzer 2 bip
    //         else if (isProtected == 2) { control_buzzer(BIP_1); } // Kich hoat buzzer 1 bip
    //     }
    // }

    // // Phan code tu dong tat den
    // if ((isProtected == 0) && (digitalReadAdj(Sensor1) == LOW) && (digitalReadAdj(Sensor3) == HIGH)) {
    //     isProtected = 1;
    //     control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
    //     delay(5000); // Cho 5 giay
    //     digitalWrite(Relay1, LOW); // Tat den
    // }

    // // Phan code dieu khien coi, den va may tinh
    // if ((digitalReadAdj(Sensor1) == HIGH) && (isProtected == 1)) {

    //     if (previousMillisSet) {
    //         while (millis() >= (4294967295 - interval)) {} // Cho den khi millis khong bi tran
    //         previousMillis = millis();
    //         previousMillisSet = false;
    //     }

    //     if (down_edge_detector_Sensor4() == 1) { temp++; }

    //     if (temp >= 3) {
    //         delay(1000);
    //         isProtected = 0;
    //         control_buzzer(BIP_4); // Kich hoat buzzer bao tat chuc nang bao ve
    //         temp = 0;
    //         digitalWrite(Relay1, HIGH); // Bat den
    //         previousMillisSet = true;
    //         delay(20000); // Cho 20 giay
    //     }

    //     currentMillis = millis();
    //     if (currentMillis - previousMillis >= interval) {

    //         isProtected = 1;

    //         if (isFirstRun == true) {

    //             digitalWrite(Relay4, HIGH);
    //             delay(800); // Cho 800 ms
    //             digitalWrite(Relay4, LOW); // Bat may tinh
    //             delay(1000); // Cho 1 giay, PHAN CODE TEST-------------------------------------------------------------------------------------
    //             control_buzzer(BIP_6); // Kich hoat buzzer 6 bip, PHAN CODE TEST---------------------------------------------------------------

    //             digitalWrite(Coi, HIGH); // Kich hoat Coi
    //             digitalWrite(Buzzer, HIGH); // Kich hoat Buzzer, PHAN CODE TEST----------------------------------------------------------------
    //             delay(30000); // Cho 30 giay
    //             digitalWrite(Coi, LOW); // Tat coi
    //             digitalWrite(Buzzer, LOW); // Tat Buzzer, PHAN CODE TEST-----------------------------------------------------------------------
    //             delay(10000); // Cho 10 giay
    //             isFirstRun = false;
    //         }

    //         if (!isFirstRun) {
    //             digitalWrite(Coi, HIGH); // Kich hoat Coi
    //             digitalWrite(Buzzer, HIGH); // Kich hoat Buzzer, PHAN CODE TEST----------------------------------------------------------------
    //             delay(5000); // Cho 5 giay
    //             digitalWrite(Coi, LOW); // Tat coi
    //             digitalWrite(Buzzer, LOW); // Tat Buzzer, PHAN CODE TEST-----------------------------------------------------------------------
    //             delay(60000); // Cho 60 giay
    //         }
    //         previousMillisSet = true;
    //     }
    // }

    // if ((digitalReadAdj(Sensor1) == LOW) && down_edge_detector_Sensor3() == 1)
    // {
    //     control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
    // }

    // if ((isProtected == 0) && (digitalReadAdj(Sensor3) == HIGH)) { // Chot cua duoc mo
    //     control_buzzer(BIP_1); // Kich hoat buzzer 1 bip
    //     delay(30000); // Cho 30 giay
    // }

    // ///////////////////////////////////////////////END CODE CU///////////////////////////////////////////////////////////////

    ///////////////////////////////////DK RELAY BANG HC05//////////////////////////////////////////

    if( mySerial.available() > 0 ) {
        val = mySerial.read();
        delay(200);
        // Neu gia tri val khong phai ky tu hop le thi gan lai val = ' ', 
        // ki tu hop le la: '0', '1','2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 's', 'p', 'q', 'u', 'r'
        if ( isValidChar( val ) == false ) {
            val = ' ';
        }
    }

    // Xac nhan mat khau dieu khien
    if ( control_mode == false && val == '8' ) { // nhan 8 de bat dau nhap mat khau
        for ( i = 0; i < 2; i++ ) {
            val = getValidCharFromHC05();
            password_input[i] = val - '0'; // Chuyen ky tu sang so
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
            control_buzzer(BIP_2);
            val = ' ';
        }
        else {
            delay(300);
            control_buzzer(BIP_4); // Kich hoat buzzer 4 bip
        }
    }
    
    // Che do dieu khien
    if (control_mode == true)
    {
        // Relay 1 on
        if( val == '1' ) {
            digitalWrite(Relay1,HIGH); statusRelay1="1"; control_buzzer(BIP_2); }
        // Relay 2 on
        else if( val == '2' ) {
            digitalWrite(Relay2,HIGH); statusRelay2="2"; control_buzzer(BIP_2); }
        // Relay 3 on
        else if( val == '3' ) {
            digitalWrite(Relay3,HIGH); statusRelay3="3"; control_buzzer(BIP_2); }
        // Relay 1 off
        else if( val == 'a' ) {
            control_buzzer(BIP_1); // Kich hoat buzzer 1 bip
            delay(60000); // Cho 60 giay
            digitalWrite(Relay1,LOW); statusRelay1="a"; control_buzzer(BIP_1); }
        // Relay 2 off
        else if( val == 'b' ) {
            digitalWrite(Relay2,LOW); statusRelay2="b"; control_buzzer(BIP_1); }
        // Relay 3 off
        else if( val == 'c' ) {
            digitalWrite(Relay3,LOW); statusRelay3="c"; control_buzzer(BIP_1); }
        // Bat isProtected = true, tat den va thoat che do dieu khien
        else if( val == 'p' ) {
            control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
            delay(20000); // Cho 20 giay
            val = ' ';
            isProtected = true;
            control_mode = false;
            digitalWrite(Relay1, LOW); statusRelay1="a"; // Tat den
            control_buzzer(BIP_2); } // Kich hoat buzzer 2 bip
        // Tat isProtected = false va bat den
        else if ( val == 'q' ) {
            val = ' ';
            isProtected = false;
            digitalWrite(Relay1,HIGH); 
            statusRelay1="1"; 
            control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
            }
        // Tat isProtected = false
        else if( val == 'u' ) {
            val = ' ';
            isProtected = false; control_buzzer(BIP_4); }
        // Reset he thong
        else if( val == 'r' ) {
            val = ' ';
            control_buzzer(BIP_3); // Kich hoat buzzer 3 bip
            delay(2000); // Cho 2 giay
            resetArduino(); }   
    }

    // Thoat che do dieu khien
    if ( val == '9' && control_mode == true ) { // nhan 9 de thoat che do dieu khien
        control_mode = false;
        control_buzzer(BIP_4); // Kich hoat buzzer 4 bip
    }

    // Kiem tra isProtected khi nhan 's'
    if (val == 's') {
        val = ' ';
        if (isProtected == true) {
            control_buzzer(BIP_2); // Kich hoat buzzer 2 bip
        }
        else {
            control_buzzer(BIP_4); // Kich hoat buzzer 4 bip
        }
    }
    
    val=' ';

    ///////////////////////////////////END DK RELAY BANG HC05//////////////////////////////////////

    ///////////////////////////////////DK QUA CONG SERIAL MAY TINH///////////////////////////////////////

    if (Serial.available() > 0)
    {
        data=Serial.readStringUntil('\r');
        if (data == "batcoi")
        {
            control_buzzer(BIP_4); // Kich hoat buzzer 4 bip, PHAN CODE TEST---------------------------------------------------------------
            digitalWrite(Coi, HIGH);
            Serial.println("Dabatcoi");
            delay(25000); // Cho 25 giay
            digitalWrite(Coi, LOW);
            Serial.println("Datatcoi");
            delay(10000); // Cho 10 giay
        }
    }

    ///////////////////////////////END DK QUA CONG SERIAL MAY TINH///////////////////////////////////////

    // // Cap nhat trang thai bien
    // sensor3_previous_state = sensor3_current_state;
    // sensor4_previous_state = sensor4_current_state;

}