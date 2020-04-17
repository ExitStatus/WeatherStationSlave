#ifndef RingBuffer_h
#define RingBuffer_h

class RingBuffer {
private:
  int _size;
  int _position;
  float* _values;

public:
  RingBuffer(int size);
  ~RingBuffer();
  void Add(float value);
  float Average();
};

#endif 
