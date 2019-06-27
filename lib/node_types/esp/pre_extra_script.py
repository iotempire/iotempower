Import("env")

# access to global construction environment
#print env

# Dump construction environment (for debug purpose)
#print env.Dump()

import os

# now in environment variable PLATFORMIO_BUILD_CACHE_DIR
# env.CacheDir(os.environ['IOTEMPOWER_COMPILE_CACHE']+'/scons')

# # Dump construction environment (for debug purpose)
# print "=========== env dump ========"
# print env.Dump()
# print "=========== env dump end ===="
