call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
mkdir buildvs
cd buildvs
cmake -G "Visual Studio 16 2019" -A x64 -Thost=x64 -DLLVM_ENABLE_RPMALLOC=true -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="lld" -DCMAKE_BUILD_TYPE=Release -D-DLLVM_USE_CRT_RELEASE=MT -DLLVM_OPTIMIZED_TABLEGEN=true -DLLVM_ENABLE_PDB=true ../llvm
cd ..
mkdir buildninjafast
cd buildninjafast
cmake -G Ninja -DLLVM_ENABLE_RPMALLOC=true -DCMAKE_EXE_LINKER_FLAGS="-time" -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="lld" -DCMAKE_BUILD_TYPE=Release -D-DLLVM_USE_CRT_RELEASE=MT -DLLVM_OPTIMIZED_TABLEGEN=true -DLLVM_ENABLE_PDB=true -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_LINKER="C:/Program Files/LLVM/bin/lld-link.exe" ../llvm
cd ..
mkdir buildninjadebug
cd buildninjadebug
cmake -G Ninja -DLLVM_ENABLE_RPMALLOC=true -DCMAKE_EXE_LINKER_FLAGS="-time" -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="lld" -DCMAKE_BUILD_TYPE=Debug -D-DLLVM_USE_CRT_DEBUG=MT -DLLVM_OPTIMIZED_TABLEGEN=true -DLLVM_ENABLE_PDB=true -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_LINKER="C:/Program Files/LLVM/bin/lld-link.exe" ../llvm
cd ..
mkdir buildninjalto
cd buildninjalto
cmake -G Ninja -DLLVM_ENABLE_RPMALLOC=true -DLLVM_BUILD_INSTRUMENTED=IR -DLLVM_BUILD_RUNTIME=No -DCMAKE_EXE_LINKER_FLAGS="-time" -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="lld" -DCMAKE_BUILD_TYPE=Release -DLLVM_USE_CRT_RELEASE=MT -DLLVM_OPTIMIZED_TABLEGEN=true -DLLVM_ENABLE_PDB=true -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_LINKER="C:/Program Files/LLVM/bin/lld-link.exe" ../llvm
cd ..
