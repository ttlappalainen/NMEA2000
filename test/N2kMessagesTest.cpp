#include <string.h>
#include <catch.hpp>
#include <N2kMessages.h>

// This is a test file for checking N2k message syntax.
// Each test case deals with a single PGN type.
// Test case sets arbitrary parameter values and checks if the same values are parsed back. Not all messages are tested.

TEST_CASE("PGN129039 AIS Class B Position")
{

  tN2kMsg N2kMsg;
  uint8_t MessageID[2] = {5,0};
  tN2kAISRepeat Repeat[2] = {N2kaisr_Final,N2kaisr_Final};
  uint32_t UserID[2] = {7,0};
  double Latitude[2] = {-33.0,0};
  double Longitude[2] = {151.0,0};
  bool Accuracy[2] = {true,true};
  bool RAIM[2] = {false,false};
  uint8_t Seconds[2] = {4,0};
  double COG[2] = {0.1,0};
  double SOG[2] = {2.,0};
  tN2kAISTransceiverInformation AISTransceiverInformation[2] = {N2kaischannel_B_VDL_transmission,N2kaischannel_B_VDL_transmission};
  double Heading[2] = {0.15,0};
  tN2kAISUnit Unit[2] = {N2kaisunit_ClassB_CS,N2kaisunit_ClassB_CS};
  bool Display[2] = {true,true};
  bool DSC[2] = {false,false};
  bool Band[2] = {false,false};
  bool Msg22[2] = {true,true};
  tN2kAISMode Mode[2] = {N2kaismode_Assigned,N2kaismode_Assigned};
  bool State[2] = {true,true};

  SetN2kAISClassBPosition(N2kMsg,
        MessageID[0],
        Repeat[0],
        UserID[0],
        Latitude[0],
        Longitude[0],
        Accuracy[0],
        RAIM[0],
        Seconds[0],
        COG[0],
        SOG[0],
        AISTransceiverInformation[0],
        Heading[0],
        Unit[0],
        Display[0],
        DSC[0],
        Band[0],
        Msg22[0],
        Mode[0],
        State[0]);

  ParseN2kAISClassBPosition(N2kMsg,
        MessageID[1],
        Repeat[1],
        UserID[1],
        Latitude[1],
        Longitude[1],
        Accuracy[1],
        RAIM[1],
        Seconds[1],
        COG[1],
        SOG[1],
        AISTransceiverInformation[1],
        Heading[1],
        Unit[1],
        Display[1],
        DSC[1],
        Band[1],
        Msg22[1],
        Mode[1],
        State[1]);

  SECTION("parsed values match set values")
  {
    REQUIRE(MessageID[0] ==                  MessageID[1]);
    REQUIRE(Repeat[0] ==                     Repeat[1]);
    REQUIRE(UserID[0] ==                     UserID[1]);
    REQUIRE(Latitude[0] ==                   Latitude[1]);
    REQUIRE(Longitude[0] ==                  Longitude[1]);
    REQUIRE(Accuracy[0] ==                   Accuracy[1]);
    REQUIRE(RAIM[0] ==                       RAIM[1]);
    REQUIRE(Seconds[0] ==                    Seconds[1]);
    REQUIRE(COG[0] ==                        COG[1]);
    REQUIRE(SOG[0] ==                        SOG[1]);
    REQUIRE(AISTransceiverInformation[0] ==  AISTransceiverInformation[1]);
    REQUIRE(Heading[0] ==                    Heading[1]);
    REQUIRE(Unit[0] ==                       Unit[1]);
    REQUIRE(Display[0] ==                    Display[1]);
    REQUIRE(DSC[0] ==                        DSC[1]);
    REQUIRE(Band[0] ==                       Band[1]);
    REQUIRE(Msg22[0] ==                      Msg22[1]);
    REQUIRE(Mode[0] ==                       Mode[1]);
    REQUIRE(State[0] ==                      State[1]);
  }

  // use previous version calls to check for backwards compatibility
  SetN2kAISClassBPosition(N2kMsg,
          MessageID[0],
          Repeat[0],
          UserID[0],
          Latitude[0],
          Longitude[0],
          Accuracy[0],
          RAIM[0],
          Seconds[0],
          COG[0],
          SOG[0],
          Heading[0],
          Unit[0],
          Display[0],
          DSC[0],
          Band[0],
          Msg22[0],
          Mode[0],
          State[0]);

    ParseN2kAISClassBPosition(N2kMsg,
          MessageID[1],
          Repeat[1],
          UserID[1],
          Latitude[1],
          Longitude[1],
          Accuracy[1],
          RAIM[1],
          Seconds[1],
          COG[1],
          SOG[1],
          Heading[1],
          Unit[1],
          Display[1],
          DSC[1],
          Band[1],
          Msg22[1],
          Mode[1],
          State[1]);

    SECTION("parsed values match set values")
      {
        REQUIRE(MessageID[0] ==                  MessageID[1]);
        REQUIRE(Repeat[0] ==                     Repeat[1]);
        REQUIRE(UserID[0] ==                     UserID[1]);
        REQUIRE(Latitude[0] ==                   Latitude[1]);
        REQUIRE(Longitude[0] ==                  Longitude[1]);
        REQUIRE(Accuracy[0] ==                   Accuracy[1]);
        REQUIRE(RAIM[0] ==                       RAIM[1]);
        REQUIRE(Seconds[0] ==                    Seconds[1]);
        REQUIRE(COG[0] ==                        COG[1]);
        REQUIRE(SOG[0] ==                        SOG[1]);
        REQUIRE(AISTransceiverInformation[0] ==  AISTransceiverInformation[1]);
        REQUIRE(Heading[0] ==                    Heading[1]);
        REQUIRE(Unit[0] ==                       Unit[1]);
        REQUIRE(Display[0] ==                    Display[1]);
        REQUIRE(DSC[0] ==                        DSC[1]);
        REQUIRE(Band[0] ==                       Band[1]);
        REQUIRE(Msg22[0] ==                      Msg22[1]);
        REQUIRE(Mode[0] ==                       Mode[1]);
        REQUIRE(State[0] ==                      State[1]);
      }
}

TEST_CASE("PGN130323 Meteorlogical Station Data")
{
  tN2kMsg N2kMsg;

  tN2kMeteorlogicalStationData data_tx;

  data_tx.Mode = N2kaismode_Assigned;
  data_tx.SystemDate = 10;
  data_tx.SystemTime = 10.8;
  data_tx.Latitude = -33.1;
  data_tx.Longitude = 151.6;
  data_tx.WindSpeed = 12.3;
  data_tx.WindDirection = 2.1;
  data_tx.WindReference = N2kWind_True_North;
  data_tx.WindGusts = 12.3;
  data_tx.AtmosphericPressure = 100;
  data_tx.OutsideAmbientAirTemperature= 19.1;
  data_tx.SetStationID("AX12");
  data_tx.SetStationName("StationName");

  SetN2kPGN130323(N2kMsg, data_tx);

  tN2kMeteorlogicalStationData data_rx;
  ParseN2kPGN130323(N2kMsg, data_rx);

  SECTION("parsed values match set values")
  {
    REQUIRE(data_tx.Mode ==                            data_rx.Mode);
    REQUIRE(data_tx.SystemDate ==                      data_rx.SystemDate);
    REQUIRE(data_tx.SystemTime ==                      data_rx.SystemTime);
    REQUIRE(data_tx.Latitude ==                        data_rx.Latitude);
    REQUIRE(data_tx.Longitude ==                       data_rx.Longitude);
    REQUIRE(data_tx.WindSpeed ==                       data_rx.WindSpeed);
    REQUIRE(data_tx.WindDirection ==                   data_rx.WindDirection);
    REQUIRE(data_tx.WindReference ==                   data_rx.WindReference);
    REQUIRE(data_tx.WindGusts ==                       data_rx.WindGusts);
    REQUIRE(data_tx.AtmosphericPressure ==             data_rx.AtmosphericPressure);
    REQUIRE(data_tx.OutsideAmbientAirTemperature ==    data_rx.OutsideAmbientAirTemperature);
    REQUIRE(strcmp(data_tx.StationID,                  data_rx.StationID) == 0);
    REQUIRE(strcmp(data_tx.StationName,                data_rx.StationName) == 0);
   }

}

TEST_CASE("PGN129041 AIS AtoN Navigation Report") 
{
  tN2kMsg N2kMsg;
  tN2kAISAtoNReportData data_tx;

  data_tx.MessageID = 5;
  data_tx.Repeat = N2kaisr_Final;
  data_tx.UserID = 7;
  data_tx.Longitude =  -33.0;
  data_tx.Latitude = 151.0;
  data_tx.Accuracy = true;
  data_tx.RAIM = true;
  data_tx.Seconds = 4;
  data_tx.Length =  52.5;
  data_tx.Beam = 21.5;
  data_tx.PositionReferenceStarboard  = 3.6;
  data_tx.PositionReferenceTrueNorth = 7.2;
  data_tx.AtoNType = N2kAISAtoN_beacon_isolated_danger;
  data_tx.OffPositionIndicator = true;
  data_tx.VirtualAtoNFlag = true;
  data_tx.AssignedModeFlag = true;
  data_tx.GNSSType = N2kGNSSt_Chayka;
  data_tx.AtoNStatus = 0x00;
  data_tx.AISTransceiverInformation =  N2kaischannel_B_VDL_transmission;
  data_tx.SetAtoNName("BINGBONG");

  SetN2kAISAtoNReport(N2kMsg, data_tx);

  tN2kAISAtoNReportData data_rx;
  ParseN2kAISAtoNReport(N2kMsg, data_rx);

  SECTION("parsed values match set values")
  {
      REQUIRE(data_tx.MessageID ==                    data_rx.MessageID);
      REQUIRE(data_tx.Repeat ==                       data_rx.Repeat);
      REQUIRE(data_tx.UserID ==                       data_rx.UserID);
      REQUIRE(data_tx.Latitude ==                     data_rx.Latitude);
      REQUIRE(data_tx.Longitude ==                    data_rx.Longitude);
      REQUIRE(data_tx.Accuracy ==                     data_rx.Accuracy);
      REQUIRE(data_tx.RAIM ==                         data_rx.RAIM);
      REQUIRE(data_tx.Seconds ==                      data_rx.Seconds);
      REQUIRE(data_tx.Length ==                       data_rx.Length);
      REQUIRE(data_tx.Beam ==                         data_rx.Beam);
      REQUIRE(data_tx.PositionReferenceStarboard ==   data_rx.PositionReferenceStarboard);
      REQUIRE(data_tx.PositionReferenceTrueNorth ==   data_rx.PositionReferenceTrueNorth);
      REQUIRE(data_tx.AtoNType ==                     data_rx.AtoNType);
      REQUIRE(data_tx.OffPositionIndicator ==         data_rx.OffPositionIndicator);
      REQUIRE(data_tx.VirtualAtoNFlag ==              data_rx.VirtualAtoNFlag);
      REQUIRE(data_tx.AssignedModeFlag ==             data_rx.AssignedModeFlag);
      REQUIRE(data_tx.GNSSType ==                     data_rx.GNSSType);
      REQUIRE(data_tx.AtoNStatus ==                   data_rx.AtoNStatus);
      REQUIRE(data_tx.AISTransceiverInformation ==    data_rx.AISTransceiverInformation);
  }
}

TEST_CASE("PGN130577 Direction Data")
{
  tN2kMsg N2kMsg;
  tN2kDataMode DataMode[2] = {N2kDD025_Simulator,N2kDD025_Simulator};
  tN2kHeadingReference CogReference[2] = {N2khr_magnetic,N2khr_magnetic};
  unsigned char SID[2] = {3,0};
  double COG[2] = {0.1,0};
  double SOG[2] = {5.0,0};
  double Heading[2] = {0.2,0};
  double SpeedThroughWater[2] = {10.0,0};
  double Set[2] = {0.15,0};
  double Drift[2] = {3.0,0};

  SetN2kDirectionData(
         N2kMsg,
         DataMode[0],
         CogReference[0],
         SID[0],
         COG[0],
         SOG[0],
         Heading[0],
         SpeedThroughWater[0],
         Set[0],
         Drift[0]);

  ParseN2kDirectionData(
         N2kMsg,
         DataMode[1],
         CogReference[1],
         SID[1],
         COG[1],
         SOG[1],
         Heading[1],
         SpeedThroughWater[1],
         Set[1],
         Drift[1]);

  SECTION("parsed values match set values")
  {
    REQUIRE(DataMode[0] ==           DataMode[1]);
    REQUIRE(CogReference[0] ==       CogReference[1]);
    REQUIRE(SID[0] ==                SID[1]);
    REQUIRE(COG[0] ==                COG[1]);
    REQUIRE(SOG[0] ==                SOG[1]);
    REQUIRE(Heading[0] ==            Heading[1]);
    REQUIRE(SpeedThroughWater[0] ==  SpeedThroughWater[1]);
    REQUIRE(Set[0] ==                Set[1]);
    REQUIRE(Drift[0] ==              Drift[1]);
  }
}

TEST_CASE("PGN127233 MOB")
{
  tN2kMsg N2kMsg;
  unsigned char SID[2] = {1,0};
  uint32_t MobEmitterId[2] = {2,0};
  tN2kMOBStatus MOBStatus[2] = {MOBNotActive,MOBNotActive};
  double ActivationTime[2] = {10.0,0};
  tN2kMOBPositionSource PositionSource[2] = {PositionReportedByMOBEmitter,PositionReportedByMOBEmitter};
  uint16_t PositionDate[2] = {20,0};
  double PositionTime[2] = {30.0,0};
  double Latitude[2] = {-33.0,0};
  double Longitude[2] = {151.0,0};
  tN2kHeadingReference COGReference[2] = {N2khr_error,N2khr_error};
  double COG[2] = {0.1,0};
  double SOG[2] = {10.0,0};
  uint32_t MMSI[2] = {1234,0};
  tN2kMOBEmitterBatteryStatus MOBEmitterBatteryStatus[2] = {Low,Low};

  SetN2kMOBNotification(N2kMsg,
         SID[0],
         MobEmitterId[0],
         MOBStatus[0],
         ActivationTime[0],
         PositionSource[0],
         PositionDate[0],
         PositionTime[0],
         Latitude[0],
         Longitude[0],
         COGReference[0],
         COG[0],
         SOG[0],
         MMSI[0],
         MOBEmitterBatteryStatus[0]);

  ParseN2kMOBNotification(N2kMsg,
        SID[1],
        MobEmitterId[1],
        MOBStatus[1],
        ActivationTime[1],
        PositionSource[1],
        PositionDate[1],
        PositionTime[1],
        Latitude[1],
        Longitude[1],
        COGReference[1],
        COG[1],
        SOG[1],
        MMSI[1],
        MOBEmitterBatteryStatus[1]);

  SECTION("parsed values match set values")
  {
    REQUIRE(SID[0]                     == SID[1]);
    REQUIRE(MobEmitterId[0]            == MobEmitterId[1]);
    REQUIRE(MOBStatus[0]               == MOBStatus[1]);
    REQUIRE(ActivationTime[0]          == ActivationTime[1]);
    REQUIRE(PositionSource[0]          == PositionSource[1]);
    REQUIRE(PositionDate[0]            == PositionDate[1]);
    REQUIRE(PositionTime[0]            == PositionTime[1]);
    REQUIRE(Latitude[0]                == Latitude[1]);
    REQUIRE(Longitude[0]               == Longitude[1]);
    REQUIRE(COGReference[0]            == COGReference[1]);
    REQUIRE(COG[0]                     == COG[1]);
    REQUIRE(SOG[0]                     == SOG[1]);
    REQUIRE(MMSI[0]                    == MMSI[1]);
    REQUIRE(MOBEmitterBatteryStatus[0] == MOBEmitterBatteryStatus[1]);
  }

}

TEST_CASE("PGN127237 HeadingTrackControl")
{
  tN2kMsg N2kMsg;
  tN2kOnOff RudderLimitExceeded[2] = {N2kOnOff_On,N2kOnOff_On};
  tN2kOnOff OffHeadingLimitExceeded[2] = {N2kOnOff_Off,N2kOnOff_Off};
  tN2kOnOff OffTrackLimitExceeded[2] = {N2kOnOff_On,N2kOnOff_On};
  tN2kOnOff Override[2] = {N2kOnOff_Off,N2kOnOff_Off};
  tN2kSteeringMode SteeringMode[2] = {N2kSM_FollowUpDevice,N2kSM_FollowUpDevice};
  tN2kTurnMode TurnMode[2] = {N2kTM_RadiusControlled,N2kTM_RadiusControlled};
  tN2kHeadingReference HeadingReference[2] = {N2khr_Unavailable,N2khr_Unavailable};
  tN2kRudderDirectionOrder CommandedRudderDirection[2] = {N2kRDO_Unavailable,N2kRDO_Unavailable};
  double CommandedRudderAngle[2] = {0.1,0.0};
  double HeadingToSteerCourse[2] = {0.2,0.0};
  double Track[2] = {0.3,0.0};
  double RudderLimit[2] = {0.4,0.0};
  double OffHeadingLimit[2] = {0.5,0.0};
  double RadiusOfTurnOrder[2] = {10,0.0};
  double RateOfTurnOrder[2] = {0.7,0.0};
  double OffTrackLimit[2] = {4,0.0};
  double VesselHeading[2] = {0.9,0.0};

  SetN2kHeadingTrackControl(N2kMsg,
    RudderLimitExceeded[0],
    OffHeadingLimitExceeded[0],
    OffTrackLimitExceeded[0],
    Override[0],
    SteeringMode[0],
    TurnMode[0],
    HeadingReference[0],
    CommandedRudderDirection[0],
    CommandedRudderAngle[0],
    HeadingToSteerCourse[0],
    Track[0],
    RudderLimit[0],
    OffHeadingLimit[0],
    RadiusOfTurnOrder[0],
    RateOfTurnOrder[0],
    OffTrackLimit[0],
    VesselHeading[0]);

  ParseN2kHeadingTrackControl(N2kMsg,
    RudderLimitExceeded[1],
    OffHeadingLimitExceeded[1],
    OffTrackLimitExceeded[1],
    Override[1],
    SteeringMode[1],
    TurnMode[1],
    HeadingReference[1],
    CommandedRudderDirection[1],
    CommandedRudderAngle[1],
    HeadingToSteerCourse[1],
    Track[1],
    RudderLimit[1],
    OffHeadingLimit[1],
    RadiusOfTurnOrder[1],
    RateOfTurnOrder[1],
    OffTrackLimit[1],
    VesselHeading[1]);

  SECTION("parsed values match set values")
  {
    REQUIRE(RudderLimitExceeded[0] == RudderLimitExceeded[1]);
    REQUIRE(OffHeadingLimitExceeded[1] == OffHeadingLimitExceeded[1]);
    REQUIRE(OffTrackLimitExceeded[0] == OffTrackLimitExceeded[1]);
    REQUIRE(Override[0] == Override[1]);
    REQUIRE(SteeringMode[0] == SteeringMode[1]);
    REQUIRE(TurnMode[0]== TurnMode[1]);
    REQUIRE(HeadingReference[0] == HeadingReference[1]);
    REQUIRE(CommandedRudderDirection[0] == CommandedRudderDirection[1]);
    REQUIRE(CommandedRudderAngle[0] == CommandedRudderAngle[1]);
    REQUIRE(HeadingToSteerCourse[0] == HeadingToSteerCourse[1]);
    REQUIRE(Track[0] == Track[1]);
    REQUIRE(RudderLimit[0] == RudderLimit[1]);
    REQUIRE(OffHeadingLimit[0] == OffHeadingLimit[1]);
    REQUIRE(RadiusOfTurnOrder[0] == RadiusOfTurnOrder[1]);
    REQUIRE(RateOfTurnOrder[0] == Approx(RateOfTurnOrder[1]));
    REQUIRE(OffTrackLimit[0] == OffTrackLimit[1]);
    REQUIRE(VesselHeading[0] == VesselHeading[1]);
  }
}

TEST_CASE("PGN129285 Route/WP information")
{
   tN2kMsg N2kMsg1, N2kMsg2;
   uint16_t Start = 1;
   uint16_t Database = 2;
   uint16_t Route = 3;
   tN2kNavigationDirection NavDirectiont =  N2kdir_reverse;
   const char* RouteName = "test route";

   SetN2kPGN129285(N2kMsg1, Start, Database, Route, NavDirectiont, (char*)RouteName, N2kDD002_Yes );

   SetN2kRouteWPInfo(N2kMsg2, Start, Database, Route, NavDirectiont, (char*)RouteName, N2kDD002_Yes );

   SECTION("values of both calls produce identical messages")
   {
      REQUIRE(N2kMsg1.DataLen == N2kMsg2.DataLen);
      for(int i = 0; i < N2kMsg1.DataLen;i++)
      {
         REQUIRE(N2kMsg1.Data[i] == N2kMsg2.Data[i]);
      }
   }
}

TEST_CASE("PGN129802 AIS Safety Related Broadcast Message")
{
   tN2kMsg N2kMsg;
   uint8_t MessageID[2] = {27, 27};
   tN2kAISRepeat Repeat[2] = {N2kaisr_First, N2kaisr_First};
   uint32_t SourceID[2] = {2, 2};
   tN2kAISTransceiverInformation AISTransceiverInformation[2] = {N2kaischannel_B_VDL_transmission, N2kaischannel_B_VDL_transmission};
   const char * SafetyRelatedText_TX = "MOB";
   size_t buflen = 36;
   char SafetyRelatedText_RX[buflen];

   SetN2kAISSafetyRelatedBroadcastMsg(N2kMsg, MessageID[0], Repeat[0], SourceID[0], AISTransceiverInformation[0], (char*)SafetyRelatedText_TX);
   ParseN2kAISSafetyRelatedBroadcastMsg(N2kMsg, MessageID[1], Repeat[1], SourceID[1], AISTransceiverInformation[1], SafetyRelatedText_RX, buflen);

   SECTION("parsed values match set values")
   {
     REQUIRE(MessageID[0] == MessageID[1]);
     REQUIRE(Repeat[0] == Repeat[1]);
     REQUIRE(SourceID[0] == SourceID[1]);
     REQUIRE(AISTransceiverInformation[0] == AISTransceiverInformation[1]);
     REQUIRE(strcmp(SafetyRelatedText_TX,SafetyRelatedText_RX) == 0);
   }
}

TEST_CASE("PGN127501 Binary status report and PGN127502 Switch Bank Control")
{
  const int numItems = 28; // each Bank can contain up to 28 Items
  tN2kMsg N2kMsg;
  tN2kBinaryStatus switchBank_init, switchBank_expect, switchBank_actual;
  tN2kOnOff itemStatus_expect[numItems];
  N2kResetBinaryStatus(switchBank_init);
  N2kResetBinaryStatus(switchBank_expect);
  N2kResetBinaryStatus(switchBank_actual);
  for (int i = 0; i < numItems; i++)
  {
    itemStatus_expect[i] = ((i+1) & 1) ? (N2kOnOff_On) : (N2kOnOff_Off); // every even switch off, every odd switch on
    N2kSetStatusBinaryOnStatus(switchBank_expect, itemStatus_expect[i], i + 1);
  }

  SECTION("BinaryStatus helper functions work properly")
  {  
    for (int i = 0; i < numItems; i++)
    {
      tN2kOnOff itemStatus_actual = N2kGetStatusOnBinaryStatus(switchBank_init, i + 1);
      REQUIRE(itemStatus_actual == N2kOnOff_Unavailable); // initial value, all set to "unavailable"
    }
    for (int i = 0; i < numItems; i++)
    {
      tN2kOnOff itemStatus_actual = N2kGetStatusOnBinaryStatus(switchBank_expect, i + 1);
      REQUIRE(itemStatus_actual == itemStatus_expect[i]);
    }
  }

  const uint8_t bankNo_expect = 3;
  uint8_t bankNo_actual = 0;

  SetN2kBinaryStatus(N2kMsg, bankNo_expect, switchBank_expect); //PGN127501
  ParseN2kBinaryStatus(N2kMsg, bankNo_actual, switchBank_actual); //PGN127501

  SECTION("PGN127501 parsed values match set values")
  {
    REQUIRE(bankNo_expect == bankNo_actual);
    for (int i = 0; i < numItems; i++)
    {
      tN2kOnOff itemStatus_actual = N2kGetStatusOnBinaryStatus(switchBank_actual, i + 1);
      REQUIRE(itemStatus_actual == itemStatus_expect[i]);
    }
  }

  N2kResetBinaryStatus(switchBank_actual);
  bankNo_actual = 0;
  SetN2kSwitchbankControl(N2kMsg, bankNo_expect, switchBank_expect); //PGN127502
  ParseN2kSwitchbankControl(N2kMsg, bankNo_actual, switchBank_actual); //PGN127502

  SECTION("PGN127502 parsed values match set values")
  {
    REQUIRE(bankNo_expect == bankNo_actual);
    for (int i = 0; i < numItems; i++)
    {
      tN2kOnOff itemStatus_actual = N2kGetStatusOnBinaryStatus(switchBank_actual, i + 1);
      REQUIRE(itemStatus_actual == itemStatus_expect[i]);
    }
  }
}
