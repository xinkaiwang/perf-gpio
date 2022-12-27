{
  "targets": [
    {
      "target_name": "isr",
      "sources": [ "isr.cc", "inputBank.cc", "button.cc", "clockOut.cc", "onoff.cc", "quadratureLowRes.cc", "quadratureHighRes.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      'libraries': [
        '-lwiringPi'
      ]
    }

  ]
}

