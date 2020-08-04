# Cryologger - Glacier Velocity Measurement System
A low-cost, open-source glacier velocity measurement system based on the u-blox ZED-F9P intended for use with Precise Point Positioning (PPP) techniques.

## Background

## Objective
* Inexpensive
* Power efficiency

## Design

### Components:
* SparkFun GPS-RTK-SMA ZED-F9P 
* SparkFun OpenLog Artemis
* u-blox ANN-MB antenna

## Measurements
The firmware runs on the SparkFun OpenLog Artemis and is dedicated to logging UBX messages from the u-blox ZED-F9P.

Messages are streamed directly to a microSD card in UBX format without being processed. 

Log files can then be analysed afterwards using u-blox's u-center or RTKLIB and converted to RINEX in preparation for submission to NRCan's Precise Point Positioning (PPP) tool: https://webapp.geod.nrcan.gc.ca/geod/tools-outils/ppp.php

## Repository Contents
* **/Software** - Contains the Arduino code and firmware binaries

* **/Hardware** - Autodesk Eagle PCB schematic and board design files

* **/Bill of Materials** - Information on components and their associated costs

* **/Archive** - Previous design iterations

## Documentation
* Currently in progress

## License Information
This project is released under the MIT License (http://opensource.org/licenses/MIT).

Cheers,

**Adam**
