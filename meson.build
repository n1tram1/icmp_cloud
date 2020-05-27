project('cloud', 'c',
  default_options: [
      'warnlevel=2',
      'debug=true',
      'c_std=c99',
      'b_sanitize=address',
  ])

cloud_sources = ['src/main.c',
                 'src/dns.c',
                 'src/ping.c',
                 'src/chunk.c',
                ]
cloud = executable('cloud',
                   cloud_sources)

