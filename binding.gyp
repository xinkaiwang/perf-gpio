{
  "targets": [
    {
      "target_name": "isr",
      "sources": [ "isr.cc", "button.cc", "onoff.cc", "quadratureDecoder.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        'wiringPi/wiringPi',
        'wiringPi/devLib'
      ],
      'libraries': [
        '<!(pwd)/wiringPi/wiringPi/libwiringPi.a',
        '<!(pwd)/wiringPi/devLib/libwiringPiDev.a'
      ]
    }

  ]
}

