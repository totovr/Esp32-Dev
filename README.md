# esp32

esp32 programs for different applications

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

* [VS Code](https://code.visualstudio.com/)
* esp32 Dev Board
* A computer with OSX, Windows, Linux

### Installing

#### For Windows users

* Install the [driver](https://github.com/totovr/Esp32-Dev/blob/master/CP210x_Universal_Windows_Driver.zip)
* Install the last stable version of PlataformIO, you can download the package with the manager package of VS Code
* Open PlataformIO
* Create a new project or migrate one of Arduino
* Select esp32 Dev board

#### For Mac users

* Install the last stable version of PlataformIO, you can download the package with the manager package of VS Code
* Open PlataformIO
* Create a new project or migrate one of Arduino
* Select esp32 Dev board

#### For linux users please follow the next steps 

* Install latest Arduino IDE from [arduino.cc](https://www.arduino.cc/en/Main/Software)
* Open Terminal and execute the following command (copy->paste and hit enter):

  ```bash
  sudo usermod -a -G dialout $USER && \
  sudo apt-get install git && \
  wget https://bootstrap.pypa.io/get-pip.py && \
  sudo python get-pip.py && \
  sudo pip install pyserial && \
  mkdir -p ~/Arduino/hardware/espressif && \
  cd ~/Arduino/hardware/espressif && \
  git clone https://github.com/espressif/arduino-esp32.git esp32 && \
  cd esp32 && \
  git submodule update --init --recursive && \
  cd tools && \
  python3 get.py
  ```
* Restart PC, sometimes the driver is not detected until the PC is restarted
* Install the last stable version of PlataformIO, you can download the package with the manager package of VS Code
* Open PlataformIO
* Create a new project or migrate one of Arduino
* Select esp32 Dev board

## Contributing

Please read [CONTRIBUTING.md](https://github.com/totovr/Processing/blob/master/CONTRIBUTING.md) for details of the code of conduct, and the process for submitting pull requests to us.

## Versioning

I use [SemVer](http://semver.org/) for versioning.

## Authors

Antonio Vega Ramirez:

* [Github](https://github.com/totovr)
* [Twitter](https://twitter.com/SpainDice)

## License

This project is licensed under The MIT License (MIT) - see the [LICENSE.md](https://github.com/totovr/Arduino/blob/master/LICENSE.md) file for details
