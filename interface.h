#ifndef INTERFACE_H
#define INTERFACE_H

#include "domain.h"
#include "registerblock.h"

enum endianness_t{
    ENDIAN_LITTLE,
    ENDIAN_BIG,

    num_endiannesses
};

class Interface
{
public:
    Interface();

    Domain* domain;
    RegisterBlock* reg_block;


    //The domain:block alignment ratio is a general way of describing the
    //relationship between an address access from a given interface and the
    //addresses accessed within the register block.

    //1:1 means that if i access address 0x67, i'll get all the data from
    //register at offset 0x67.

    //2:1 means that if i access address 0x67, i'll get half of the
    //register data from the register at offset 0x33 (floor(0x67/2)).
    //I would be able to access the other half of the register at 0x33 by
    //accessing address 0x66 from the interface.
    //Another way of putting this is that it takes two sequential domain
    //accesses to access one entire register from the block.

    //Note that which half is accessed by which address is not specified.
    //That is because this configuration requires an endianness.
    //Little endian configuation would mean that address 0x66 accesses the
    //least significant half of register 0x33, and address 0x67 accesses the
    //most significant half.
    //For example, if the register at 0x33 has the data "0xDEADBEEF" (32 bits),
    //a 16-bit read at address 0x66 will return "0xBEEF".
    //Big endian configuation would mean that address 0x66 accesses the most
    //significant half of register 0x33, and address 0x67 accesses the least
    //significant half.
    //For example, if the register at 0x33 has the data "0xDEADBEEF" (32 bits),
    //a 16-bit read at address 0x66 will return "0xDEAD".

    //1:2 means that if i access address 0x67, i'll actually access two
    //registers at offsets 0xCE and 0xCF, respectively.
    //I would access the register at offset 0x67 by using address 0x33 on the
    //interface, which would give me the registers at offsets 0x66 and 0x67.
    //Another way of putting this is that one access on the domain actually
    //accesses two registers in the block.

    //The order that those registers appear on the data bus depends on the
    //endianness of the interface.
    //A Big endian configuation means that the lower register offsets will
    //appear in more significant places in the interface bus.
    //Another way of putting this is that the registers' data will appear in
    //offset order, from left to right, in the interface bus word.
    //Lets say register 0x66 has "0x4A", and register 0x67 has "0x90".
    //A read on address 0x33 would return "0x4A90"
    //A Little endian configuation means that the lower register offsets will
    //appear in less significant places in the interface bus.
    //Another way of putting this is that the registers' data will appear in
    //reverse offset order in the interface bus word.
    //Lets say register 0x66 has "0x4A", and register 0x67 has "0x90".
    //A read on address 0x33 would return "0x904A"
    uint8_t align_ratio_domain;
    uint8_t align_ratio_rb;

    //The number of bits in the least significant byte that should always be 0.
    //This usually is mandated because an interface only respects word (read:
    //register or data bus width) aligned accesses, and the full address
    //(including these zeroed out bits) is a byte address.
    //Depending on the design, the end user/technician that has to care about
    //what address means exactly what thing into the interface might be shown
    //the address with or without the zeroed out LSBs.

    //This program will assume that these unused LSBs are perscribed by the
    //master controlling the bus--that, for example, if 2 LSBs are unused, it's
    //because the master will never use those bits coming from its address
    //output, not that it wants the register block to pretend there's two 0s
    //on the end of the address it was given and then ignore them.

    //A common scenario is for a register block to have 32-bit registers, and
    //one of the 32-bit-wide interfaces to that block has 2 unused LSBs.
    //So for example, an access at address 0x2C accesses the full register at
    //offset 0x0B (0x2C >> 2)

    //In that example, normally the interface would have a 4:1 relationship
    //with the block. But letting ths program know of these unused bits allows
    //the implementation to be much simpler--it can assume a 1:1 relationship
    //with a simply shifted address.
    uint8_t unused_lsbs;

    endianness_t endianness;
};

#endif // INTERFACE_H
