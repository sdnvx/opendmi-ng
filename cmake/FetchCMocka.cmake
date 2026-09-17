include(FetchContent)

# Release tarball is used instead of the Git repository: it does not depend on
# availability of the Git server, and its integrity is verified by the hash
FetchContent_Declare(
  cmocka
  URL      https://cmocka.org/files/2.0/cmocka-2.0.2.tar.xz
  URL_HASH SHA256=39f92f366bdf3f1a02af4da75b4a5c52df6c9f7e736c7d65de13283f9f0ef416
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(WITH_STATIC_LIB ON CACHE BOOL "CMocka: Build with a static library" FORCE)
set(WITH_CMOCKERY_SUPPORT OFF CACHE BOOL "CMocka: Install a cmockery header" FORCE)
set(WITH_EXAMPLES OFF CACHE BOOL "CMocka: Build examples" FORCE)
set(UNIT_TESTING ON CACHE BOOL "CMocka: Build with unit testing" FORCE)
set(PICKY_DEVELOPER OFF CACHE BOOL "CMocka: Build with picky developer flags" FORCE)

FetchContent_MakeAvailable(cmocka)
