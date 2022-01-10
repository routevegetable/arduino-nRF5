#include "verify.h"

#include "Uart.h"

void VERIFY_MESS_impl(int32_t _status, const char* (*_fstr)(int32_t), const char* func_name, int line_number)
{
    Serial.print(func_name);
    Serial.print(": ");
    Serial.print(line_number);
    Serial.print(": verify failed, error = ");

    if (_fstr && _fstr(_status))
    {
      Serial.print(_fstr(_status));
    }
    else
    {
      Serial.print(_status, HEX);
      Serial.print(" (");
      Serial.print(_status);
      Serial.print(")");
    }
    Serial.println("");
}
