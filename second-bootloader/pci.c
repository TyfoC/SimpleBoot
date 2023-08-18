#include "pci.h"

static uint8_t characteristics_ = 0;
static uint32_t pm_entry_point_ = 0;

extern uint8_t __pci_get_info(uint8_t* info, uint32_t* entryPoint);

uint8_t pci_init(void) {
	if (!__pci_get_info(&characteristics_, &pm_entry_point_)) return false;

	characteristics_ |= PCI_HRDWR_CHAR_INFO_PRESENT;
	return true;
}

uint8_t pci_get_hardware_characteristics(void) {
	return characteristics_;
}

uint32_t pci_get_pm_entry_point(void) {
	return pm_entry_point_;
}