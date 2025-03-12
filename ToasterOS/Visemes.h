
int VISEME_NEUTRAL = 0;
int VISEME_AA = 1;
int VISEME_EE = 2;
int VISEME_OH = 3;
int VISEME_OU = 4;

byte Visemes[5][4][8] = {
  // Neutral
  {
    { B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 },  //
    { B00000000, B00000000, B00001110, B00000111, B00000001, B00000000, B00000000, B00000000 },  //
    { B00000000, B00000000, B00011000, B10111110, B11100111, B01000000, B00000000, B00000000 },  //
    { B00000000, B00000011, B00001111, B00111100, B11110000, B11000000, B00000000, B00000000 }   //
  },
  // AA
  {
    { B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 },  //
    { B00000000, B00001000, B00001111, B00000111, B00000011, B00000001, B00000000, B00000000 },  //
    { B00000000, B00000000, B00111000, B11111111, B11111111, B11111111, B01111011, B00000000 },  //
    { B00000001, B00001111, B01111111, B11111110, B11110000, B11100000, B11000000, B00000000 }   //
  },
  // EE
  {
    { B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 },  //
    { B00000000, B00000000, B00001100, B00001111, B00000111, B00000001, B00000000, B00000000 },  //
    { B00000000, B00000000, B00111000, B11111111, B11111111, B11111111, B01110001, B00000000 },  //
    { B00000000, B00000000, B00001000, B11111100, B11111110, B11111100, B11000000, B00000000 }   //
  },
  // OH
  {
    { B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 },  //
    { B00000000, B00000000, B00001100, B00001111, B00000111, B00000011, B00000001, B00000000 },  //
    { B00000000, B00000000, B11000100, B11111110, B11111100, B11111000, B10000000, B00000000 },  //
    { B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 }   //
  },
  // OU
  {
    { B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 },  //
    { B00000000, B00000000, B00011000, B00001111, B00000111, B00000011, B00000001, B00000000 },  //
    { B00000000, B00000000, B10000000, B11111100, B11110000, B11100000, B00000000, B00000000 },  //
    { B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 }   //
  }
};


struct AnimationFrame {
  int Viseme_Index;
  unsigned int Duration;
};


struct AnimationFrame TestAnimation[] = {
  { .Viseme_Index = VISEME_AA, .Duration = 580 },
  { .Viseme_Index = VISEME_EE, .Duration = 640 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1583 },
  { .Viseme_Index = VISEME_AA, .Duration = 469 },
  { .Viseme_Index = VISEME_EE, .Duration = 252 },
  { .Viseme_Index = VISEME_OU, .Duration = 299 },
  { .Viseme_Index = VISEME_AA, .Duration = 309 },
  { .Viseme_Index = VISEME_EE, .Duration = 924 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1180 },
  { .Viseme_Index = VISEME_AA, .Duration = 664 },
  { .Viseme_Index = VISEME_EE, .Duration = 328 },
  { .Viseme_Index = VISEME_OU, .Duration = 319 },
  { .Viseme_Index = VISEME_AA, .Duration = 296 },
  { .Viseme_Index = VISEME_EE, .Duration = 328 },
  { .Viseme_Index = VISEME_OU, .Duration = 268 },
  { .Viseme_Index = VISEME_AA, .Duration = 311 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 997 },
  { .Viseme_Index = VISEME_AA, .Duration = 848 },
  { .Viseme_Index = VISEME_EE, .Duration = 376 },
  { .Viseme_Index = VISEME_OU, .Duration = 327 },
  { .Viseme_Index = VISEME_AA, .Duration = 301 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1580 },
  { .Viseme_Index = VISEME_AA, .Duration = 391 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 789 },
  { .Viseme_Index = VISEME_AA, .Duration = 448 },
  { .Viseme_Index = VISEME_EE, .Duration = 288 },
  { .Viseme_Index = VISEME_OU, .Duration = 288 },
  { .Viseme_Index = VISEME_AA, .Duration = 888 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1016 },
  { .Viseme_Index = VISEME_AA, .Duration = 844 },
  { .Viseme_Index = VISEME_EE, .Duration = 364 },
  { .Viseme_Index = VISEME_OU, .Duration = 299 },
  { .Viseme_Index = VISEME_AA, .Duration = 325 },
  { .Viseme_Index = VISEME_EE, .Duration = 912 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 980 },
  { .Viseme_Index = VISEME_AA, .Duration = 460 },
  { .Viseme_Index = VISEME_EE, .Duration = 288 },
  { .Viseme_Index = VISEME_OU, .Duration = 252 },
  { .Viseme_Index = VISEME_AA, .Duration = 292 },
  { .Viseme_Index = VISEME_EE, .Duration = 252 },
  { .Viseme_Index = VISEME_OU, .Duration = 288 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1095 },
  { .Viseme_Index = VISEME_AA, .Duration = 2016 },
  { .Viseme_Index = VISEME_EE, .Duration = 340 },
  { .Viseme_Index = VISEME_OU, .Duration = 332 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1848 },
  { .Viseme_Index = VISEME_AA, .Duration = 1768 },
  { .Viseme_Index = VISEME_EE, .Duration = 964 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1340 },
  { .Viseme_Index = VISEME_AA, .Duration = 544 },
  { .Viseme_Index = VISEME_EE, .Duration = 443 },
  { .Viseme_Index = VISEME_OU, .Duration = 309 },
  { .Viseme_Index = VISEME_AA, .Duration = 313 },
  { .Viseme_Index = VISEME_EE, .Duration = 823 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1548 },
  { .Viseme_Index = VISEME_AA, .Duration = 356 },
  { .Viseme_Index = VISEME_EE, .Duration = 347 },
  { .Viseme_Index = VISEME_OU, .Duration = 321 },
  { .Viseme_Index = VISEME_AA, .Duration = 336 },
  { .Viseme_Index = VISEME_EE, .Duration = 301 },
  { .Viseme_Index = VISEME_OU, .Duration = 295 },
  { .Viseme_Index = VISEME_AA, .Duration = 308 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1004 },
  { .Viseme_Index = VISEME_AA, .Duration = 911 },
  { .Viseme_Index = VISEME_EE, .Duration = 337 },
  { .Viseme_Index = VISEME_OU, .Duration = 305 },
  { .Viseme_Index = VISEME_AA, .Duration = 295 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1020 },
  { .Viseme_Index = VISEME_AA, .Duration = 2119 },
  { .Viseme_Index = VISEME_EE, .Duration = 279 },
  { .Viseme_Index = VISEME_OU, .Duration = 333 },
  { .Viseme_Index = VISEME_AA, .Duration = 832 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1184 },
  { .Viseme_Index = VISEME_AA, .Duration = 716 },
  { .Viseme_Index = VISEME_EE, .Duration = 348 },
  { .Viseme_Index = VISEME_OU, .Duration = 332 },
  { .Viseme_Index = VISEME_AA, .Duration = 312 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 907 },
  { .Viseme_Index = VISEME_AA, .Duration = 1396 },
  { .Viseme_Index = VISEME_EE, .Duration = 240 },
  { .Viseme_Index = VISEME_OU, .Duration = 296 },
  { .Viseme_Index = VISEME_AA, .Duration = 256 },
  { .Viseme_Index = VISEME_EE, .Duration = 273 },
  { .Viseme_Index = VISEME_OU, .Duration = 351 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1788 },
  { .Viseme_Index = VISEME_AA, .Duration = 1280 },
  { .Viseme_Index = VISEME_EE, .Duration = 356 },
  { .Viseme_Index = VISEME_OU, .Duration = 400 },
  { .Viseme_Index = VISEME_NEUTRAL, .Duration = 1156 },
};
