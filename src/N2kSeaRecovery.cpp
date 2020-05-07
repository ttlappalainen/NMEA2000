// 
// 
// 

#include "N2kTypes.h"
#include "N2kSeaRecovery.h"
#include <string.h>

#define SeaRecoveryProprietary 0x991D // Sea Recovery 285 + reserved + industry code=marine


void SetN2kSeaRecoveryPGN61184(tN2kMsg& N2kMsg,
    tN2kSeaRecoveryWatermakerCommand WatermakerCommand)
{
    N2kMsg.SetPGN(61184L);
    N2kMsg.Priority = 5;
    N2kMsg.Add2ByteUInt(SeaRecoveryProprietary);
    N2kMsg.AddByte(4);   // Message ID always 4
    N2kMsg.AddByte((WatermakerCommand & 0x0f) | (0xf0));
}

bool ParseN2kSeaRecoveryPGN61184(const tN2kMsg& N2kMsg,
    tN2kSeaRecoveryWatermakerCommand& WatermakerCommand)
{
    if (N2kMsg.PGN != 61184L) return false;
    int Index = 0;
    if (N2kMsg.Get2ByteUInt(Index) != SeaRecoveryProprietary) return false;
    unsigned char msgId = N2kMsg.GetByte(Index);
    unsigned char b = N2kMsg.GetByte(Index);
    WatermakerCommand = (tN2kSeaRecoveryWatermakerCommand)((b & 0x0f));
    return true;
}

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
    tN2kOkWarnError SystemStatus)
{
    N2kMsg.SetPGN(130816L);
    N2kMsg.Priority = 5;
    N2kMsg.Add2ByteUInt(SeaRecoveryProprietary);
    N2kMsg.AddByte(4);   // Message ID always 4
    N2kMsg.AddByte((WaterMakerInstance & 0x0f) | (WatermakerStatus << 4));
    N2kMsg.Add2ByteUDouble(PreFilterPressure, 100);
    N2kMsg.Add2ByteUDouble(PostFilterPressure, 100);
    N2kMsg.Add2ByteUDouble(SystemHighPressure, 1000);
    N2kMsg.Add2ByteUDouble(ProductWaterFlow, .1);
    N2kMsg.AddByte((PreFilterStatus & 0x02) |
        (PostFilterStatus & 0x02) << 2 |
        (OperationPressureStatus & 0x02) << 4 |
        (SalinityStatus & 0x02) << 6);
    N2kMsg.AddByte((FilterStatus & 0x02) << 0 |
        (SystemStatus & 0x02) << 2 |
        (0xf0));
}


bool ParseN2kSeaRecoveryPGN130816(const tN2kMsg& N2kMsg,
    unsigned char &WaterMakerInstance,
    tN2kWatermakerStatus &WatermakerStatus,
    double &PreFilterPressure,
    double &PostFilterPressure,
    double &SystemHighPressure,
    double &ProductWaterFlow,
    tN2kOkWarnError &PreFilterStatus,
    tN2kOkWarnError &PostFilterStatus,
    tN2kOkWarnError &OperationPressureStatus,
    tN2kOkWarnError &SalinityStatus,
    tN2kOkWarnError &FilterStatus,
    tN2kOkWarnError &SystemStatus)
{
    if (N2kMsg.PGN != 130816L) return false;
    int Index = 0;
    if (N2kMsg.Get2ByteUInt(Index) != SeaRecoveryProprietary) return false;
    unsigned char msgId = N2kMsg.GetByte(Index);
    unsigned char b = N2kMsg.GetByte(Index);
    WaterMakerInstance = b & 0x04;
    WatermakerStatus = (tN2kWatermakerStatus)((b >> 4) & 0x04);
    PreFilterPressure = N2kMsg.Get2ByteUDouble(100, Index);
    PostFilterPressure = N2kMsg.Get2ByteUDouble(100, Index);
    SystemHighPressure = N2kMsg.Get2ByteUDouble(1000, Index);
    ProductWaterFlow = N2kMsg.Get2ByteDouble(.1, Index);
    b = N2kMsg.GetByte(Index);
    PreFilterStatus = (tN2kOkWarnError)(b & 0x02);
    PostFilterStatus = (tN2kOkWarnError)((b >> 2) & 0x02);
    OperationPressureStatus = (tN2kOkWarnError)((b >> 4) & 0x02);
    SalinityStatus = (tN2kOkWarnError)((b >> 6) & 0x02);
    b = N2kMsg.GetByte(Index);
    FilterStatus = (tN2kOkWarnError)((b >> 0) & 0x02);
    SystemStatus = (tN2kOkWarnError)((b >> 2) & 0x02);
    return true;
}

