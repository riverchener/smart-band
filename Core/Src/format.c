#include "format.h"


void format_u16(char *buf, uint16_t value)
{
  snprintf(buf, 10, "%u", value);
}

 void format_u16_3(char *buf, uint16_t value)
{
  snprintf(buf, 10, "%03u", value);
}

 void format_hms(char *buf, uint8_t hour_value, uint8_t min_value, uint8_t sec_value)
{
  snprintf(buf, 50, "%02u:%02u:%02u", hour_value, min_value, sec_value);
}

 void format_ymd(char *buf, int year_value, int month_value, int day_value)
{
  snprintf(buf, 50, "%04d/%02d/%02d", year_value, month_value, day_value);
}

 void format_fixed2(char *buf, const char *label, float value)
{
  int32_t scaled = (int32_t)(value * 100.0f);
  uint32_t magnitude;
  char sign = 0;

  if (scaled < 0)
  {
    sign = '-';
    magnitude = (uint32_t)(-scaled);
  }
  else
  {
    magnitude = (uint32_t)scaled;
  }

  if (label != NULL)
  {
    if (sign != 0)
    {
      snprintf(buf, 50, "%s-%lu.%02lu", label, magnitude / 100U, magnitude % 100U);
    }
    else
    {
      snprintf(buf, 50, "%s%lu.%02lu", label, magnitude / 100U, magnitude % 100U);
    }
  }
  else if (sign != 0)
  {
    snprintf(buf, 10, "-%lu.%02lu", magnitude / 100U, magnitude % 100U);
  }
  else
  {
    snprintf(buf, 10, "%lu.%02lu", magnitude / 100U, magnitude % 100U);
  }
}