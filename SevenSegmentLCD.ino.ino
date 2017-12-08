#include <Boards.h>
#include <Firmata.h>
#include <FirmataConstants.h>
#include <FirmataDefines.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>


bool clock_value = true;
byte count = 0;

//Something that could easily be made redundant
byte clock_mask()
{
  return (clock_value) ? 0xFF : 0x00;
}

//Bit packed union the represents the port pins starting with 0.
union lcd_port_values
{
  byte value;
  struct
  {
    byte com : 1;
    byte e : 1;
    byte d : 1;
    byte c : 1;
    byte g : 1;
    byte b : 1;
    byte a : 1;
    byte f : 1;
  };
};

//Done this way because to remap the port
// the only change needed is reording of the above union, nothing else.
byte ssd_value(byte number)
{
  if (0 <= number && number <= 9)
  {
    lcd_port_values value;
    value.value = 0;
    switch (number)
    {
      case 0:
        value.value = 0xFF;
        value.g = 0;
        break;
      case 1:
        value.b = 1;
        value.c = 1;
        break;
      case 2:
        value.value = 0xFF;
        value.f = 0;
        value.c = 0;
        break;
      case 3:
        value.value = 0xFF;
        value.e = 0;
        value.f = 0;
        break;
      case 4:
        value.c = 1;
        value.g = 1;
        value.b = 1;
        value.f = 1;
        break;
      case 5:
        value.value = 0xFF;
        value.b = 0;
        value.e = 0;
        break;
      case 6:
        value.value = 0xFF;
        value.b = 0;
        break;
      case 7:
        value.a = 1;
        value.b = 1;
        value.c = 1;
        break;
      case 8:
        value.value = 0xFF;
        break;
      case 9:
        value.value = 0xFF;
        value.e = 0;
        value.d = 0;
        break;
    };
    return value.value;
  }
  return 0;
}

//A single pulse of the current lcd value.
// this must be called regularly
void lcd_output(byte value)
{
  //Out of phase for all active values
  byte port_val = value ^ clock_mask();

  port_val &= 0b11111110;
  port_val |= clock_value;
  PORTD = port_val;
  
  clock_value = !clock_value;
}

//Sets up port D, thats it.
void setup() 
{
  DDRD = 0b11111111;    
  PORTD = 0b11111111;
}

//Pulses the output once for each number, the
// lcd time constant is ages...
void loop() 
{
  for (int i = 0; i < 100; ++i)
  {
    lcd_output(ssd_value(count));
    delay(10);
  }
  if (++count >= 10) count = 0;
}
