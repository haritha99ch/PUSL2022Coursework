class DoorLock{
  #define Servorpin=14
  public:
    DoorLock(){
      door.attach(Servorpin);
    }
    bool Lock(){
      door.write(90);
      return true;
    }
    bool Unlock(){
      door.write(0);
      return false;
    }
 }
