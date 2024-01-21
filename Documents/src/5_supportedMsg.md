# List of supported NMEA2000 PGNs {#listMsg} 

This list gives a short overview of all supported NMEA2000 PGNs. The most of these functions are provided in 
ef N2kMessages.h . E.g., functions for Maretron proprietary messages has been declared in N2kMaretron.h . If message you need is not listed and not in N2kMessages.h, you can create your own module and write setter/parser functions. Please do not try to do PR, before you have tested your functions with orther device supporting those functions.

|  PGN   |  Set  | Parse | Description of the PGN |
|--------|:-----:|:-----:|----------|
| 59392 |  \ref SetN2kPGN59392 "x"  |       | ISO Acknowledgment. See \ref descSystemMessages. |
| 59904 |  \ref SetN2kPGN59904 "x"  |   \ref ParseN2kPGN59904 "x"   | ISO Request. See \ref descSystemMessages. |
| 60928 |  \ref SetN2kPGN60928 "x"  |       | ISO Address Claim. See \ref descSystemMessages. |
| 126464 |  \ref SetN2kPGN126464 "x"  |       | Transmit/Receive PGN List. See \ref descSystemMessages. |
| 126992 |  \ref SetN2kPGN126992 "x"  |   \ref ParseN2kPGN126992 "x"   | System Time |
| 126993 |  \ref SetN2kPGN126993 "x"  |       | Heartbeat. See \ref descSystemMessages. |
| 126996 |  \ref SetN2kPGN126996 "x"  |   \ref ParseN2kPGN126996 "x"   | Product Information. See \ref descSystemMessages. |
| 126998 |  \ref SetN2kPGN126998 "x"  |   \ref ParseN2kPGN126998 "x"   | Configuration Information. See \ref descSystemMessages. |
| 127233 |  \ref SetN2kPGN127233 "x"  |  \ref ParseN2kPGN127233 "x"   | Man Overboard Notification(MOB) |
| 127237 |  \ref SetN2kPGN127237 "x"  |  \ref ParseN2kPGN127237 "x"   | Heading/Track Control |
| 127245 |  \ref SetN2kPGN127245 "x"  |  \ref ParseN2kPGN127245 "x"   | Rudder |
| 127250 |  \ref SetN2kPGN127250 "x"  |  \ref ParseN2kPGN127250 "x"   | Vessel Heading |
| 127251 |  \ref SetN2kPGN127251 "x"  |  \ref ParseN2kPGN127251 "x"   | Rate of Turn |
| 127252 |  \ref SetN2kPGN127252 "x"  |  \ref ParseN2kPGN127252 "x"   | Heave |
| 127257 |  \ref SetN2kPGN127257 "x"  |  \ref ParseN2kPGN127257 "x"   | Attitude |
| 127258 |  \ref SetN2kPGN127258 "x"  |  \ref ParseN2kPGN127258 "x"   | Magnetic Variation |
| 127488 |  \ref SetN2kPGN127488 "x"  |  \ref ParseN2kPGN127488 "x"   | Engine Parameters, Rapid Update |
| 127489 |  \ref SetN2kPGN127489 "x"  |  \ref ParseN2kPGN127489 "x"   | Engine Parameters, Dynamic |
| 127493 |  \ref SetN2kPGN127493 "x"  |  \ref ParseN2kPGN127493 "x"   | Transmission Parameters, Dynamic |
| 127497 |  \ref SetN2kPGN127497 "x"  |  \ref ParseN2kPGN127497 "x"   | Trip Fuel Consumption, Engine |
| 127501 |  \ref SetN2kPGN127501 "x"  |  \ref ParseN2kPGN127501 "x"   | Binary Status Report |
| 127505 |  \ref SetN2kPGN127505 "x"  |  \ref ParseN2kPGN127505 "x"   | Fluid Level |
| 127506 |  \ref SetN2kPGN127506 "x"  |  \ref ParseN2kPGN127506 "x"   | DC Detailed Status |
| 127507 |  \ref SetN2kPGN127507 "x"  |  \ref ParseN2kPGN127507 "x"   | Charger Status- DEPRECATED |
| 127508 |  \ref SetN2kPGN127508 "x"  |  \ref ParseN2kPGN127508 "x"   | Battery Status |
| 127513 |  \ref SetN2kPGN127513 "x"  |  \ref ParseN2kPGN127513 "x"   | Battery Configuration Status |
| 128000 |  \ref SetN2kPGN128000 "x"  |  \ref ParseN2kPGN128000 "x"   | Nautical Leeway Angle PGN: |
| 128259 |  \ref SetN2kPGN128259 "x"  |  \ref ParseN2kPGN128259 "x"   | Speed, Water Referenced |
| 128267 |  \ref SetN2kPGN128267 "x"  |  \ref ParseN2kPGN128267 "x"   | Water Depth |
| 128275 |  \ref SetN2kPGN128275 "x"  |  \ref ParseN2kPGN128275 "x"   | Distance Log |
| 128776 |  \ref SetN2kPGN128776 "x"  |  \ref ParseN2kPGN128776 "x"   | Windlass Control Status PGN: |
| 128777 |  \ref SetN2kPGN128777 "x"  |  \ref ParseN2kPGN128777 "x"   | Anchor Windlass Operating Status PGN: |
| 128778 |  \ref SetN2kPGN128778 "x"  |  \ref ParseN2kPGN128778 "x"   | Anchor Windlass Monitoring Status PGN: |
| 129025 |  \ref SetN2kPGN129025 "x"  |  \ref ParseN2kPGN129025 "x"   | Position, Rapid Update |
| 129026 |  \ref SetN2kPGN129026 "x"  |  \ref ParseN2kPGN129026 "x"   | COG & SOG, Rapid Update |
| 129029 |  \ref SetN2kPGN129029 "x"  |  \ref ParseN2kPGN129029 "x"   | GNSS Position Data |
| 129033 |  \ref SetN2kPGN129033 "x"  |  \ref ParseN2kPGN129033 "x"   | Local Time Offset |
| 129038 |  \ref SetN2kPGN129038 "x"  |  \ref ParseN2kPGN129038 "x"   | AIS Class A Position Report |
| 129039 |  \ref SetN2kPGN129039 "x"  |  \ref ParseN2kPGN129039 "x"   | AIS Class B Position Report |
| 129041 |  \ref SetN2kPGN129041 "x"  |  \ref ParseN2kPGN129041 "x"   | AIS Aids to Navigation (AtoN) Report |
| 129283 |  \ref SetN2kPGN129283 "x"  |  \ref ParseN2kPGN129283 "x"   | Cross Track Error |
| 129284 |  \ref SetN2kPGN129284 "x"  |  \ref ParseN2kPGN129284 "x"   | Navigation Data |
| 129285 |  x  |       | Navigation - Route/WP information |
| 129539 |  \ref SetN2kPGN129539 "x"  |  \ref ParseN2kPGN129539 "x"   | GNSS DOPs |
| 129540 |  \ref SetN2kPGN129540 "x"  |  \ref ParseN2kPGN129540 "x"   | GNSS Sats in View |
| 129794 |  \ref SetN2kPGN129794 "x"  |  \ref ParseN2kPGN129794 "x"   | AIS Class A Static and Voyage Related Data |
| 129802 |  \ref SetN2kPGN129802 "x"  |  \ref ParseN2kPGN129802 "x"   | AIS Safety Related Broadcast Message |
| 129809 |  \ref SetN2kPGN129809 "x"  |  \ref ParseN2kPGN129809 "x"   | AIS Class B "CS" Static Data Report, Part A |
| 129810 |  \ref SetN2kPGN129810 "x"  |  \ref ParseN2kPGN129810 "x"   | AIS Class B "CS" Static Data Report, Part B |
| 130074 |  x  |       | Route and WP Service - WP List - WP Name & Position |
| 130306 |  \ref SetN2kPGN130306 "x"  |  \ref ParseN2kPGN130306 "x"   | Wind Data |
| 130310 |  \ref SetN2kPGN130310 "x"  |  \ref ParseN2kPGN130310 "x"   | Environmental Parameters - DEPRECATED |
| 130311 |  \ref SetN2kPGN130311 "x"  |  \ref ParseN2kPGN130311 "x"   | Environmental Parameters- DEPRECATED |
| 130312 |  \ref SetN2kPGN130312 "x"  |  \ref ParseN2kPGN130312 "x"   | Temperature - DEPRECATED |
| 130313 |  \ref SetN2kPGN130313 "x"  |  \ref ParseN2kPGN130313 "x"   | Humidity |
| 130314 |  \ref SetN2kPGN130314 "x"  |  \ref ParseN2kPGN130314 "x"   | Actual Pressure |
| 130315 |  x  |       | Set Pressure |
| 130316 |  \ref SetN2kPGN130316 "x"  |  \ref ParseN2kPGN130316 "x"   | Temperature, Extended Range |
| 130323 |  \ref SetN2kPGN130323 "x"  |  \ref ParseN2kPGN130323 "x"   | Meteorological Station Data |
| 130576 |  \ref SetN2kPGN130576 "x"  |  \ref ParseN2kPGN130576 "x"   | Trim Tab Status |
| 130577 |  \ref SetN2kPGN130577 "x"  |  \ref ParseN2kPGN130577 "x"   | Direction Data |
| 130823 |  \ref SetN2kMaretronPGN130823 "x"  |  \ref ParseN2kMaretronPGN130823 "x"   | Maretron Temperature High Range |
