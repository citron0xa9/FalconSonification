# FalconSonification
Application to sonify falcon 9 rocket launches

# Mapping Expressions
 - uses exprtk expressions (https://github.com/ArashPartow/exprtk)
 - constants like pi are available

## Available simulation variables
 - `SimulationTimeTotal` Total time simulation is running in seconds (0s equals T-0)
 - `FlightTimeTotal` Total time rocket is actually "flying" in milliseconds
 - `MaxDynamicPressure` in Pascal
 - `MinAltitude` in Kilometers
 - `MaxAltitude` in Kilometers
 - `MaxSpeed` in km/h
 - `MaxAcceleration` in m/s²
 - `MinFuel` in Kg
 - `MaxFuel` in Kg
 - `MaxThrust` in KN
 - `MaxPropellantFlow` in Tons per Second
 - `MaxDownrage` in Kilometers
 - `MaxDragForce` in Newton
 - `MaxPitch` in Degrees
 - `MinPitch` in Degrees
 - `MaxYaw` in Degrees
 - `MinYaw` in Degrees
 - `MaxRoll` in Degrees
 - `MinRoll` in Degrees
 - `MaxAngularAcceleration` in MRad per second² 
 - `MaxAbsoluteNozzleAngle` in Degrees
 - `MaxMachNumber`

-------

 - `SimulationTime` current time simulation is running in seconds (0s equals T-0)
 - `FlightTime` current time rocket is actually "flying" in milliseconds
 - `DynamicPressure` in Pascal
 - `Altitude` in Kilometers
 - `Speed` in km/h
 - `Acceleration` in m/s²
 - `Fuel` in kg
 - `Throttle` in percent
 - `Thrust` in KN
 - `PropellantFlow` in Tons per Second
 - `Downrage` in Kilometers
 - `DragForce` in Newton
 - `Pitch` in Degrees
 - `Yaw` in Degrees
 - `Roll` in Degrees
 - `AngularAcceleration` in MRad per second² 
 - `AbsoluteNozzleAngle` in Degrees
 - `MachNumber`

# Simulation Data
 - CSV based data is required. Currently only output of F9Sim (https://www.mcrenox.com.ar/f9sim/) is supported.
 - example data for Falcon9 CRS-17 mission: [falcon9-crs17-log.zip](https://github.com/citron0xa9/FalconSonification/files/6223805/falcon9-crs17-log.zip)

# Running
 - working directory has to contain the `samples` folder which contain sampled sound files.

# Building
 - Requires C++-17
 - Tested with Visual Studio 2017
