# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.0] - 2021-02-10

### Changed

* Updated Sensirion Core library version from 0.4.0 to 0.4.2. This includes the
  renaming of the library header file from `SensirionCoreArduinoLibrary.h` to
  `SensirionCore.h`.
* Define `SCD4X_I2C_ADDRESS` as hex instead of dec (unchanged value).

### Added

* Added warning about limited EEPROM write cycles.

## [0.1.0] - 2021-02-05

Initial release

[0.2.0]: https://github.com/Sensirion/arduino-i2c-scd4x/compare/0.1.0...0.2.0
[0.1.0]: https://github.com/Sensirion/arduino-i2c-scd4x/releases/tag/0.1.0
