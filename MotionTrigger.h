class MotionTrigger{
  #define pirSensor=8;
  public:
    MotionTrigger(){
      pinMode(pirSensor, INPUT);
    }
    bool isTriggered(){
      if(pirSensor){
        return true;
      }else{
        return false;
      }
    }
}
