# List of supported NMEA2000 PGNs {#listMsg} 

This list gives a short overview of all supported NMEA2000 PGNs. The most of these functions are provided in ef N2kMessages.h . 

|  PGN   |  Set  | Parse |: Description of the PGN |
|--------|:-----:|:-----:|----------|
| 59392 |  x  |       | ISO Acknowledgment |
| 59904 |  x  |   x   | ISO Request |
| 60928 |  x  |       | ISO Address Claim |
| 126464 |  x  |       | PGN List -  Received PGNs group function |
| 126992 |  x  |   x   | System Time |
| 126993 |  x  |       | Heartbeat |
| 126996 |  x  |   x   | Product Information |
| 126998 |  x  |   x   | Configuration Information |
| 127233 |  x  |   x   | Man Overboard Notification(MOB) |
| 127237 |  x  |   x   | Heading/Track Control |
| 127245 |  x  |   x   | Rudder |
| 127250 |  x  |   x   | Vessel Heading |
| 127251 |  x  |   x   | Rate of Turn |
| 127252 |  x  |   x   | Heave |
| 127257 |  x  |   x   | Attitude |
| 127258 |  x  |   x   | Magnetic Variation |
| 127488 |  x  |   x   | Engine Parameters, Rapid Update |
| 127489 |  x  |   x   | Engine Parameters, Dynamic |
| 127493 |  x  |   x   | Transmission Parameters, Dynamic |
| 127497 |  x  |   x   | Trip Fuel Consumption, Engine |
| 127501 |  x  |   x   | Binary Status Report |
| 127505 |  x  |   x   | Fluid Level |
| 127506 |  x  |   x   | DC Detailed Status |
| 127507 |  x  |   x   | Charger Status- DEPRECATED |
| 127508 |  x  |   x   | Battery Status |
| 127513 |  x  |   x   | Battery Configuration Status |
| 128000 |  x  |   x   | Nautical Leeway Angle PGN: |
| 128259 |  x  |   x   | Speed, Water Referenced |
| 128267 |  x  |   x   | Water Depth |
| 128275 |  x  |   x   | Distance Log |
| 128776 |  x  |   x   | Windlass Control Status PGN: |
| 128777 |  x  |   x   | Anchor Windlass Operating Status PGN: |
| 128778 |  x  |   x   | Anchor Windlass Monitoring Status PGN: |
| 129025 |  x  |   x   | Position, Rapid Update |
| 129026 |  x  |   x   | COG & SOG, Rapid Update |
| 129029 |  x  |   x   | GNSS Position Data |
| 129033 |  x  |   x   | Local Time Offset |
| 129038 |  x  |   x   | AIS Class A Position Report |
| 129039 |  x  |   x   | AIS Class B Position Report |
| 129041 |  x  |   x   | AIS Aids to Navigation (AtoN) Report |
| 129283 |  x  |   x   | Cross Track Error |
| 129284 |  x  |   x   | Navigation Data |
| 129285 |  x  |       | Navigation - Route/WP information |
| 129539 |  x  |   x   | GNSS DOPs |
| 129540 |  x  |   x   | GNSS Sats in View |
| 129794 |  x  |   x   | AIS Class A Static and Voyage Related Data |
| 129802 |  x  |   x   | AIS Safety Related Broadcast Message |
| 129809 |  x  |   x   | AIS Class B "CS" Static Data Report, Part A |
| 129810 |  x  |   x   | AIS Class B "CS" Static Data Report, Part B |
| 130074 |  x  |       | Route and WP Service - WP List - WP Name & Position |
| 130306 |  x  |   x   | Wind Data |
| 130310 |  x  |   x   | Environmental Parameters - DEPRECATED |
| 130311 |  x  |   x   | Environmental Parameters- DEPRECATED |
| 130312 |  x  |   x   | Temperature - DEPRECATED |
| 130313 |  x  |   x   | Humidity |
| 130314 |  x  |   x   | Actual Pressure |
| 130315 |  x  |       | Set Pressure |
| 130316 |  x  |   x   | Temperature, Extended Range |
| 130323 |  x  |   x   | Meteorological Station Data |
| 130576 |  x  |   x   | Trim Tab Status |
| 130577 |  x  |   x   | Direction Data |
| 130823 |  x  |       | - |
