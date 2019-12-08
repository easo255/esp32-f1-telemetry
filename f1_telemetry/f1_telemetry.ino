#include "WiFi.h" // ESP32 WiFi include
#include <WiFiUdp.h>
const char *SSID = "";
const char *WiFiPassword = "";
#include <TFT_eSPI.h>




struct UDPPacket
{
    float m_time;
    float m_lapTime;
    float m_lapDistance;
    float m_totalDistance;
    float m_x;  // World space position
    float m_y;  // World space position
    float m_z;  // World space position
    float m_speed;  // Speed of car in MPH
    float m_xv; // Velocity in world space
    float m_yv; // Velocity in world space
    float m_zv; // Velocity in world space
    float m_xr; // World space right direction
    float m_yr; // World space right direction
    float m_zr; // World space right direction
    float m_xd; // World space forward direction
    float m_yd; // World space forward direction
    float m_zd; // World space forward direction
    float m_susp_pos[4];  // Note: All wheel arrays have the order:
    float m_susp_vel[4];  // RL, RR, FL, FR
    float m_wheel_speed[4];
    float m_throttle;
    float m_steer;
    float m_brake;
    float m_clutch;
    float m_gear;
    float m_gforce_lat;
    float m_gforce_lon;
    float m_lap;
    float m_engineRate;
    float m_sli_pro_native_support; // SLI Pro support
    float m_car_position;   // car race position
    float m_kers_level; // kers energy left
    float m_kers_max_level; // kers maximum energy
    float m_drs;    // 0 = off, 1 = on
    float m_traction_control;   // 0 (off) - 2 (high)
    float m_anti_lock_brakes;   // 0 (off) - 1 (on)
    float m_fuel_in_tank;   // current fuel mass
    float m_fuel_capacity;  // fuel capacity
    float m_in_pits;    // 0 = none, 1 = pitting, 2 = in pit area
    float m_sector; // 0 = sector1, 1 = sector2, 2 = sector3
    float m_sector1_time;   // time of sector1 (or 0)
    float m_sector2_time;   // time of sector2 (or 0)
    float m_brakes_temp[4]; // brakes temperature (centigrade)
    float m_tyres_pressure[4];  // tyres pressure PSI
    float m_team_info;  // team ID
    float m_total_laps; // total number of laps in this race
    float m_track_size; // track size meters
    float m_last_lap_time;  // last lap time
    float m_max_rpm;    // cars max RPM, at which point the rev limiter will kick in
    float m_idle_rpm;   // cars idle RPM
    float m_max_gears;  // maximum number of gears
    float m_sessionType;    // 0 = unknown, 1 = practice, 2 = qualifying, 3 = race
    float m_drsAllowed; // 0 = not allowed, 1 = allowed, -1 = invalid / unknown
    float m_track_number;   // -1 for unknown, 0-21 for tracks
    float m_vehicleFIAFlags;    // -1 = invalid/unknown, 0 = none, 1 = green, 2 = blue, 3 = yellow, 4 = red
    float m_era;                        // era, 2017 (modern) or 1980 (classic)
    float m_engine_temperature;     // engine temperature (centigrade)
    float m_gforce_vert;    // vertical g-force component
    float m_ang_vel_x;  // angular velocity x-component
    float m_ang_vel_y;  // angular velocity y-component
    float m_ang_vel_z;  // angular velocity z-component
    byte  m_tyres_temperature[4];   // tyres temperature (centigrade)
    byte  m_tyres_wear[4];  // tyre wear percentage
    byte  m_tyre_compound;  // compound of tyre – 0 = ultra soft, 1 = super soft, 2 = soft, 3 = medium, 4 = hard, 5 = inter, 6 = wet
    byte  m_front_brake_bias;         // front brake bias (percentage)
    byte  m_fuel_mix;                 // fuel mix - 0 = lean, 1 = standard, 2 = rich, 3 = max
    byte  m_currentLapInvalid;      // current lap invalid - 0 = valid, 1 = invalid
    byte  m_tyres_damage[4];    // tyre damage (percentage)
    byte  m_front_left_wing_damage; // front left wing damage (percentage)
    byte  m_front_right_wing_damage;    // front right wing damage (percentage)
    byte  m_rear_wing_damage;   // rear wing damage (percentage)
    byte  m_engine_damage;  // engine damage (percentage)
    byte  m_gear_box_damage;    // gear box damage (percentage)
    byte  m_exhaust_damage; // exhaust damage (percentage)
    byte  m_pit_limiter_status; // pit limiter status – 0 = off, 1 = on
    byte  m_pit_speed_limit;    // pit speed limit in mph
    float m_session_time_left;  // NEW: time left in session in seconds
    byte  m_rev_lights_percent;  // NEW: rev lights indicator (percentage)
};

 
//The IP address that this arduino has requested to be assigned to.
IPAddress ip();
WiFiUDP Udp;
int packetSize = 512;

unsigned int localPort = 8888; 
 TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library


void setup() 
{
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(5);
  tft.setRotation(1);  // landscape
  
  Serial.begin(115200);
  ConnectToWiFi();
  Udp.begin(localPort);
  tft.begin();
}

void loop(){
  int packetSize = Udp.parsePacket();
  
  //If we received something.
  if(packetSize) {
       //Places 512 bytes of packet data into a char buffer.
       char packetBuffer[packetSize];
       Udp.read(packetBuffer, packetSize);
       //This is a precaution to minimise invalid packet readings, with a variable that should never be zero in gameplay.
       if(((UDPPacket*) &packetBuffer)->m_engineRate != 0) {
         //All of the following casts the received packet to a UDPPacket instance and fetches data out of them.
         int gear = (int) (((UDPPacket*) &packetBuffer)->m_gear);
         tft.drawString("Gear: " + String(gear-1), tft.width() / 2, tft.height() / 2);
       }
  }
}
 
void ConnectToWiFi()
{
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WiFiPassword);
  Serial.print("Connecting to "); Serial.println(SSID);
 
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
 
    if ((++i % 16) == 0)
    {
      Serial.println(F(" still trying to connect"));
    }
  }
 
  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
}
