#ifndef MY_STL_HPP
#define MY_STL_HPP

namespace sjtu {
template <typename T, int MAX_SIZE> class queue {
private:
  T data[MAX_SIZE];
  int front_ = 0;
  int back_ = 0;

public:
  static int prev(int n) { return n - 1 < 0 ? n - 1 + MAX_SIZE : n - 1; }
  static int next(int n) {
    return n + 1 >= MAX_SIZE ? n + 1 - MAX_SIZE : n + 1;
  }
  static int plus(int lhs, int rhs) {
    return lhs + rhs >= MAX_SIZE ? lhs + rhs - MAX_SIZE : lhs + rhs;
  }

  T front() {
    if (empty()) {
      throw container_is_empty();
    }
    return data[front_];
  }
  T back() {
    if (empty()) {
      throw container_is_empty();
    }
    return data[prev(back_)];
  }
  T operator[](int place) {
    if (empty()) {
      throw container_is_empty();
    }
    return data[plus(front_, place)];
  }

  void push_back(const T &value) {
    data[back_] = value;
    back_ = next(back_);
  }
  void pop_back() {
    if (empty()) {
      throw container_is_empty();
    }
    back_ = prev(back_);
  }

  void push_front(const T &value) {
    front_ = prev(front_);
    data[front_] = value;
  }
  void pop_front() {
    if (empty()) {
      throw container_is_empty();
    }
    front_ = next(front_);
  }

  [[nodiscard]] bool empty() const { return back_ == front_; }
  size_t size() const {
    long long temp = back_ - front_;
    if (temp < 0) {
      temp += MAX_SIZE;
    }
    return temp;
  }
};
}; // namespace sjtu

#endif