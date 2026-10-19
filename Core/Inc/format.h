#ifndef FORMAT_H
#define FORMAT_H

#include <stdio.h>
#include <stdint.h>
void format_u16(char *buf, uint16_t value);
void format_u16_3(char *buf, uint16_t value);
void format_hms(char *buf, uint8_t hour_value, uint8_t min_value, uint8_t sec_value);
void format_ymd(char *buf, int year_value, int month_value, int day_value);
void format_fixed2(char *buf, const char *label, float value);

#endif
