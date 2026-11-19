#include <catch.hpp>
#include <N2kMessages.h>
#include <NMEA0183Messages.h>
#include "N2kDataToNMEA0183.h"
#include <string>
#include <vector>
#include <cstdlib>

namespace {
std::vector<std::vector<std::string> > messages;
void capture(const tNMEA0183Msg &msg) {
  std::vector<std::string> fields(1, msg.MessageCode());
  for (unsigned i=0; i<msg.FieldCount(); ++i) fields.push_back(msg.Field(i));
  messages.push_back(fields);
}
struct Converter : tN2kDataToNMEA0183 {
  Converter() : tN2kDataToNMEA0183(nullptr, nullptr) {
    messages.clear();
    SetSendNMEA0183MessageCallback(capture);
  }
  void variation(double degrees) {
    tN2kMsg msg;
    SetN2kMagneticVariation(msg, 0, N2kmagvar_Manual, 20000, DegToRad(degrees));
    HandleMsg(msg);
  }
  void heading(double degrees, tN2kHeadingReference ref, double deviation=N2kDoubleNA,
               double variation=N2kDoubleNA) {
    tN2kMsg msg;
    SetN2kPGN127250(msg, 0, N2kIsNA(degrees)?degrees:DegToRad(degrees),
      N2kIsNA(deviation)?deviation:DegToRad(deviation),
      N2kIsNA(variation)?variation:DegToRad(variation), ref);
    HandleMsg(msg);
  }
  void speed() {
    tN2kMsg msg;
    SetN2kBoatSpeed(msg, 0, 2, N2kDoubleNA);
    HandleMsg(msg);
  }
  void course(double degrees, tN2kHeadingReference ref) {
    tN2kMsg msg;
    SetN2kCOGSOGRapid(msg, 0, ref, N2kIsNA(degrees)?degrees:DegToRad(degrees), 2);
    HandleMsg(msg);
  }
  void rmc() {
    Latitude=50; Longitude=-1; DaysSince1970=20000; SecondsSinceMidnight=43200;
    NextRMCSend=0;
    SendRMC();
  }
};
const std::vector<std::string> &last(const char *code) {
  REQUIRE_FALSE(messages.empty());
  REQUIRE(messages.back()[0]==code);
  return messages.back();
}
void degree(const std::vector<std::string> &m, unsigned field, double expected) {
  REQUIRE(m.size()>field+1);
  REQUIRE_FALSE(m[field+1].empty());
  CHECK(std::atof(m[field+1].c_str())==Approx(expected).margin(0.11));
}
void empty(const std::vector<std::string> &m, unsigned field) {
  REQUIRE(m.size()>field+1);
  REQUIRE(m[field+1].empty());
}
}

TEST_CASE("Conversion applies east and west variation in both directions") {
  for (double variation : {-10.0, 10.0}) {
    CAPTURE(variation);
    Converter c;
    c.variation(variation);
    c.heading(100, N2khr_magnetic);
    degree(last("HDG"), 0, 100);
    c.speed();
    degree(last("VHW"), 0, 100+variation);
    degree(last("VHW"), 2, 100);
    c.heading(100+variation, N2khr_true);
    degree(last("HDG"), 0, 100);
    c.speed();
    degree(last("VHW"), 0, 100+variation);
    degree(last("VHW"), 2, 100);
    c.course(100, N2khr_magnetic);
    degree(last("VTG"), 0, 100+variation);
    degree(last("VTG"), 2, 100);
    c.rmc();
    degree(last("RMC"), 7, 100+variation);
    c.course(100+variation, N2khr_true);
    degree(last("VTG"), 0, 100+variation);
    degree(last("VTG"), 2, 100);
  }
}

TEST_CASE("Conversion keeps missing reference data unavailable") {
  Converter c;
  c.heading(100, N2khr_true);
  degree(last("HDT"), 0, 100);
  c.speed();
  degree(last("VHW"), 0, 100);
  empty(last("VHW"), 2);
  c.heading(100, N2khr_magnetic);
  degree(last("HDM"), 0, 100);
  c.speed();
  empty(last("VHW"), 0);
  degree(last("VHW"), 2, 100);
  c.course(100, N2khr_magnetic);
  empty(last("VTG"), 0);
  degree(last("VTG"), 2, 100);
  c.rmc();
  empty(last("RMC"), 7);
  c.course(100, N2khr_true);
  degree(last("VTG"), 0, 100);
  empty(last("VTG"), 2);
}

TEST_CASE("Conversion does not perform arithmetic on unavailable angles") {
  Converter c;
  c.variation(10);
  c.course(N2kDoubleNA, N2khr_magnetic);
  empty(last("VTG"), 0);
  empty(last("VTG"), 2);
  c.heading(100, N2khr_magnetic);
  c.heading(N2kDoubleNA, N2khr_magnetic);
  c.speed();
  empty(last("VHW"), 0);
  empty(last("VHW"), 2);
}

TEST_CASE("Conversion wraps angles at north") {
  for (double variation : {-10.0, 10.0}) {
    Converter c;
    c.variation(variation);
    const double magnetic=variation>0?355:5;
    const double truth=variation>0?5:355;
    c.heading(magnetic, N2khr_magnetic);
    c.speed();
    degree(last("VHW"), 0, truth);
    degree(last("VHW"), 2, magnetic);
    c.heading(truth, N2khr_true);
    degree(last("HDG"), 0, magnetic);
    c.course(magnetic, N2khr_magnetic);
    degree(last("VTG"), 0, truth);
    c.course(truth, N2khr_true);
    degree(last("VTG"), 2, magnetic);
  }
}

TEST_CASE("Alternating true and magnetic heading preserves deviation") {
  Converter c;
  c.heading(100, N2khr_magnetic, 2, 10);
  degree(last("HDG"), 0, 100);
  degree(last("HDG"), 1, 2);
  c.speed();
  degree(last("VHW"), 0, 112);
  degree(last("VHW"), 2, 102);
  c.heading(112, N2khr_true);
  degree(last("HDG"), 0, 100);
  degree(last("HDG"), 1, 2);
  c.speed();
  degree(last("VHW"), 0, 112);
  c.heading(100, N2khr_magnetic); // Missing deviation on magnetic input means zero.
  c.speed();
  degree(last("VHW"), 0, 110);
}

TEST_CASE("True heading can supply variation without a separate message") {
  Converter c;
  c.heading(110, N2khr_true, N2kDoubleNA, 10);
  degree(last("HDG"), 0, 100);
  c.speed();
  degree(last("VHW"), 2, 100);
}

TEST_CASE("Unavailable reference never becomes a true heading or course") {
  Converter c;
  c.variation(10);
  c.heading(100, N2khr_true);
  messages.clear();
  c.heading(100, N2khr_Unavailable);
  REQUIRE(messages.empty());
  c.speed();
  empty(last("VHW"), 0);
  empty(last("VHW"), 2);
  c.course(100, N2khr_Unavailable);
  empty(last("VTG"), 0);
  empty(last("VTG"), 2);
  degree(last("VTG"), 4, 2*3600/1852.0); // speed is still valid
}

TEST_CASE("West deviation is retained when true and magnetic headings alternate") {
  Converter c;
  c.heading(100, N2khr_magnetic, -2, -10);
  degree(last("HDG"), 0, 100);
  degree(last("HDG"), 1, 2);
  REQUIRE(last("HDG")[3]=="W");
  REQUIRE(last("HDG")[5]=="W");
  c.speed();
  degree(last("VHW"), 0, 88);
  degree(last("VHW"), 2, 98);
  c.heading(88, N2khr_true);
  degree(last("HDG"), 0, 100);
  c.speed();
  degree(last("VHW"), 0, 88);
  degree(last("VHW"), 2, 98);
}

TEST_CASE("Zero variation preserves an already true course") {
  Converter c;
  c.variation(0);
  c.course(100, N2khr_true);
  degree(last("VTG"), 0, 100);
  degree(last("VTG"), 2, 100);
  degree(last("VTG"), 4, 2*3600/1852.0);
}
