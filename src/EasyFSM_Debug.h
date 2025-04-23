#ifndef EASY_FSM_DEBUG_H
#define EASY_FSM_DEBUG_H

#define DEBUG_STATE(name, id) \
  [](){ Serial.print("-> Enter State "); Serial.print(id); Serial.print(": "); Serial.println(name); }, \
  [](){ Serial.print("<- Exit State "); Serial.print(id); Serial.print(": "); Serial.println(name); }


#endif
