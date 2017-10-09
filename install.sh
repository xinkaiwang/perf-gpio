#!/bin/bash

check_make_ok() {
  if [ $? != 0 ]; then
    echo "failed."
    echo ""
    echo "================================================================================"
    if [ $2 == 1 ]; then
      echo "FATAL: Making $1 failed."
    else
      echo "Making $1 failed."
    fi
    echo "Please check install.log and fix any problems. If you're still stuck,"
    echo "then please open a new issue then post all the output and as many details as you can to"
    echo "  https://github.com/WiringPi/WiringPi-Node/issues"
    echo "================================================================================"
    echo ""
    if [ $2 == 1 ]; then
      exit 1
    fi
  fi
}

check_git_clone() {
  if [ $? != 0 ]; then
    echo "failed."
    echo ""
    echo "================================================================================"
    echo "FATAL: Cloning libWiringPi failed."
    echo "Please check install.log and fix any problems. If you're still stuck,"
    echo "then please open a new issue then post all the output and as many details as you can to"
    echo "  https://github.com/WiringPi/WiringPi-Node/issues"
    echo "================================================================================"
    echo ""
    exit 1
  fi
}

rm ./install.log 2>/dev/null 1>&2

echo -n "Cloning libWiringPi ... "
rm -Rf ./wiringPi 2>/dev/null 1>&2
git clone git://git.drogon.net/wiringPi > ./install.log 2>&1
check_git_clone
echo "done."

echo -n "Making libWiringPi ... "
cd ./wiringPi/wiringPi/
make clean >> ../../install.log 2>&1
make static >> ../../install.log 2>&1
check_make_ok "libWiringPi" 1
cd ../../
echo "done."

cd ./wiringPi/devLib/
echo -n "Making devLib ..."
make clean >> ../../install.log 2>&1
make static >> ../../install.log 2>&1
check_make_ok "devLib" 0
cd ../../
echo "done."

cd ./wiringPi/gpio/
echo -n "Unistalling gpio utility ... "
sudo make uninstall >> ../../install.log 2>&1
echo "done."

echo -n "Making gpio utility ... "
make clean >> ../../install.log 2>&1
make >> ../../install.log 2>&1
check_make_ok "gpio utility" 0
echo "done."

echo -n "Installing gpio utility ... "
sudo make install >> ../../install.log 2>&1
check_make_ok "gpio utility" 0
cd ../../
echo "done."

echo -n "Making perf-gpio ... "
node-gyp rebuild 2>&1 | tee -a ./install.log
check_make_ok "perf-gpio" 1
echo "done."

echo "Enjoy !"
