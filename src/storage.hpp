#ifndef STORAGE_HPP
#define STORAGE_HPP

class Register {
private:
  int Content;

public:
  Register();
  int read();
  void write(int value);
};

class RegisterGroup {
private:
  Register Registers[32];

  RegisterGroup() {
    for (auto iter : Registers) {
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
  Register input;
  Register output;

public:
  int read();
  void write(int value);
  void refresh();
};

#endif