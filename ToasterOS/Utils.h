/// Will be negative for future times
long timeSince(unsigned long previousTime) {
  return millis() - previousTime;
}

byte Reverse(byte b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}


// Example
// PrintF("Rendering Row %d\n", row);
// https://stackoverflow.com/a/77608699/13497411
int PrintF(const char* format, ...) {
  static char buffer[1024];  // Adjust to your needs
  va_list aptr;
  va_start(aptr, format);
  int results = vsnprintf(buffer, sizeof(buffer), format, aptr);
  va_end(aptr);
  Serial.print(buffer);
  return results;
}
