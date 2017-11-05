#include <NewPing.h>
#include <Servo.h>

#define TOO_CLOSE 35
#define LEFT_MOTOR_INIT_1 6
#define LEFT_MOTOR_INIT_2 7
#define ENABLE_PIN_1 3

#define RIGHT_MOTOR_INIT_1 3
#define RIGHT_MOTOR_INIT_2 4
#define ENABLE_PIN_2 7
#define RUN_TIME 30

#define TRIGGER_PIN   12 
#define ECHO_PIN      11
#define MAX_DISTANCE 200 
#define SERVO_PIN 9
#define MAX_READ 5
#define INTERVAL 15
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int distance;
int average = 0;         
unsigned int pingSpeed = 50;
unsigned long pingTimer; 
   
Servo servo; 
int pos=0; 
int increment;
unsigned long lastUpdate;            

void Attach(int pin)
{
  servo.attach(pin);
}

void echoCheck()
{
  if (sonar.check_timer()) 
    distance=(sonar.ping_result / US_ROUNDTRIP_CM);
}        
  
void sweep()
{
   if((millis() - lastUpdate) > INTERVAL) 
   {
      lastUpdate = millis();
      pos += increment;
      servo.write(pos);
      if (millis() >= pingTimer)
      {  
        pingTimer += pingSpeed; 
        sonar.ping_timer(echoCheck);
      }
      if ((pos >= 180) || (pos <= 0))
        increment=-increment;
   }
}
     
class Robot
{
  private:
  enum state_t { stateStopped, stateMoving, stateTurning, stateRemote };
  state_t state; 
  
  unsigned long startTime;
  unsigned long endStateTime;
  unsigned long endTime;
  
  protected:
  void  setMotor(boolean reverse, int flag)
  {
    if(flag==1)
    {
      digitalWrite(LEFT_MOTOR_INIT_1,reverse);  
      digitalWrite(LEFT_MOTOR_INIT_2,!reverse); 
    
      digitalWrite(RIGHT_MOTOR_INIT_1,reverse);  
      digitalWrite(RIGHT_MOTOR_INIT_2,!reverse);          
    }
    else if(flag==2)
    {
  
      digitalWrite(LEFT_MOTOR_INIT_1,!reverse);  
      digitalWrite(LEFT_MOTOR_INIT_2,reverse); 
    
      digitalWrite(RIGHT_MOTOR_INIT_1,!reverse);  
      digitalWrite(RIGHT_MOTOR_INIT_2,reverse);         
    }
    else if(flag==3)
    { 
      digitalWrite(LEFT_MOTOR_INIT_1,reverse);  
      digitalWrite(LEFT_MOTOR_INIT_2,reverse); 
    
      digitalWrite(RIGHT_MOTOR_INIT_1,reverse);  
      digitalWrite(RIGHT_MOTOR_INIT_2,reverse);         
    }
    else if(flag==4)
    { 
      digitalWrite(LEFT_MOTOR_INIT_1,reverse);  
      digitalWrite(LEFT_MOTOR_INIT_2,!reverse); 
    
      digitalWrite(RIGHT_MOTOR_INIT_1,!reverse);  
      digitalWrite(RIGHT_MOTOR_INIT_2,reverse);         
    }
  }

  void remote()
  {
      setMotor(true,0);
      state = stateRemote;
  }
  
  void move_obj()
  {
      setMotor(true,4);            
      state = stateMoving;
  }
  
  void stop_obj()
  {
      setMotor(true,3);          
      state = stateStopped;
  }

  bool doneRunning(unsigned long currentTime)
  {
      return (currentTime >= endTime);
  }
  
  bool obstacleAhead(unsigned int distance)
  {
      return (distance <= TOO_CLOSE);
  }
  
  int turn(unsigned long currentTime)
  {
      if(pos<90)
        setMotor(true,2);
      else if(pos>90)
        setMotor(true,1);
      else if (random(2) == 0) 
          setMotor(true,1);
      else 
          setMotor(true,2);
      state = stateTurning;
      endStateTime = currentTime + random(500, 1000);
      return 0;
  }
  
  bool doneTurning(unsigned long currentTime, unsigned int distance)
  {
     if (currentTime >= endStateTime)
          return (distance > TOO_CLOSE);
     return false;
  }
  
  bool moving() 
  { 
    return (state == stateMoving); 
  }
  bool turning() 
  { 
    return (state == stateTurning); 
  }
  bool stopped() 
  { 
    return (state == stateStopped); 
  }
  bool remoteControlled() 
  { 
    return (state == stateRemote); 
  }
  
  public:
  void initialize()
  {
     endTime = millis() + RUN_TIME * 2000;
     move_obj();
  }

  Robot()
  {
    initialize();
    increment=3;
  }

  void run()
  {
    unsigned long currentTime = millis();
    Attach(SERVO_PIN);
    sweep();
    Serial.println(distance);   
    if (doneRunning(currentTime))
      stop_obj();
    else if (moving()) 
    {
      if (obstacleAhead(distance))
        turn(currentTime);
    }
    else if (turning()) 
    {
      if (doneTurning(currentTime, distance))
        move_obj();
    }
  }

}robot;



    
void setup() 
{
  Serial.begin(9600);
  pinMode(LEFT_MOTOR_INIT_1,OUTPUT);
  pinMode(LEFT_MOTOR_INIT_2,OUTPUT);

  pinMode(RIGHT_MOTOR_INIT_1,OUTPUT);
  pinMode(RIGHT_MOTOR_INIT_2,OUTPUT);
  robot.initialize(); 
  pingTimer = millis();
}

void loop() 
{
    robot.run();
}


