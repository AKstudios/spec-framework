//==========================================================================================================
// SLACify - A C++ Wrapper around Qualcomm's SLAC implementation
// Developed by Akram S. Ali
//==========================================================================================================

#pragma once

class CSLACify
{

public:

    // Call this to initialize SLAC based on device type and SLAC settings
    int init(int device_type, signed slac_setting = 'n', int slac_limit = 40, int timeout = 20, int retries = 3);

    // Call this to attempt SLAC and creating a logical network
    int connect();

    // Call this to disconnect an established AVLN
    void close();

protected:
    int m_device_type;
    int m_setting;
};

enum device_type_t
{
    EVSE = 0,
    PEV  = 1
};

//==========================================================================================================