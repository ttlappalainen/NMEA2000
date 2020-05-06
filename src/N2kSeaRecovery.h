// N2kSeaRecovery.h

#ifndef _N2KSEARECOVERY_h
#define _N2KSEARECOVERY_h

#include "N2kMsg.h"
#include <N2kMessages.h>
#include <stdint.h>

enum tN2kSeaRecoveryWatermakerCommand {
    N2kSRWC_Stop=0,
    N2kSRWC_Start=1,
    N2kSRWC_Flush=2,
    N2kSRWC_EmergencyStop=14
};

//*****************************************************************************
// Watermaker Control [as used by Sea Recovery]
// This PGN is used to control a watermaker. 
// Watermaker controls.
// Input:
//  - Command               Stop, Start, Flush, Emergency Stop
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kSeaRecoveryPGN61184(tN2kMsg& N2kMsg,
    tN2kSeaRecoveryWatermakerCommand WatermakerCommand);

inline void SetN2kSeaRecoveryWatermakerCommand(tN2kMsg& N2kMsg,
    tN2kSeaRecoveryWatermakerCommand WatermakerCommand)
{
    SetN2kSeaRecoveryPGN61184(N2kMsg,
        WatermakerCommand);
}

bool ParseN2kSeaRecoveryPGN61184(const tN2kMsg& N2kMsg,
    tN2kSeaRecoveryWatermakerCommand& WatermakerCommand);

inline bool ParseN2kSeaRecoveryWatermakerCommand(const tN2kMsg& N2kMsg,
    tN2kSeaRecoveryWatermakerCommand& WatermakerCommand)
{
    return ParseN2kSeaRecoveryPGN61184(N2kMsg,
        WatermakerCommand);
}

//*****************************************************************************
// Watermaker Input Setting and Status [as used by Sea Recovery]
// This PGN may be requested or used to command and configure a number of 
// Watermaker controls.
// Input:
//  - WaterMakerInstance            This should be unique at least on one device. May be best to have it unique over all devices sending this PGN.
//  - WatermakerStatus              The watermaker operating status.
//  - PreFilterPressure             Pre-Filter pressure 0-6,553,200 Pa
//  - PostFilterPressure            Post-Filter pressure 0-6,553,200 Pa
//  - SystemHighPressure            System High Pressure 0-65,532,000 Pa
//  - ProductWaterFlow              Product water flow liters per hour
//  - PreFilterStatus               Ok, Warn, Error
//  - PostFilterStatus              Ok, Warn, Error
//  - OperationPressureStatus       Ok, Warn, Error
//  - SalinityStatus                Ok, Warn, Error
//  - FilterStatus                  Ok, Warn, Error
//  - SystemStatus                  Ok, Warn, Error
//  - Reserved
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kSeaRecoveryPGN130816(tN2kMsg& N2kMsg,
    unsigned char WaterMakerInstance,
    tN2kWatermakerStatus WatermakerStatus,
    double PreFilterPressure,
    double PostFilterPressure,
    double SystemHighPressure,
    double ProductWaterFlow,
    tN2kOkWarnError PreFilterStatus,
    tN2kOkWarnError PostFilterStatus,
    tN2kOkWarnError OperationPressureStatus,
    tN2kOkWarnError SalinityStatus,
    tN2kOkWarnError FilterStatus,
    tN2kOkWarnError SystemStatus);

inline void SetN2kSeaRecoveryWatermakerStatus(
    tN2kMsg& N2kMsg,
    unsigned char WaterMakerInstance,
    tN2kWatermakerStatus WatermakerStatus,
    double PreFilterPressure,
    double PostFilterPressure,
    double SystemHighPressure,
    double ProductWaterFlow,
    tN2kOkWarnError PreFilterStatus,
    tN2kOkWarnError PostFilterStatus,
    tN2kOkWarnError OperationPressureStatus,
    tN2kOkWarnError SalinityStatus,
    tN2kOkWarnError FilterStatus,
    tN2kOkWarnError SystemStatus)
{
    SetN2kSeaRecoveryPGN130816(N2kMsg,
        WaterMakerInstance,
        WatermakerStatus,
        PreFilterPressure,
        PostFilterPressure,
        SystemHighPressure,
        ProductWaterFlow,
        PreFilterStatus,
        PostFilterStatus,
        OperationPressureStatus,
        SalinityStatus,
        FilterStatus,
        SystemStatus);
}

bool ParseN2kSeaRecoveryPGN130816(const tN2kMsg& N2kMsg,
    unsigned char& WaterMakerInstance,
    tN2kWatermakerStatus& WatermakerStatus,
    double& PreFilterPressure,
    double& PostFilterPressure,
    double& SystemHighPressure,
    double& ProductWaterFlow,
    tN2kOkWarnError& PreFilterStatus,
    tN2kOkWarnError& PostFilterStatus,
    tN2kOkWarnError& OperationPressureStatus,
    tN2kOkWarnError& SalinityStatus,
    tN2kOkWarnError& FilterStatus,
    tN2kOkWarnError& SystemStatus);

inline bool ParseN2kSeaRecoveryWatermakerStatus(
    const tN2kMsg& N2kMsg,
    unsigned char& WaterMakerInstance,
    tN2kWatermakerStatus& WatermakerStatus,
    double& PreFilterPressure,
    double& PostFilterPressure,
    double& SystemHighPressure,
    double& ProductWaterFlow,
    tN2kOkWarnError& PreFilterStatus,
    tN2kOkWarnError& PostFilterStatus,
    tN2kOkWarnError& OperationPressureStatus,
    tN2kOkWarnError& SalinityStatus,
    tN2kOkWarnError& FilterStatus,
    tN2kOkWarnError& SystemStatus) 
{

    bool ParseN2kSeaRecoveryPGN130816(
        tN2kMsg & N2kMsg,
        unsigned char WaterMakerInstance,
        tN2kWatermakerStatus  WatermakerStatus,
        double PreFilterPressure,
        double PostFilterPressure,
        double SystemHighPressure,
        double ProductWaterFlow,
        tN2kOkWarnError  PreFilterStatus,
        tN2kOkWarnError  PostFilterStatus,
        tN2kOkWarnError  OperationPressureStatus,
        tN2kOkWarnError  SalinityStatus,
        tN2kOkWarnError  FilterStatus,
        tN2kOkWarnError  SystemStatus);
}


#endif

