/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "NMEA2000 Library", "index.html", [
    [ "NMEA2000 library for C++", "index.html", [
      [ "Recommended hardware", "index.html#autotoc_md0", null ],
      [ "Hardware depended libraries", "index.html#autotoc_md1", null ],
      [ "References", "index.html#secRef", [
        [ "Thanks", "index.html#autotoc_md2", null ],
        [ "MIT license", "index.html#autotoc_md3", null ]
      ] ]
    ] ],
    [ "Reference for C++ library", "pg_lib_ref.html", [
      [ "Introduction", "pg_lib_ref.html#autotoc_md4", null ],
      [ "History", "pg_lib_ref.html#autotoc_md5", null ],
      [ "Start from scratch and make your first NMEA2000 device with Arduino IDE", "pg_lib_ref.html#autotoc_md6", null ],
      [ "Preparing to use NMEA2000 library", "pg_lib_ref.html#autotoc_md7", null ],
      [ "Try NMEA2000 library with TemperatureMonitor example", "pg_lib_ref.html#autotoc_md8", [
        [ "Try on PC only", "pg_lib_ref.html#autotoc_md9", null ],
        [ "A bit deeper look in to the TemperatureMonitor example", "pg_lib_ref.html#autotoc_md10", [
          [ "Change to use read real data", "pg_lib_ref.html#autotoc_md11", null ],
          [ "Remove unnecessary messages", "pg_lib_ref.html#autotoc_md12", null ],
          [ "Change the device and product information", "pg_lib_ref.html#autotoc_md13", null ]
        ] ],
        [ "Connecting your sample device to the NMEA2000 bus", "pg_lib_ref.html#autotoc_md14", null ],
        [ "Try NMEA2000 library with WindMonitor example", "pg_lib_ref.html#autotoc_md15", null ]
      ] ],
      [ "Arduino Programming IDE", "pg_lib_ref.html#autotoc_md16", [
        [ "Getting started with Arduino Programming IDE", "pg_lib_ref.html#autotoc_md17", null ],
        [ "Arduino Libraries", "pg_lib_ref.html#autotoc_md18", null ],
        [ "Importing library from zip", "pg_lib_ref.html#autotoc_md19", null ],
        [ "Importing libraries from GitHub", "pg_lib_ref.html#autotoc_md20", null ]
      ] ],
      [ "NMEA2000 library reference", "pg_lib_ref.html#autotoc_md21", [
        [ "Relevant Classes", "pg_lib_ref.html#autotoc_md22", [
          [ "Abstract base class tNMEA2000", "pg_lib_ref.html#autotoc_md23", null ],
          [ "Abstract base class tNMEA2000::tMsgHandler", "pg_lib_ref.html#autotoc_md24", null ],
          [ "Inherited classes according to your Board", "pg_lib_ref.html#autotoc_md25", [
            [ "Inherited class tNMEA2000_mcp", "pg_lib_ref.html#autotoc_md26", null ],
            [ "Inherited class tNMEA2000_due", "pg_lib_ref.html#autotoc_md27", null ],
            [ "Inherited class tNMEA2000_teensy", "pg_lib_ref.html#autotoc_md28", null ],
            [ "Inherited class tNMEA2000_Teensyx", "pg_lib_ref.html#autotoc_md29", null ],
            [ "Inherited class tNMEA2000_esp32", "pg_lib_ref.html#autotoc_md30", null ],
            [ "Inherited class tNMEA2000_avr", "pg_lib_ref.html#autotoc_md31", null ]
          ] ],
          [ "Message container class tN2kMsg", "pg_lib_ref.html#autotoc_md32", null ],
          [ "Group function (PGN 126208) handler class tN2kGroupFunctionHandler", "pg_lib_ref.html#autotoc_md33", null ],
          [ "Device list collector class tN2kDeviceList", "pg_lib_ref.html#autotoc_md34", null ]
        ] ],
        [ "Using tNMEA2000", "pg_lib_ref.html#autotoc_md35", [
          [ "Device modes", "pg_lib_ref.html#secDeviceModes", null ],
          [ "Message forwarding", "pg_lib_ref.html#secMessageforwarding", null ],
          [ "Debug mode", "pg_lib_ref.html#descDebugMode", null ],
          [ "Multi device support", "pg_lib_ref.html#descMultiDeviceSupport", null ],
          [ "Running library", "pg_lib_ref.html#descRunningLibrary", [
            [ "Some timing examples", "pg_lib_ref.html#autotoc_md36", null ],
            [ "System messages and library internal functionality", "pg_lib_ref.html#descSystemMessages", null ]
          ] ],
          [ "Member functions", "pg_lib_ref.html#autotoc_md37", null ]
        ] ]
      ] ]
    ] ],
    [ "Setting Up Hardware Environment", "page_h_w_set_up.html", [
      [ "Some terminology", "page_h_w_set_up.html#autotoc_md126", null ],
      [ "Recommended hardware", "page_h_w_set_up.html#secTRecHW", [
        [ "CAN controller", "page_h_w_set_up.html#subCANcontr", null ],
        [ "CAN Transceiver", "page_h_w_set_up.html#subCANtrans", [
          [ "Recommended transceiver", "page_h_w_set_up.html#subsubRecTra", null ],
          [ "Unisolated transceiver", "page_h_w_set_up.html#subsubUnIso", [
            [ "Microchip MCP2562", "page_h_w_set_up.html#autotoc_md127", null ]
          ] ],
          [ "Isolated transceiver", "page_h_w_set_up.html#subsubIso", [
            [ "TI ISO1050 Isolated CAN Transceiver", "page_h_w_set_up.html#autotoc_md128", null ]
          ] ]
        ] ]
      ] ],
      [ "Schematics sample", "page_h_w_set_up.html#secSchem", [
        [ "Feeding power from NMEA2000", "page_h_w_set_up.html#subFeedPwr", null ],
        [ "Teensy 3.2, simple unisolated", "page_h_w_set_up.html#subTEunIso", null ],
        [ "Teensy 3.2, simple isolated", "page_h_w_set_up.html#subTEiso", null ],
        [ "More protection for tranceiver", "page_h_w_set_up.html#subTEperf", null ],
        [ "Feed isolated power from NMEA2000", "page_h_w_set_up.html#subFeedPwe", null ]
      ] ],
      [ "Testing devices", "page_h_w_set_up.html#secTesting", null ],
      [ "Most common hardware errors", "page_h_w_set_up.html#secCommErr", null ],
      [ "NMEA2000 bus", "page_h_w_set_up.html#secNMEA2000bus", [
        [ "Bus topology", "page_h_w_set_up.html#subTopology", null ],
        [ "Bus connectors", "page_h_w_set_up.html#subConnetor", null ],
        [ "Bus cables", "page_h_w_set_up.html#subCable", null ],
        [ "Prepare for failures", "page_h_w_set_up.html#secFailure", null ]
      ] ]
    ] ],
    [ "Get your Project Started", "get_started.html", [
      [ "Memory requirements", "get_started.html#secMem", null ],
      [ "Hardware setup", "get_started.html#secHWSet", null ],
      [ "Breakout Boards", "get_started.html#autotoc_md129", [
        [ "Isolated breakout board", "get_started.html#autotoc_md130", null ],
        [ "Teensy 4.0, 4.1", "get_started.html#autotoc_md131", null ],
        [ "Teensy 4.0", "get_started.html#autotoc_md132", null ],
        [ "Arduino Due", "get_started.html#autotoc_md133", null ],
        [ "Arduino Mega", "get_started.html#autotoc_md134", null ]
      ] ],
      [ "Schematics for standalone CAN transceiver", "get_started.html#autotoc_md135", [
        [ "Teensy 3.2", "get_started.html#autotoc_md136", null ],
        [ "Arduino Due", "get_started.html#autotoc_md137", null ],
        [ "Arduino Mega", "get_started.html#autotoc_md138", null ],
        [ "ATmegaxxM1", "get_started.html#autotoc_md139", null ]
      ] ],
      [ "Hardware depended libraries", "get_started.html#secHWlib", [
        [ "Teensy 4.0/4.1 boards with internal CAN", "get_started.html#subT4Libraries", null ],
        [ "Teensy 3.1/3.2 or 3.5/3.6 board with internal CAN", "get_started.html#autotoc_md140", null ],
        [ "ESP32 boards  with internal CAN", "get_started.html#autotoc_md141", null ],
        [ "Arduino Due board with internal CAN", "get_started.html#autotoc_md142", null ],
        [ "Boards with MCP2515 CAN bus controller (e.g. Arduino Mega, ESP8266)", "get_started.html#autotoc_md143", null ],
        [ "AVR processors with internal CAN", "get_started.html#autotoc_md144", null ],
        [ "MBED boards", "get_started.html#autotoc_md145", null ],
        [ "Raspberry Pi", "get_started.html#autotoc_md146", null ]
      ] ],
      [ "Quick Example", "get_started.html#autotoc_md147", null ],
      [ "Forcing CAN board dependent \"driver\"", "get_started.html#autotoc_md148", [
        [ "For use with Teensy 4.x (also with end of life boards 3.1/3.2/3.5/3.6)", "get_started.html#autotoc_md149", null ],
        [ "For use with Teensy 3.1/3.2/3.5/3.6 board and FlexCan", "get_started.html#autotoc_md150", null ],
        [ "For use with ESP32", "get_started.html#autotoc_md151", null ],
        [ "For use board with MCP2515 SPI can bus tranceiver and mcp_can library", "get_started.html#autotoc_md152", null ],
        [ "For use with Arduino due and due_can library", "get_started.html#autotoc_md153", null ],
        [ "For use with Atmel AVR processors internal CAN controller", "get_started.html#autotoc_md154", null ]
      ] ]
    ] ],
    [ "List of supported NMEA2000 PGNs", "list_msg.html", null ],
    [ "Information for developers", "md_6_develop_info.html", [
      [ "Running Unit Tests", "md_6_develop_info.html#autotoc_md156", null ],
      [ "Library document generation", "md_6_develop_info.html#libDocuGen", [
        [ "General Content inside the Documentation", "md_6_develop_info.html#autotoc_md157", null ],
        [ "Documenting the Code", "md_6_develop_info.html#autotoc_md158", null ],
        [ "File Structure of the Documentation", "md_6_develop_info.html#autotoc_md159", null ],
        [ "Generating a new Local Documentation", "md_6_develop_info.html#autotoc_md160", null ],
        [ "Copying the online Documentation to local PC", "md_6_develop_info.html#autotoc_md161", null ],
        [ "Automatic Documentation Deployment with github ACTION", "md_6_develop_info.html#autotoc_md162", null ]
      ] ]
    ] ],
    [ "Additional Information about NMEA2000", "md_7_glossary.html", [
      [ "NAME", "md_7_glossary.html#secRefTermNAME", null ],
      [ "Address claiming", "md_7_glossary.html#secRefTermAddressClaiming", null ],
      [ "NMEA2000 instances", "md_7_glossary.html#autotoc_md164", [
        [ "Different types of instances", "md_7_glossary.html#autotoc_md165", [
          [ "Device instance", "md_7_glossary.html#autotoc_md166", null ],
          [ "Data instances", "md_7_glossary.html#autotoc_md167", null ],
          [ "System instance", "md_7_glossary.html#autotoc_md168", null ]
        ] ]
      ] ],
      [ "SID", "md_7_glossary.html#secRefTermSID", null ],
      [ "Manufacturer Code", "md_7_glossary.html#secRefTermManufacturerCode", null ],
      [ "NMEA2000 certification", "md_7_glossary.html#secRefNMEA2000Certification", null ],
      [ "Relation between CAN ID and NMEA2000", "md_7_glossary.html#autotoc_md169", null ],
      [ "List of registered manufacturer codes", "md_7_glossary.html#secRefMfgCodes", null ]
    ] ],
    [ "Changes to the Library", "changes.html", [
      [ "09.02.2024", "changes.html#autotoc_md170", null ],
      [ "06.02.2024", "changes.html#autotoc_md171", null ],
      [ "02.02.2024", "changes.html#autotoc_md172", null ],
      [ "22.01.2024", "changes.html#autotoc_md173", null ],
      [ "21.01.2024", "changes.html#autotoc_md174", null ],
      [ "13.01.2024", "changes.html#autotoc_md175", null ],
      [ "08.12.2023", "changes.html#autotoc_md176", null ],
      [ "04.10.2023", "changes.html#autotoc_md177", null ],
      [ "12.09.2023", "changes.html#autotoc_md178", null ],
      [ "25.07.2023", "changes.html#autotoc_md179", null ],
      [ "20.05.2023", "changes.html#autotoc_md180", null ],
      [ "19.05.2023", "changes.html#autotoc_md181", null ],
      [ "08.02.2023", "changes.html#autotoc_md182", null ],
      [ "01.02.2023", "changes.html#autotoc_md183", null ],
      [ "21.12.2022", "changes.html#autotoc_md184", null ],
      [ "27.11.2022", "changes.html#autotoc_md185", null ],
      [ "27.10.2022", "changes.html#autotoc_md186", null ],
      [ "26.10.2022", "changes.html#autotoc_md187", null ],
      [ "03.10.2022", "changes.html#autotoc_md188", null ],
      [ "11.08.2022", "changes.html#autotoc_md189", null ],
      [ "06.07.2022", "changes.html#autotoc_md190", null ],
      [ "07.04.2022", "changes.html#autotoc_md191", null ],
      [ "05.09.2021", "changes.html#autotoc_md192", null ],
      [ "23.08.2021", "changes.html#autotoc_md193", null ],
      [ "04.03.2021", "changes.html#autotoc_md194", null ],
      [ "10.02.2021", "changes.html#autotoc_md195", null ],
      [ "06.02.2021", "changes.html#autotoc_md196", null ],
      [ "15.01.2021", "changes.html#autotoc_md197", null ],
      [ "05.01.2021", "changes.html#autotoc_md198", null ],
      [ "08.10.2020", "changes.html#autotoc_md199", null ],
      [ "23.08.2020", "changes.html#autotoc_md200", null ],
      [ "05.08.2020", "changes.html#autotoc_md201", null ],
      [ "16.07.2020", "changes.html#autotoc_md202", null ],
      [ "27.06.2020", "changes.html#autotoc_md203", null ],
      [ "28.01.2019", "changes.html#autotoc_md204", null ],
      [ "20.10.2019", "changes.html#autotoc_md205", null ],
      [ "17.10.2019", "changes.html#autotoc_md206", null ],
      [ "07.07.2019", "changes.html#autotoc_md207", null ],
      [ "03.03.2019", "changes.html#autotoc_md208", null ],
      [ "14.01.2019", "changes.html#autotoc_md209", null ],
      [ "04.08.2018", "changes.html#autotoc_md210", null ],
      [ "18.07.2018", "changes.html#autotoc_md211", null ],
      [ "09.05.2018", "changes.html#autotoc_md212", null ],
      [ "10.04.2018", "changes.html#autotoc_md213", null ],
      [ "06.04.2018", "changes.html#autotoc_md214", null ],
      [ "04.04.2018", "changes.html#autotoc_md215", null ],
      [ "26.03.2018", "changes.html#autotoc_md216", null ],
      [ "13.03.2018", "changes.html#autotoc_md217", null ],
      [ "23.02.2018", "changes.html#autotoc_md218", null ],
      [ "16.02.2018", "changes.html#autotoc_md219", null ],
      [ "29.01.2018", "changes.html#autotoc_md220", null ],
      [ "15.01.2018", "changes.html#autotoc_md221", null ],
      [ "07.01.2018", "changes.html#autotoc_md222", null ],
      [ "02.12.2017", "changes.html#autotoc_md223", null ],
      [ "29.11.2017", "changes.html#autotoc_md224", null ],
      [ "27.11.2017", "changes.html#autotoc_md225", null ],
      [ "14.11.2017", "changes.html#autotoc_md226", null ],
      [ "09.11.2017", "changes.html#autotoc_md227", null ],
      [ "31.10.2017", "changes.html#autotoc_md228", null ],
      [ "14.10.2017", "changes.html#autotoc_md229", null ],
      [ "12.10.2017", "changes.html#autotoc_md230", null ],
      [ "10.10.2017", "changes.html#autotoc_md231", null ],
      [ "02.10.2017", "changes.html#autotoc_md232", null ],
      [ "26.09.2017", "changes.html#autotoc_md233", null ],
      [ "31.08.2017", "changes.html#autotoc_md234", null ],
      [ "31.07.2017", "changes.html#autotoc_md235", null ],
      [ "29.07.2017", "changes.html#autotoc_md236", null ],
      [ "26.06.2017", "changes.html#autotoc_md237", null ],
      [ "25.06.2017", "changes.html#autotoc_md238", null ],
      [ "22.06.2017", "changes.html#autotoc_md239", null ],
      [ "19.06.2017", "changes.html#autotoc_md240", null ],
      [ "13.06.2017", "changes.html#autotoc_md241", null ],
      [ "11.06.2017", "changes.html#autotoc_md242", null ],
      [ "05.06.2017", "changes.html#autotoc_md243", null ],
      [ "28.05.2017", "changes.html#autotoc_md244", null ],
      [ "08.04.2017", "changes.html#autotoc_md245", null ],
      [ "09.03.2017", "changes.html#autotoc_md246", null ],
      [ "07.03.2017", "changes.html#autotoc_md247", null ],
      [ "05.03.2017", "changes.html#autotoc_md248", null ],
      [ "08.02.2017", "changes.html#autotoc_md249", null ],
      [ "22.01.2017", "changes.html#autotoc_md250", null ],
      [ "01.01.2017", "changes.html#autotoc_md251", null ],
      [ "20.12.2016", "changes.html#autotoc_md252", null ],
      [ "17.12.2016", "changes.html#autotoc_md253", null ],
      [ "16.12.2016", "changes.html#autotoc_md254", null ],
      [ "01.12.2016", "changes.html#autotoc_md255", null ],
      [ "12.11.2016", "changes.html#autotoc_md256", null ],
      [ "11.11.2016", "changes.html#autotoc_md257", null ],
      [ "18.10.2016", "changes.html#autotoc_md258", null ],
      [ "19.09.2016", "changes.html#autotoc_md259", null ],
      [ "17.09.2016", "changes.html#autotoc_md260", null ],
      [ "12.09.2016", "changes.html#autotoc_md261", null ],
      [ "09.08.2016", "changes.html#autotoc_md262", null ],
      [ "06.08.2016", "changes.html#autotoc_md263", null ],
      [ "30.07.2016", "changes.html#autotoc_md264", null ],
      [ "19.07.2016", "changes.html#autotoc_md265", null ],
      [ "12.07.2016", "changes.html#autotoc_md266", null ],
      [ "25.06.2016", "changes.html#autotoc_md267", null ],
      [ "23.03.2016", "changes.html#autotoc_md268", null ],
      [ "13.03.2016", "changes.html#autotoc_md269", null ],
      [ "12.03.2016", "changes.html#autotoc_md270", null ],
      [ "09.03.2016", "changes.html#autotoc_md271", null ],
      [ "08.03.2016", "changes.html#autotoc_md272", null ],
      [ "02.02.2016", "changes.html#autotoc_md273", null ],
      [ "23.01.2016", "changes.html#autotoc_md274", null ],
      [ "05.12.2015", "changes.html#autotoc_md275", null ]
    ] ],
    [ "Todo List", "todo.html", null ],
    [ "Modules", "modules.html", "modules" ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", "functions_vars" ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Enumerator", "functions_eval.html", null ],
        [ "Related Functions", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", "globals_eval" ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_actisense_reader_8cpp.html",
"_n2k_msg_8cpp.html#ae061f6de95e9577a3185242fdaa4772d",
"_n2k_types_8h.html#ga6f360c10d61b4c8559979a1a447222f0a8bda5620ddb9b759dd8ec72a28b26de0",
"_n2k_types_8h.html#gaf6a4cf563abea3f9d3a1addb314b2dd1a6c65b86b7a5de0f4293cf3ab52160aa5",
"_n_m_e_a2000_std_types_8h.html#ga618c24960fcbfb338c5d29ef034c5cc1aff80811073a9253ecf1efb19e3aec349",
"classt_actisense_reader.html#ae1d7d55c931d15af44e036e392791da2",
"classt_n2k_sync_scheduler.html#ae631faca16a9be1cffd6379ad3107c7c",
"classt_n_m_e_a2000_1_1t_internal_device.html#a08cf97fc8d7bb983287ad78fecd61532",
"group__group__msg_parsers.html#ga1e125b342a3d7fce03cd1034461238c2",
"group__group__msg_types.html#ga618c24960fcbfb338c5d29ef034c5cc1",
"uniont_n2k_d_d223.html#a6b9b065f28da5843b7ef29d8012acaa2"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';