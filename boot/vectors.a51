$NOMOD51
; Boot reset at 0; IRQ trampolines to App INTVECTOR(0x1000).
; Relocatable CODE starts at 0x0060 so it does not fill vector gaps.

                EXTRN   CODE (main)
                PUBLIC  ?C_STARTUP
                PUBLIC  boot_entry

?STACK          SEGMENT   IDATA
                RSEG    ?STACK
                DS      1

                CSEG    AT      0
?C_STARTUP:     LJMP    boot_entry

                CSEG    AT      0003H
                LJMP    1003H
                CSEG    AT      000BH
                LJMP    100BH
                CSEG    AT      0013H
                LJMP    1013H
                CSEG    AT      001BH
                LJMP    101BH
                CSEG    AT      0023H
                LJMP    1023H
                CSEG    AT      002BH
                LJMP    102BH
                CSEG    AT      0033H
                LJMP    1033H
                CSEG    AT      003BH
                LJMP    103BH
                CSEG    AT      0043H
                LJMP    1043H
                CSEG    AT      004BH
                LJMP    104BH
                CSEG    AT      0053H
                LJMP    1053H
                CSEG    AT      005BH
                LJMP    105BH

?BOOT_ENTRY     SEGMENT CODE
                RSEG    ?BOOT_ENTRY
boot_entry:     MOV     SP,#?STACK-1
                LJMP    main

                END
