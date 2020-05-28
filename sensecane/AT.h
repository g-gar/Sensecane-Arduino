namespace AT
{

const char* ip = ""; // add
const char* port = ""; // add
const char* token = ""; // add

/********************************************************************************/
/********************************************************************************/
bool WaitOK( long, bool = true, bool = true );
int  WaitAnswer( long, bool = true, bool = true );
void sendComand( String, bool = true );

/********************************************************************************/
/********************************************************************************/
void sendComand(String c, bool echo) {
  Serial1.flush();
  if (echo)Serial.flush();
  Serial1.println(c);
  Serial1.flush();
  if (echo) Serial.println("*****************************************");
  if (echo) Serial.println(c);
  if (echo) Serial.flush();
}

/********************************************************************************/
/********************************************************************************/
void sendGet( String cmd ) {
    String cad = "AT+CIPSEND=" + String( cmd.length() + 4 );
    sendComand( cad );
    WaitOK( 500 );
    sendComand( cmd, true );
    sendComand( "" );
}

/********************************************************************************/
/********************************************************************************/
bool sendPost( String uri, String body ) {
    String linea1 = "POST " + uri + " HTTP/1.1";
    String linea2 = "Host: " + String(ip) + ": " + String(port);
    String linea3 = "Accept: *" + String("/*");
    String linea4 = "Content-Length: " + String(body.length());
    String linea5 = "Content-Type: application/json";
    String linea6 = "User-Agent: ATMega2560";
    String linea7 = "Authorization: Bearer " + String(token) ;
    int sizePost = linea1.length() + linea2.length() + linea3.length() + linea4.length() + linea5.length() + linea6.length() + linea7.length() + body.length() + 16;
    String sendCmd = "AT+CIPSEND=" + String(sizePost);

    sendComand(sendCmd);
    if (!WaitOK(2000)) return false;
    Serial1.println(linea1);
    Serial1.println(linea2);
    Serial1.println(linea3);
    Serial1.println(linea4);
    Serial1.println(linea5);
    Serial1.println(linea6);
    Serial1.println(linea7);
    Serial1.println("");
    Serial1.println(body);
    Serial1.println("");
    return true;
}

/********************************************************************************/
/********************************************************************************/
void PrintResponse( long timeoutamount ) {
    unsigned long timeout = millis() + timeoutamount;
    unsigned long timeStart = millis();
    char c;
    Serial.println( "=========================================" );
    while( millis() <= timeout ) {
        if( Serial1.available() > 0 ) {
            c = Serial1.read();
            Serial.write( c );
        }
    }
}

/********************************************************************************/
/********************************************************************************/
bool WaitOK( long timeoutamount, bool echo, bool echoError ) {
    static int codigo = 0;
    unsigned long timeout = millis() + timeoutamount;
    unsigned long timeStart = millis();
    char c0 = ' ', c1 = ' ';
    codigo += 1;
    if( echo )Serial.println( "-----------------------------------------" );
    while( millis() <= timeout ) {
        while( Serial1.available() > 0 ) {
            c1 = Serial1.read();
            if( echo )Serial.write( c1 );
            if( c0 == 'O' && c1 == 'K' ) {
                if( echo )Serial.println( "\nWaitOK TRUE [" + String( millis() - timeStart ) + "ms]" );
                delay( 100 );
                while( Serial1.available() > 0 ) Serial1.read();
                return true;
            }
            c0 = c1;
        }
    }
    if( echoError )Serial.println( "WaitOK False {Code:" + String( codigo ) + "} [" + String( millis() - timeStart ) + "ms]" );
    return false;
}

/********************************************************************************/
/********************************************************************************/
int WaitAnswer( long timeoutamount, bool echo, bool echoError ) {
    unsigned long timeout = millis() + timeoutamount;
    unsigned long timeStart = millis();
    char c[7] = "       ";
    if( echo )Serial.println( "-----------------------------------------" );
    while( millis() <= timeout ) {
        while( Serial1.available() > 0 ) {
            c[6] = Serial1.read();
            if( echo )Serial.write( c[6] );
            if( c[6] == '#' ) {
                if( c[0] == 'E' && c[1] == 'R' && c[2] == 'R' && c[3] == 'O' && c[4] == 'R' ) {
                    Serial.println( "\nAnswer ERROR:" + String( c[5] ) + " [" + String( millis() - timeStart ) + "ms]" );
                    delay( 100 );
                    while( Serial1.available() > 0 ) Serial1.read();
                    return ( c[5] - '0' );
                }
                if( c[4] == 'O' && c[5] == 'K' ) {
                    Serial1.read();
                    char code = Serial1.read();
                    if( echo )Serial.println( "\nAnswer OK [" + String( millis() - timeStart ) + "ms]" );
                    delay( 100 );
                    while( Serial1.available() > 0 ) Serial1.read();
                    return 0;
                }
            }
            c[0] = c[1]; c[1] = c[2]; c[2] = c[3]; c[3] = c[4]; c[4] = c[5]; c[5] = c[6];
        }
    }
    if( echoError )Serial.println( "WaitAnswer False [" + String( millis() - timeStart ) + "ms]" );
    return -1;
}

}
