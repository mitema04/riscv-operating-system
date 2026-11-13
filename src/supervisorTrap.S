.extern handleSupervisorTrap
.align 4
.global supervisorTrap
supervisorTrap:
    addi sp, sp, -256

    .irp num, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\num, \num * 8(sp)
    .endr

    // adresa gde pocinju registri se sada nalazi u a0 i prosledjuje se handleru
    mv a0, sp

    call handleSupervisorTrap

    .irp num, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\num, \num * 8(sp)
    .endr

    addi sp, sp, 256

    sret

