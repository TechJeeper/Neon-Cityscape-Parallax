module.exports = [
  {
    type: 'heading',
    defaultValue: 'Neon Cityscape'
  },
  {
    type: 'text',
    defaultValue:
      'Cyberpunk skyline with accelerometer parallax — tilt the watch to shift the layers.'
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Time'
      },
      {
        type: 'select',
        messageKey: 'TimeFormat',
        defaultValue: '0',
        label: 'Hour format',
        options: [
          { label: 'System default', value: '0' },
          { label: '12-hour', value: '1' },
          { label: '24-hour', value: '2' }
        ]
      },
      {
        type: 'toggle',
        messageKey: 'LeadingZero',
        label: 'Leading zero on hours',
        defaultValue: true
      },
      {
        type: 'toggle',
        messageKey: 'ShowDate',
        label: 'Show date',
        defaultValue: true
      },
      {
        type: 'toggle',
        messageKey: 'ShowSeconds',
        label: 'Show seconds',
        defaultValue: false
      }
    ]
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Parallax'
      },
      {
        type: 'toggle',
        messageKey: 'ParallaxEnabled',
        label: 'Enable parallax',
        description: 'Uses the accelerometer to shift sky, skyline, and rooftops.',
        defaultValue: true
      },
      {
        type: 'select',
        messageKey: 'ParallaxIntensity',
        defaultValue: '1',
        label: 'Intensity',
        options: [
          { label: 'Low', value: '0' },
          { label: 'Medium', value: '1' },
          { label: 'High', value: '2' }
        ]
      }
    ]
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Look'
      },
      {
        type: 'select',
        messageKey: 'NeonTheme',
        defaultValue: '1',
        label: 'Neon accent (color watches)',
        options: [
          { label: 'Magenta', value: '0' },
          { label: 'Cyan', value: '1' },
          { label: 'Amber', value: '2' }
        ]
      },
      {
        type: 'toggle',
        messageKey: 'InvertBw',
        label: 'Invert on black & white',
        description: 'For Pebble 2 / Time 2 Duo style displays.',
        defaultValue: false
      }
    ]
  },

  {
    type: 'submit',
    defaultValue: 'Save'
  }
];
