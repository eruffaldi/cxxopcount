#<arch><sub>-<vendor>-<sys>-<abi>
clang -O3 -S -emit-llvm  opcount2_orig.cpp
mv opcount2_orig.ll opcount2_orig.x86.ll
clang -O3 -S  opcount2_orig.cpp
mv opcount2_orig.s opcount2_orig.x86.s
clang -target arm-none-eabi  -DBARE -mcpu=cortex-m4  -mfloat-abi=hard  -mthumb -O3 -S -emit-llvm  opcount2_orig.cpp
mv opcount2_orig.ll opcount2_orig.stm32.ll
clang -target arm-none-eabi  -DBARE -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=vfp -mthumb -O3 -S  opcount2_orig.cpp
mv opcount2_orig.s opcount2_orig.stm32.s

clang -target arm-none-eabi  -DBARE -mcpu=cortex-m4  -mfloat-abi=hard  -mfpu=vfp -mthumb -O3 -S  opcount_asinf.c
mv opcount_asinf.c opcount_asinf.stm32.c
