#ifndef __RXSDT_H_INCLUDED__
#define __RXSDT_H_INCLUDED__

#include <acpi/sdt.h>
#include <utils.h>

void rxsdt_init(void);
struct sdt_header *rxsdt_query_table(const char *name);

#endif