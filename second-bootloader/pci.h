#pragma once
#ifndef PCI_H
#define PCI_H

/*
	References:
		https://www.ctyme.com/intr/rb-2371.htm
*/

#include "typedefs.h"

#define PCI_INT_SIGN						0x20494350		//	'PCI '(LE)

#define PCI_HRDWR_CHAR_ACC_MECH_1			0x01
#define PCI_HRDWR_CHAR_ACC_MECH_2			0x02
#define PCI_HRDWR_CHAR_SPEC_CYCLE_MECH_1	0x08
#define PCI_HRDWR_CHAR_SPEC_CYCLE_MECH_2	0x10
#define PCI_HRDWR_CHAR_INFO_PRESENT			0x80

uint8_t pci_init(void);
uint8_t pci_get_hardware_characteristics(void);
uint32_t pci_get_pm_entry_point(void);

#endif