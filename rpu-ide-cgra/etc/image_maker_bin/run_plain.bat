@echo  off


maker.exe -in  ./firmware_625_shell_canrun.bin -out fw_plain.fw  -type plain -run 0x00080080 -load 0x00080000 

pause
