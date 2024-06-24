#include "sys.hpp";


bool checkCpuVendor(const char* vendor) {
    std::array<int, 4> cpui;
    __cpuid(cpui.data(), 0);
    char vendor_id[13];
    memcpy(vendor_id, &cpui[1], 4);  // EBX
    memcpy(vendor_id + 4, &cpui[3], 4);  // EDX
    memcpy(vendor_id + 8, &cpui[2], 4);  // ECX
    vendor_id[12] = '\0';
    return strcmp(vendor_id, vendor) == 0;
}

bool checkVirtualEnvironment() {
    // Check common CPU vendors used by virtual environments
    if (checkCpuVendor("KVMKVMKVM") || checkCpuVendor("Microsoft Hv") || checkCpuVendor("VMwareVMware")) {
        return true;
    }

    // Check for hypervisor bit
    std::array<int, 4> cpui;
    __cpuid(cpui.data(), 1);
    bool isHypervisor = cpui[2] & (1 << 31);
    if (isHypervisor) {
        return true;
    }

    return false;
}
