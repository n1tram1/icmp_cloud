project('cloud', 'c',
  default_options: [
    'warnlevel=2',
    'debug=true',
    'c_std=c99',
    'b_sanitize=address',
])


inc = include_directories('src/')

utils_sources = [
  'src/utils/dns.c',
  'src/utils/ping.c',
  'src/utils/list.c',
  'src/utils/pinger.c',
]

liburing_dep = dependency('liburing', required : true)
utils_lib = static_library('utils',
                           utils_sources,
                           dependencies : [liburing_dep])

client_sources = [
  'src/client/main.c',
  'src/client/chunk.c',
  'src/client/juggler.c',
  'src/client/host.c',
]

client = executable('client',
                    client_sources,
                    include_directories : inc,
                    link_with : utils_lib)

daemon_sources = [
  'src/daemon/main.c',
]

daemon = executable('daemon',
                    daemon_sources,
                    include_directories : inc,
                    link_with : utils_lib)

