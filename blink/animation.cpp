class Animation {
  public:
    int duration;
    int speedOrDelay;
    int effect;
    Animation(int duration, int speedOrDelay, int effect){
      this->duration = duration;
      this->speedOrDelay = speedOrDelay;
      this->effect = effect;
    };
};

