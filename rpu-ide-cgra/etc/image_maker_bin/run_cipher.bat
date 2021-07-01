@echo off


maker.exe -in  ./firmware_625_shell_canrun.bin   -out fw_cipherd.fw   -sign-key ca/sign_private.der  -encrypt-key ca/aes_public.der          -type en_sig_aes_dynamic                -run 0x00080080           -load 0x00080000 
pause
