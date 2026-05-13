#define digitalWriteFast(pin, val) \
  if (val) { \
    digitalPinToPort(pin)->PIO_SODR = digitalPinToBitMask(pin); \
  } else { \
    digitalPinToPort(pin)->PIO_CODR = digitalPinToBitMask(pin); \
  }
