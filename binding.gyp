{
  "targets": [
    {
      "target_name": "isr",
      "sources": [ "isr.cc", "button.cc", "onoff.cc", "quadratureLowRes.cc", "quadratureHighRes.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        # '<!(pwd)/wiringPi/wiringPi',
        # '<!(pwd)/wiringPi/devLib'
      ],
      'libraries': [
        # '<!(pwd)/wiringPi/wiringPi/libwiringPi.a',
        # '<!(pwd)/wiringPi/devLib/libwiringPiDev.a'
      ]
    }

  ]
}

