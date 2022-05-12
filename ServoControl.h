
class ServoControl {
  public:
#include <ESP32Servo.h>
lock.attach(14);
    Servo lock;
    void Unlock() {
      
      lock.write(0);
    }
    void Lock(){
      lock.write(90);
    }
}
