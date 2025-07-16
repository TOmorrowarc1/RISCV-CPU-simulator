#ifndef STORAGE_HPP
#define STORAGE_HPP

class Register {
private:
  int content_;

public:
  Register();
  int read();
  void write(int value);
};

class RegisterGroup {
private:
  Register registers_[32];

  RegisterGroup() {
    for (auto iter : registers_) {
      iter.write(0);
    }
  };

public:
  static RegisterGroup &getInstance();
  int read(int target);
  int write(int target, int value);
};

class StageRegister {
private:
  Register input_;
  Register output_;

public:
  int read();
  void write(int value);
  void refresh();
};

#endif