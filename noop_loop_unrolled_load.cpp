StartTick = ReadCPUCounter();
__asm__ volatile("\                 
                 nop
                 nop
                 nop
                 nop
                 nop
                 nop
                 nop
                 nop"
                :
                :
                );
EndTick = ReadCPUCounter();                
