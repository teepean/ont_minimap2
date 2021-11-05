Starting from minimap2 2.19 the only way to compile minimap2 is with clang-cl toolset.

Example cmake invocation:

cmake -DZLIB_LIBRARY=<PATH_TO_ZLIB_LIBRARY> -DZLIB_INCLUDE_DIR=<PATH_TO_ZLIB_INCLUDE> -T ClangCL ..