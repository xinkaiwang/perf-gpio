{
  "targets": [
    {
      "target_name": "isr",
      "sources": [ "isr.cc", "button.cc", "inputBank.cc", "clockOut.cc", "onoff.cc", "quadratureLowRes.cc", "quadratureHighRes.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        '<!(pwd)/WiringPi/wiringPi',
        '<!(pwd)/WiringPi/devLib'
      ],
      'libraries': [
        '<!(pwd)/WiringPi/wiringPi/libwiringPi.a',
        '<!(pwd)/WiringPi/devLib/libwiringPiDev.a'
      ]
    }

  ]
}

