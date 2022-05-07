
class ServoControl {
  public:
#include <ESP32Servo.h>
    Servo lock;
    void Unlock() {
      lock.write(0);
    }
}
