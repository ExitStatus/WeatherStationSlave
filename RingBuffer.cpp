#include "RingBuffer.h"

RingBuffer::RingBuffer(int size) : _size(size), _position(0)
{
  _values = new float[_size];
  for (int i=0; i < _size; i++)
    _values[i] = 500;
}

RingBuffer::~RingBuffer()
{
  delete[] _values;
}

void RingBuffer::Add(float value)
{
  _values[_position] = value;
  if (++_position >= _size)
    _position = 0;
}

float RingBuffer::Average()
{
  float total = 0.0;
  int items = 0;
  
  for (int i=0; i<_size; i++)
  {
    if (_values[i] != 500)
    {
      total += _values[i];
      items++;
    }
  } 

  return total / items;
}
