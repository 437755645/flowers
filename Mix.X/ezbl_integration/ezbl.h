/*******************************************************************************
  Easy Bootloader for PIC24 and dsPIC33 API Definitions

  File Name:
    ezbl.h

  Summary:
    EZBL and Bootloader definitions for Bootloader and Application API call use.

  Description:
    Bootloader definitions for the EZBL Application callable API. This file
    is intended to be #included into any application files which wish to
    communicate with or call into the EZBL library or an EZBL Bootloader at run 
    time.
  
    Note that many of these functions may not exist in your bootloader (were 
    stripped out at bootloader linking time to save code space via the "Remove 
    unused sections" linker option). If you get a linker error specifying that a 
    given EZBL symbol defined in this header cannot be found, this most likely 
    means you need to add the the bootloader's output merge.s file to your 
    application Source Files, add ezbl_lib16.a or ezbl_lib16ep.a Libraries 
    files, or add now.c or other appropriate source file to your application 
    project in order to get a copy of the given API you are trying to use. 
 
    For some APIs, the linker will give a similar undefined symbol error message 
    for a symbol that isn't declared in this header. These may be for symbols 
    defined a special .gld linker script that you are missing or not using. 
    Ensure you have your bootloader's output merge.gld linker script included in 
    your application and placed properly in the "Linker Files" MPLAB X Projects 
    category folder. If a symbol is still missing you may be trying to use an 
    API that isn't supported in your bootloader type, requiring you to stop 
    attempting to call it.
 ******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
  Copyright (C) 2015 Microchip Technology Inc.

  MICROCHIP SOFTWARE NOTICE AND DISCLAIMER:  You may use this software, and any
  derivatives created by any person or entity by or on your behalf, exclusively
  with Microchip's products.  Microchip and its licensors retain all ownership
  and intellectual property rights in the accompanying software and in all
  derivatives here to.

  This software and any accompanying information is for suggestion only.  It
  does not modify Microchip's standard warranty for its products.  You agree
  that you are solely responsible for testing the software and determining its
  suitability.  Microchip has no obligation to modify, test, certify, or
  support the software.

  THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
  EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
  PURPOSE APPLY TO THIS SOFTWARE, ITS INTERACTION WITH MICROCHIP'S PRODUCTS,
  COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

  IN NO EVENT, WILL MICROCHIP BE LIABLE, WHETHER IN CONTRACT, WARRANTY, TORT
  (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), STRICT LIABILITY,
  INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, PUNITIVE,
  EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF
  ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWSOEVER CAUSED, EVEN IF
  MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.
  TO THE FULLEST EXTENT ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
  CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF
  FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

  MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
  TERMS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef EZBL_H
#define	EZBL_H

// Easy Bootloader for PIC24 and dsPIC33 library release version
#define EZBL_VERSION            1.01        // The complete EZBL version string, but written as a floating point number, including two decimals.
#define EZBL_VERSION_MAJOR      1           // The single EZBL major revision numeral to the left of the dot
#define EZBL_VERSION_MINOR      1           // The numerical EZBL minor revision to the right of the dot. This is 1 for version x.01, 10 for x.10, 26 for x.26, etc.
#define EZBL_VERSION_MINORH     0           // The single EZBL minor revision numeral one place right of the dot (immediate right of the dot)
#define EZBL_VERSION_MINORL     1           // The single EZBL minor revision numeral two places right of the dot

/**
 * #define STRINGIFY(value)
 *
 * Macros for converting a macro value into a string representation of the
 * macro value. This is needed for concatenating macro contents to other strings.
 * Using STRINGIFY_INNER() directly does not allow macro expansion.
 * Using STRINGIFY() does perform macro expansion of value before passing
 * to the inner version.
 * As an example, consider this code:
 *  #define APP_VERSION_MAJOR    3
 *  #define APP_VERSION_MINOR    14
 *      UARTPrintROMString("Firmware version: " STRINGIFY(APP_VERSION_MAJOR) "." STRINGIFY(APP_VERSION_MINOR) "\n");
 * Here, the version macros are integer compile-time constants. With
 * stringification, the UARTPrintROMString() function doesn't need any logic to
 * do binary to ASCII number conversion like the heavyweight printf() function.
 *
 * @param value The token(s) to convert to a string literal.
 *
 * @return The 'value' token(s), after any macro expansion, converted to a
 *         string literal.
 */
#if !defined(STRINGIFY)
#define STRINGIFY_INNER(x)              #x
#define STRINGIFY(value)                STRINGIFY_INNER(value)
#endif


// Structure representing the first 28 bytes of a .blob file (just before the 
// first record header starts).
typedef struct
{
    unsigned long length;       // Total number of bytes in the .blob file, including this 28-byte header. Little-endian.
    unsigned long signature;    // Unused 4-byte field. Typically set to ASCII 'BLOB' (0x424F4C42).
    unsigned char hash[20];     // SHA-1 hash of the entire .blob file, in unencrypted state. Includes this header (but with this 20-byte hash field zeroed) before computation.
} EZBL_BLOB_HEADER;             // 28-byte header affixed to all legal .blob files. This header is not encrypted.

// Structure for storing AES128 round keys needed for encryption and decryption
typedef struct
{
    unsigned long key_length;
    unsigned long data[44];
} AES_ROUND_KEYS_128_BIT;       


// Structure for storing a complete AES128 context for doing Counter Mode encryption and decryption
typedef struct
{
    AES_ROUND_KEYS_128_BIT aes128RoundKeys;
    unsigned long counter[4];
    unsigned long offset;
    unsigned long keyStreamBuffer[4];
} EZBL_CRYPT_STATE;




// Management and memory movement functions provided in ezbl_fast_util.S
void EZBL_RAMCopy(void *dest, void *source, unsigned int length);                // Copies unaligned RAM bytes from *source to *dest
void EZBL_RAMSet(void *dest, int value, unsigned int length);                    // Writes value (actually a single char) into *dest memory a total of length times
unsigned int EZBL_RAMCompare(void *source1, void *source2, unsigned int length); // Compares length bytes from *source1 with *source2. On exact match 0 is returned. Any difference returns 0x0001


// Math functions provided in ezbl_fast_util.S
unsigned long EZBL_SYM32(unsigned long symbol);                                  // Converts a *(__pack_upper_byte void) type symbol pointer into an ordinary 32-bit unsigned long symbol value or address


// Other Flash, Interrupt, and program flow functions provided in ezbl_fast_util.S
unsigned long EZBL_WriteROMRow(unsigned long destFlashAddress, void *srcData, unsigned int dataByteCount);       // Reserved - do not call. Writes to full Flash program rows, but only supports a very limited set of devices right now; Must set EZBL_NVMKey = 0x03DF before calling.
unsigned long EZBL_WriteROMRowChecked(unsigned long destFlashAddress, void *srcData, unsigned int dataByteCount);// Reserved - do not call. Writes to full Flash program rows after masking off any data which is occupied by the bootloader; Must set EZBL_NVMKey = 0x03DF before calling. *srcData contents can change due to the bootloader address-overlay data masking feature.
unsigned long EZBL_WriteROMOnlyOnce(unsigned long destFlashAddress, void *srcData, unsigned int dataByteCount);  // Writes Flash program words to an aligned Flash word address (like EZBL_WriteROM() and EZBL_WriteROMChecked() functions, but only after masking off any data which shouldn't or can't be written because the Flash already contains data in the given Flash addresses; Must set EZBL_NVMKey = 0x03DF before calling. *srcData contents can change due to the bootloader address-overlay data masking feature.
void EZBL_WaitForNVMOpComplete(void);                            // Polls the NVMCON<WR> bit and waits for it to clear. Then NVMCON<WREN> is cleared before returning.
unsigned int EZBL_DisableInterrupts(void);                       // Disables all interrupts, including IPL 7 ones, by setting the CPU IPL to 7. The return value is encoded IPL state information which should be saved and passed to EZBL_RestoreInterrupts() if you wish to later turn the same interrupts disabled back on.)
void EZBL_RestoreInterrupts(unsigned int previousSave);          // Restores interrupts to the same IPL level as was in effect when EZBL_DissableInterrupts() was called.


/**
 * void EZBL_CallLongPointer0(unsigned long functionAddress);
 * 
 * Calls a long (23-bit) function pointer, passing no arguments (called function 
 * should have void as the argument list). Other related functions, such as 
 * EZBL_CallLongPointer1(), EZBL_CallLongPointer2(), etc. exist and can be 
 * instead called if you need to pass arguments to the called function.
 * 
 * For a null pointer, this function does nothing and returns immediately rather 
 * than trying to branch to address 0x000000. This is particularly useful when 
 * using 'weak' attributed function declarations and you want a function call to 
 * disappear when the applicable function is not linked into the project (as 
 * opposed to blindly calling a null pointer and effectively resetting the 
 * device).
 * 
 * This function first masks off the top 8 bits of the functionPointer so as to 
 * be compatible with __builtin_tblpage() and #tblpage() which may set bit 24 on 
 * devices with EDS instead of PSV architecture features (ex: dsPIC33E/PIC24E 
 * and some PIC24F products).
 *
 * Return type is specified as void, but it will actually exactly match the
 * called function return type and have the called function's return data.
 *
 * Function optional parameters correspond to the parameters that the called
 * function will receive. They are realigned so that param1 goes to w0, param2 to
 * w1, etc. like the called function will normally expect.
 */
void EZBL_CallLongPointer0(unsigned long functionAddress);
void EZBL_CallLongPointer1(unsigned long functionAddress, unsigned int param1);                         // See EZBL_CallLongPointer0() help
void EZBL_CallLongPointer2(unsigned long functionAddress, unsigned int param1, unsigned int param2);    // See EZBL_CallLongPointer0() help
void EZBL_CallLongPointer3(unsigned long functionAddress, unsigned int param1, unsigned int param2, unsigned int param3);   // See EZBL_CallLongPointer0() help
void EZBL_CallLongPointer4(unsigned long functionAddress, unsigned int param1, unsigned int param2, unsigned int param3, unsigned int param4);  // See EZBL_CallLongPointer0() help
void EZBL_CallLongPointer5(unsigned long functionAddress, unsigned int param1, unsigned int param2, unsigned int param3, unsigned int param4, unsigned int param5); // See EZBL_CallLongPointer0() help


/**
 * Macro that calls a void function (with no arguments) by token name, assuming 
 * it is linked into the project. You do not have to declare a prototype for the 
 * function first.
 * 
 * This macro internally defines the function symbol name as a weak, extern 
 * symbol using inline assembly and then calls it using the 
 * EZBL_CallLongPointer0() function.
 * 
 * @param function_name a function symbol name to call. The symbol name should 
 *                      be the "C" name (i.e. without a leading underscore 
 *                      prefix added). If the symbol doesn't exist at link time,
 *                      a few cycles are wasted without doing anything.
 * 
 * @return void     Absolutely nothing is returned in the C context. You cannot 
 *                  assign the return value from the function to a variable if 
 *                  the function didn't actually have a void return type.
 * 
 */
#define EZBL_CallFunctionIfPresent(function_name)                                       \
        __asm__ volatile("    .extern   _" STRINGIFY(function_name) "               \n" \
                         "    .weak     _" STRINGIFY(function_name) "               \n" \
                         "    mov   #tbloffset(_" STRINGIFY(function_name) "), w0   \n" \
                         "    mov   #tblpage(_" STRINGIFY(function_name) "), w1     \n" \
                         "    call  _EZBL_CallLongPointer0                          \n" \
                         : : : "w0", "w1", "w2", "w3", "w4", "w5", "w6", "w7", "cc", "memory")


void EZBL_WriteNVMCON(unsigned int nvmconValue, unsigned int unlockKeyMultiplier, unsigned int blockUntilDone);   // Must set EZBL_NVMKey = 0x03DF before calling and specify unlockKeyMultiplier = 0x03E5.
void EZBL_PartitionSwap(int executeResetCode);      // Swaps active Flash partitions in a dual-partition configured device. Set executeResetCode = 0 for a live-update that jumps directly to main(). Using executeResetCode = 1 will goto address 0x000000 on the swapped panel to execute all initialization code (but the chip will not truly reset so already initalized peripherals, SFRs, and Config bit values won't change).


/**
 * void EZBL_RestoreAppErasableBootloaderItems(unsigned int restoreConfigBytes);
 *
 * Restores any bootloader components (defined in __EZBL_ERASE_RESTORE_TABLE) 
 * that might have been erased by a Flash erase operation. For example, if the 
 * bootloader has configuration words set in it, the device has Flash based 
 * configuration words, and the last page is erased, then this function will 
 * automatically check if the needed configuration values are programmed, and 
 * if not, will reprogram them according to what the bootloader was built to 
 * have stored there. The reset vector goto instruction and possibly interrupt 
 * vectors are also handled by this function (assuming page 0 is erased and it 
 * is shared with the application - not entirely dedicated to the bootloader).
 *
 * It is safe to call this function multiple times if the restoreConfigBytes 
 * parameter = 0, even if all other bootloader components are already 
 * programmed to Flash. The function first checks the destination address to see 
 * if data is present before attempting to program it. On the other hand, if the 
 * device has Config bytes (Config words at >= address 0xF80000) and the 
 * restoreConfigBytes flag is non-zero, then this function should only be called 
 * once when needing to restore these Config bytes. Non-Flash based Config bytes
 * are always reprogrammed since they don't have a detectable erased state 
 * (like EEPROM, you just reprogram them without erasing first).
 * 
 * If a Reserved Bit in a Flash config word exists that requires a '0' to
 * be programmed to it, this bit is written to '0' by the 
 * __EZBL_ERASE_RESTORE_TABLE element that applies to it (assuming such an 
 * element was generated).
 * 
 * @param restoreConfigBytes On devices with "Non-volatile" Config bytes (not 
 *          Flash based Config Words), a non-zero value for this flag specifies 
 *          that any bootloader defined Config bytes should be reprogrammed by 
 *          this function. As noted above, these values are always programmed 
 *          for every call to EZBL_RestoreAppErasableBootloaderItems() when this 
 *          flag is set, so minimize unneeded restore operations with this flag 
 *          set to avoid wasting Config byte endurance.
 * 
 *          On devices with Flash based Config words ("Volatile Config Words"), 
 *          this flag is not applicable and ignored.
 * 
 * @return Any Flash address that the bootloader requires and has stored in the 
 *         __ERASE_RESTORE_TABLE is reprogrammed, assuming the destination 
 *         address doesn't already have data stored there. Additionally, if the 
 *         device has "Non-volatile" Config bytes and the restoreConfigBytes 
 *         parameter is non-zero, the bootloader's Config bytes will be 
 *         programmed. 
 * 
 *         For anti-bricking safety, this function always blocks until the 
 *         reprogramming is complete, even if the underlying programming 
 *         hardware operation doesn't stall the CPU for the write.
 */
void EZBL_RestoreAppErasableBootloaderItems(unsigned int restoreConfigBytes);


/**
 * signed int EZBL_ROMAddressInBootloader(unsigned long flashAddress);
 *
 * Determines if the given program memory address exists in a bootloader 
 * reserved region. If so, EZBL_WriteROMChecked() will not be able to write to 
 * this address (although EZBL_ErasePage() and EZBL_WriteROM() will still be 
 * able to erase/write to this address if you really want to modify bootloader 
 * space).
 * 
 * This is implemented as a macro that calls EZBL_MaskBootloaderRegions() with 
 * the flashAddress, a null write pointer, and length of 1 for API naming
 * compatibility reasons. Applications should call EZBL_MaskBootloaderRegions() 
 * directly if an address range needs to be tested for bootloader overlap 
 * (instead of a single memory address like this macro does).
 * 
 * @param flashAddress Specifies the program memory address that should be 
 *                     checked for overlap with any bootloader reserved region. 
 *                     Bootloader reserved regions normally include any extra 
 *                     Page boundary padding addresses as well.
 *
 * @return 0 if the address does not exist in bootloader space (normally implies 
 *           this address is Application space, but illegal addresses will also 
 *           return 0).
 *         1 if the address is in a Bootloader reserved Flash program region.
 */
#define EZBL_ROMAddressInBootloader(address)     ((signed int)EZBL_MaskBootloaderRegions(address, (void*)0x0000, 1))


/**
 * #define EZBL_GetSymbol(dest, symbolName)
 *
 * Macro that retrieves the 24 or 32-bit program memory address of the specified
 * symbol and writes it into a variable.
 *
 * Executing this macro has a similar effect to using the C address-of operator
 * (&) to get a pointer. For example calling:
 *      EZBL_GetSymbol(myFunctionPtr, main);
 * will have a similar effect to:
 *      myFunctionPtr = &main;
 * However, this macro will always capture the true 24-bit program address of
 * the main() function. Using the address-of operator will sometimes return a
 * true 16-bit program address of the main() function, and other times return a
 * 16-bit handle to a "near" program memory address that contains a "goto _main"
 * instruction in it.
 *
 * This macro can be used to retrieve symbol addresses for items that have
 * global scope, but are not defined in the C context. For example, any symbol
 * defined in a .gld linker script. The symbol need not have any C extern
 * declaration associated with it.
 *
 * Occasionally, this macro will generate an impossible operand reload assembly
 * error with certain XC16 compiler versions if certain other surrounding code
 * conditions and compiler optimization settings are used. In such a case, you
 * can either isolate this macro use to a separate function, disable compiler
 * optimizations and/or revert to the more computationally expensive SYM32()
 * macro defined in ezbl_core.h.
 *
 * @param dest 32-bit variable to write the symbol address into. This parameter
 *             must be an L-value. I.e. something you would put on the left hand
 *             side of an assignment statement such as the 'i' in:
 *                i = counter + 1;
 *             dest normally should be of unsigned long type, but other L-value
 *             variables could be used as well, so long as an assignment to it
 *             can be made from an unsigned long without requiring a type-cast.
 *
 * @param symbolName Raw C-style symbol name. For example, to get the address of
 *                   the main() function you would simply type "main" without
 *                   the quotation marks and without a leading underscore which
 *                   gets automatically added to all C symbols.
 *
 * @return 24 or 32-bit symbol address (value) in 'dest' variable.
 */
#define EZBL_GetSymbol(dest, symbolName)                                                    \
    do                                                                                      \
    {                                                                                       \
        volatile register unsigned long destReg;                                            \
        __asm__ volatile ("mov #packed_lo(_" STRINGIFY(symbolName) "), %0  \n"              \
                      "    mov #packed_hi(_" STRINGIFY(symbolName) "), %d0   " : "=r"(destReg)); \
        dest = destReg;                                                                     \
    } while(0)

/**
 * #define EZBL_GetWeakSymbol(dest, symbolName)
 *
 * Macro that retrieves the 24 or 32-bit program memory address of the specified
 * symbol and writes it into a variable. If the symbol is undefined at link 
 * time, rather than failing to link, linking will succeed and the run-time 
 * return dest value will be 0.
 * 
 * Executing this macro has a similar effect to using the C address-of operator
 * (&) to get a pointer. For example calling:
 *      EZBL_GetWeakSymbol(myFunctionPtr, main);
 * will have a similar effect to:
 *      myFunctionPtr = &main;
 * However, this macro will always capture the true 24-bit program address of
 * the main() function. Using the address-of operator will sometimes return a
 * true 16-bit program address of the main() function, and other times return a
 * 16-bit handle to a "near" program memory address that contains a "goto _main"
 * instruction in it.
 *
 * This macro can be used to retrieve symbol addresses for items that have
 * global scope, but are not defined in the C context. For example, any symbol
 * defined in a .gld linker script. The symbol need not have any C extern
 * declaration associated with it.
 *
 * Occasionally, this macro will generate an impossible operand reload assembly
 * error with certain XC16 compiler versions if certain other surrounding code
 * conditions and compiler optimization settings are used. In such a case, you
 * can either isolate this macro use to a separate function, disable compiler
 * optimizations and/or revert to the more computationally expensive SYM32()
 * macro defined in ezbl_core.h.
 *
 * @param dest 32-bit variable to write the symbol address into. This parameter
 *             must be an L-value. I.e. something you would put on the left hand
 *             side of an assignment statement such as the 'i' in:
 *                i = counter + 1;
 *             dest normally should be of unsigned long type, but other L-value
 *             variables could be used as well, so long as an assignment to it
 *             can be made from an unsigned long without requiring a type-cast.
 *
 * @param symbolName Raw C-style symbol name. For example, to get the address of
 *                   the main() function you would simply type "main" without
 *                   the quotation marks and without a leading underscore which
 *                   gets automatically added to all C symbols.
 *
 * @return 24 or 32-bit symbol address (value) in 'dest' variable if symbolName 
 *         exists. When symbolName does not exist, 0 is returned.
 */
#define EZBL_GetWeakSymbol(dest, symbolName)                                                \
    do                                                                                      \
    {                                                                                       \
        volatile register unsigned long destReg;                                            \
        __asm__ volatile (".weak     _" STRINGIFY(symbolName) "            \n"              \
                      "    mov #packed_lo(_" STRINGIFY(symbolName) "), %0  \n"              \
                      "    mov #packed_hi(_" STRINGIFY(symbolName) "), %d0   " : "=r"(destReg)); \
        dest = destReg;                                                                     \
    } while(0)



/**
 * unsigned long EZBL_Mul3Div2(unsigned int addrToByteNumber);
 *
 * Multiplies a 16-bit unsigned integer number by 3/2 halfs. Normally, this is
 * needed when converting a Flash address into a contiguous byte offset. Each
 * Flash address increment is normally '2', which represents '3' data bytes.
 * Requires 9 instruction cycles, not including the call, but including the
 * return.
 *
 * @param addrToByteNumber unsigned 16-bit integer to be multiplied by 3/2.
 *
 * @return unsigned 32-bit long integer of addrToByteNumber multiplied by 3 and
 *         divided by 2. If an odd number is provided as input, the result
 *         has the fractional 0.5 return portion truncated off. Ex:
 *         1*(3/2) = 1, 3*(3/2) = 4, 5*(3/2) = 7, etc. Even numbered inputs
 *         always result in perfect precision since they are always integer
 *         divisible by 2 without data loss.
 */
unsigned long EZBL_Mul3Div2(unsigned int addrToByteNumber);


/**
 * unsigned long EZBL_Mul3Div2L(unsigned long addrToByteNumber);
 *
 * Multiplies a 32-bit unsigned long number by 3/2 halfs. Normally, this is
 * needed when converting a Flash address offset into a byte count. Each
 * Flash address increment is normally '2', which represents '3' data bytes.
 *
 * Requires 11 instruction cycles, not including the call, but including the
 * return. addrToByteNumber > 2,863,311,530 will result in overflow (but the
 * return result will still be correct in modulo 32-bit math; i.e. just add
 * 0x100000000 if(addrToByteNumber > 2,863,311,530) to get the correct result)
 *
 * @param addrToByteNumber unsigned 32-bit long integer to be multiplied by 3/2.
 *                         If your input will always be 16-bits or less, the
 *                         EZBL_Mul3Div2() function can be used instead to
 *                         save a clock cycle or two.
 *
 * @return unsigned 32-bit long integer of addrToByteNumber multiplied by 3 and
 *         divided by 2. If an odd number is provided as input, the result
 *         has the fractional 0.5 return portion truncated off. Ex:
 *         1*(3/2) = 1, 3*(3/2) = 4, 5*(3/2) = 7, etc. Even numbered inputs
 *         always result in perfect precision since they are always integer
 *         divisible by 2 without data loss.
 */
unsigned long EZBL_Mul3Div2L(unsigned long addrToByteNumber);


/**
 * unsigned int EZBL_Div3Mul2(unsigned int byteToAddrNumber, unsigned int *remainder);
 *
 * Multiplies a 16-bit unsigned integer number by 2/3rds and optionally returns
 * the remainder of the divide by 3 operation (effectively before the multiply
 * by 2). Normally, this function is needed when converting a byte count into a
 * Flash address increment. Each Flash address increment is '2', which represents
 * '3' data bytes. If there are any remainder bytes that cannot fill a whole
 * Flash address increment of 2, then the remainder bytes can be returned via the
 * *remainder pointer. Specify a null pointer if the remainder is not needed.
 *
 * Requires 32 instruction cycles (on dsPIC33E/PIC24E) or 27 cycles (on
 * PIC24/dsPIC33F), including the return, but not the call.
 *
 * @param byteToAddrNumber unsigned 16-bit integer to be multiplied by 2 and
 *                         divided by 3. If your input can be larger than
 *                         16-bits, use the EZBL_Div3Mul2L() function
 *                         instead.
 *
 * @param *remainder    Optional pointer to an unsigned 16-bit integer to store
 *                      the division by 3 remainder. This parameter can be null
 *                      (0x0000) if the remainder is not needed. The remainder
 *                      is computed after the multiplication by 2 has already
 *                      been done to yield perfect precision. Ex:
 *                      1 * (2/3) = 0 Remainder 1, 2 * (2/3) = 1 Remainder 1,
 *                      3 * (2/3) = 2 Remainder 0, 4 * (2/3) = 2 Remainder 2,
 *                      etc.
 *
 * @return unsigned 16-bit integer result of the computation where the remainder
 *         (if any) is truncated off. If the remainder is needed, use the
 *         *remainder input option to retrieve it.
 */
unsigned int EZBL_Div3Mul2(unsigned int byteToAddrNumber, unsigned int *remainder);


/**
 * unsigned long EZBL_Div3Mul2L(unsigned long byteToAddrNumber, unsigned int *remainder);
 *
 * Multiplies a 32-bit unsigned long number by 2/3rds and optionally returns the
 * remainder of the divide by 3 operation (effectively before the multiply by 2).
 * Normally, this function is needed when converting a byte count into a Flash
 * address increment. Each Flash address increment is '2', which represents '3'
 * data bytes. If there are any remainder bytes that cannot fill a whole Flash
 * address increment of 2, then the remainder bytes can be returned via the
 * *remainder pointer. Specify a null pointer if the remainder is not needed.
 * Multiplication and division is done iteratively with bit shifts.
 *
 * Requires around 570 instruction cycles (on dsPIC33E/PIC24E), up to about
 * (exactly?) 634 maximum, including the return, but not the call. Fewer cycles
 * are required on PIC24/dsPIC33F due to shorter by about 173 cycles (~397 total
 * typically) due to branch delay reduction.
 *
 * @param byteToAddrNumber unsigned 32-bit integer to be multiplied by 2 and
 *                         divided by 3. If your input will always be 16-bits or
 *                         less, use the EZBL_Div3Mul2() function instead for
 *                         greater efficiency.
 *
 * @param *remainder    Optional pointer to an unsigned 16-bit integer to store
 *                      the division by 3 remainder. This parameter can be null
 *                      (0x0000) if the remainder is not needed. The remainder
 *                      is computed after the multiplication by 2 has already
 *                      been done to yield perfect precision. Ex:
 *                      1 * (2/3) = 0 Remainder 1, 2 * (2/3) = 1 Remainder 1,
 *                      3 * (2/3) = 2 Remainder 0, 4 * (2/3) = 2 Remainder 2,
 *                      etc.
 *
 * @return unsigned 32-bit long integer result of the computation where the
 *         remainder (if any) is truncated off. If the remainder is needed, use
 *         the *remainder input option to retrieve it.
 */
unsigned long EZBL_Div3Mul2L(unsigned long byteToAddrNumber, unsigned int *remainder);


/** 
 * unsigned long EZBL_ReadDEVID(void);
 *
 * @return Returns the 24-bit contents of the "DEVID" Device ID program 
 *         memory location at 0xFF0000. Upper 8-bits returned are always zeros.
 */
unsigned long EZBL_ReadDEVID(void);


/** 
 * unsigned long EZBL_ReadDEVREV(void);
 *
 * @return Returns the 24-bit contents of the "DEVREV" Device Revision program 
 *         memory location at 0xFF0002. Upper 8-bits returned are always zeros.
 */
unsigned long EZBL_ReadDEVREV(void);


/**
 * unsigned long EZBL_ReadFlashInstruction(unsigned long address);
 *
 * Reads one instruction word (24 bits) from Flash at the specified Flash
 * program memory address. The returned uppermost 8 bits are always 0x00.
 *
 * Besides ordinary program memory locations, this function can be used to read
 * from Auxiliary Flash, Configuration fuse bytes, and other debug or
 * programming executive memory spaces. On applicable devices programmed for
 * multiple Flash partitions, this function can also be used to read from the
 * inactive Flash partition (starting at address 0x400000), but only when the
 * inactive Flash partition is idle (i.e. not undergoing an erase or programming
 * operation). If the inactive partition is busy with an erase or programming
 * operation, the return results will be indeterminate.
 *
 * Care should be taken to ensure that the target address is legal for the
 * device. Attempting to read from unimplemented memory locations will, in most
 * cases, result in an Address Error Trap rather than returning 0x000000 data.
 * Similarly, on devices that implement segmented Flash Code Protection
 * security, attempting to read from a Flash region that is disallowed will
 * trigger a device reset.
 *
 * This function must temporary change the TBLPAG register contents in order to
 * issue the read. Ensure no interrupts are implemented which require TBLPAG to
 * be always valid. Alternatively, disable interrupts before calling this
 * function. The TBLPAG contents are restored upon return.
 *
 * @param address   unsigned 32-bit long integer of the address to read from
 *                  (will actually be 24-bits max and the LSbit must be set to
 *                  0). The EZBL_GetSymbol() macro can be used if you wish to
 *                  discover the address of an appropriate symbol.
 *
 * @return  unsigned 32-bit long integer data read from the given program memory
 *          address. Since program data is always 24-bits wide, the upper 8 bits
 *          will always be 0x00.
 */
extern unsigned long EZBL_ReadFlashInstruction(unsigned long address);


/**
 * void EZBL_ReadPackedFlash(void *dest, unsigned long baseAddress, unsigned int byteOffset, unsigned int byteCount);
 *
 * Copies Flash/program memory region contents into RAM.
 *
 * Besides ordinary program memory locations, this function can be used to read
 * from Auxiliary Flash, Configuration fuse bytes, and other debug or
 * programming executive memory spaces. On applicable devices programmed for
 * multiple Flash partitions, this function can also be used to read from the
 * inactive Flash partition (starting at address 0x400000), but only when the
 * inactive Flash partition is idle (i.e. not undergoing an erase or programming
 * operation). If the inactive partition is busy with an erase or programming
 * operation, the return results will be indeterminate.
 *
 * Care should be taken to ensure that the target address is legal for the
 * device. Attempting to read from unimplemented memory locations will, in most
 * cases, result in an Address Error Trap rather than returning 0x000000 data.
 * Similarly, on devices that implement segmented Flash Code Protection
 * security, attempting to read from a Flash region that is disallowed will
 * trigger a device reset.
 *
 * This function must temporary change the TBLPAG register contents in order to
 * issue the read. Ensure no interrupts are implemented which require TBLPAG to
 * be always valid. Alternatively, disable interrupts before calling this
 * function. The TBLPAG contents are restored upon return.
 *
 * @param *dest RAM pointer to write the read program memory data to. This
 *              pointer has no alignment requirements (i.e. can be odd). This
 *              parameter can be null only if byteCount is zero.
 *
 * @param baseAddress Unsigned 32-bit (24-bit) program memory address partially
 *                    specifying where to read from. This address must be even
 *                    (instruction aligned address). The byteOffset parameter is
 *                    added to this address (with proper 3 byte/2 address
 *                    translation) to generate the actual starting read
 *                    location.
 *
 * @param byteOffset A count of how many bytes to add to baseAddress before
 *                   beginning reading. Proper 3-byte/2-address conversion is
 *                   done. For example, if you want to read bits 8-23 of an
 *                   instruction word at address 0x001234 (throwing the first
 *                   8-bits of instruction word data away), you could specify
 *                   baseAddress = 0x001234 and byteOffset = 1.
 *                   Alternatively, baseAddress = 0x001230 and byteOffset = 7
 *                   would address the same byte (byteOffsets 0, 1, and 2 would
 *                   be the 24-bit data bits from address 0x001230; offsets 3,
 *                   4, and 5 would be data from 0x001232, and offset 6 would be
 *                   the least significant 8-bits of the data at address
 *                   0x001234.
 *
 * @param byteCount  Count of many bytes to read from Flash starting at the
 *                   specified address and byte offset. The *dest RAM must have
 *                   this much free-space allocated.
 *
 * @return  Program memory data read is written to the *dest pointer. The data
 *          is returned in naturally packed form (i.e. no "phantom" 0x00 bytes
 *          included).
 */
void EZBL_ReadPackedFlash(void *dest, unsigned long baseAddress, unsigned int byteOffset, unsigned int byteCount);


/**
 * void EZBL_ErasePage(unsigned long addressOnPage);
 *
 * Erases the page containing the specified address. Be very careful if calling
 * this function as the function will erase any legal page, including ones
 * inside your bootloader and also possibly including the same page that this
 * function itself is stored on.
 *
 * EZBL_NVMKey must have been externally set to 0x03DF or this function will
 * do nothing. Immediately upon use or return, EZBL_NVMKey will be set to
 * 0x0000.
 *
 * Do not call this function on PIC24FxxKxxx, or dsPIC30F devices. They have very
 * different NVMCON bit encodings and are not supported at this time.
 *
 * @param addressOnPage 24-bit (unsigned long) specifying an address on the page
 *                      to be erased. Anywhere on the page will work as the
 *                      hardware will mask off the lower bits within the page.
 */
void EZBL_ErasePage(unsigned long addressOnPage);


/**
 * unsigned long EZBL_EraseAllPiecewise(unsigned long lastAddressState);
 *
 * Enumerates and erases all non-bootloader Flash pages, returning between each
 * erase operation so the application has a chance to print status messages or
 * do other tasks instead of blocking continuously for the entire erase, which
 * could take a long time (ex: 7 seconds on a big device).
 *
 * If any bootloader components get erased by this function, they are
 * automatically reprogrammed as soon as possible before returning. Reset and
 * IVT vectors + Flash configuration fuse values specified in the bootloader,
 * assuming they are allowed to be erased, would be restored.
 *
 * @param lastAddressState
 *      Previous erase address state information returned by the prior call to
 *      EZBL_EraseAllPiecewise(). This is used to identify which Flash page
 *      should be erased. When calling for the first time, specify a value of
 *      0x00000000.
 *
 * @remarks EZBL_NVMKey
 *      The EZBL_NVMKey global variable must be set to a value of
 *      <b>0xFC21</b> before calling this function or the erase will be aborted
 *      and the returned state information will not increment. Upon success or
 *      abort return, EZBL_NVMKey will be cleared to 0x0000.
 *
 * @return unsigned long
 *      Internal state information identifying what sector needs to be erased
 *      next. The order of returned addresses is not always sequential or
 *      guaranteed. However, on devices that have configuration words stored in
 *      Flash and the bootloader allows erasing them, it is guaranteed that the
 *      page with the configuration words will be the last erased sector.
 *
 *      The quantity of pages erased isn't constant as already erased pages are
 *      skipped.
 *
 *      When all pages have been erased, 0xFFFFFFFF will be returned, indicating
 *      the erase is complete and you should stop calling
 *      EZBL_EraseAllPiecewise().
 */
unsigned long EZBL_EraseAllPiecewise(unsigned long lastAddressState);


/**
 * void EZBL_EraseInactivePartition(void);
 *
 * Starts erasing the entire inactive partition and then returns without waiting
 * for completion. NVMCON<WREN> remains set once the erase is done.
 *
 * Unlike most NVM functions, you do not have to set EZBL_NVMKey to any
 * particular value to allow the erase. You don't have to set it externally 
 * since erasing the inactive partition won't normally brick your device. You 
 * will have what you are currently running, so less paranoia is warranted. 
 * Traditional single-boot applications shouldn't call this function, so the 
 * linker will be able to remove this code and not store the NVM unlock sequence 
 * so close to the NVM unlock operation.
 *
 * Upon function return, EZBL_NVMKey will be cleared to 0x0000.
 *
 * To check if the erase is done, check NVMCON<WR> or NVMCONbits.WR. Zero means 
 * the erase is done.
 *
 * If this function is called while a prior NVM Flash erase or write operation is
 * already in progress, the function blocks until the prior operation completes.
 * The erase operation is then started before returning.
 */
void EZBL_EraseInactivePartition(void);


/**
 * unsigned long EZBL_WriteROM(unsigned long flashWriteAddress, void *srcData, unsigned int dataByteCount);
 *
 * Programs an array of bytes into Flash or Config Word memory at the specified
 * Program Address without address checking. This function can overwrite or 
 * brick an existing bootloader. For a safer application-layer Flash write 
 * function, call EZBL_WriteROMChecked() instead.
 *
 * Before calling this function, set EZBL_NVMKey = 0x03DF to permit
 * the Flash write. Any other value in EZBL_NVMKey will cause all function
 * behavior to execute, but without actually writing anything to Flash.
 *
 * The operation blocks during the write operation. If this function is called
 * while a prior non-blocking NVM Flash erase or write operation is already in
 * progress, the function safely blocks until the prior operation completes. This
 * function disables interrupts only for each Flash word programming operation,
 * and then reenables interrupts immediately after the word programming operation.
 * This allows queued interrupts to still execute with minimal extra latency.
 *
 * This function implements data masking. If all bytes within a given Flash
 * programming word are 0xFF, the given word address will be skipped and not
 * programmed (although the address pointers and dataByteCounter will still
 * increment). This enables preexisting data preservation without wasting time or
 * Flash endurance/retention by programming the actual Flash cell (ex: can allow
 * any legal address range to be passed in, but for any region occupied by a
 * bootloader, change the data to all '1's so they don't damage the bootloader
 * or Flash by multi-programming the same address). This masking does not apply
 * when programming "Non-volatile" type Config Words (0xF8xxxx address range)
 * since they don't need to be erased before being programmed, thereby making
 * all '1's a potentially intended write value.
 *
 * This function can be used to program "Volatile" Flash based Configuration
 * Words stored in the last page of Flash memory. It will also program
 * "Non-volatile" type Config Words (exist in the 0xF8xxxx address range).
 *
 * This function cannot be used to program EEPROM or Data Flash, if it exists.
 * These memories require different programming sizes and NVMCON control codes.
 *
 * This function will program Auxiliary Flash and Inactive Panel Flash if it
 * exists on the device.
 *
 * On devices that implement Flash ECC, you must not attempt to program the same
 * Flash word location more than once per erase (i.e. all such bytes must be
 * masked as 0xFF if they exist in the destination address range). On devices
 * without Flash ECC, programming a preexisting Flash word a second time might be
 * permissable, but is illegal on many devices because it is outside the
 * specified Flash operating characteristics I.e. doing so may damage the Flash
 * cells and they will no longer meet specified Flash retention and endurance
 * specifications. Programming a Flash word 3+ times without erasing it first is
 * never allowed (although this function does not prevent you from doing so).
 *
 * @param flashWriteAddress
 *   Address in Program Memory to start writing to. This parameter must be
 *   a legal memory address aligned to the native Flash word width (3 or 6 bytes,
 *   which equates to 0x2 or 0x4 addresses), or, if targeting non-volatile
 *   Config Words, aligned to 3 bytes/0x2 addresses. Bit 0 must be cleared. No
 *   error checking is done for unaligned or addresses outside the implemented
 *   destination memory.
 *
 * @param srcData
 *   Pointer to the source data in RAM to write to Flash or Config Words. The
 *   data to write must be packed in RAM without "phantom bytes", ex:
 *       srcData[0]: first instruction (or first Config Word byte), low byte
 *       srcData[1]: first instruction (or dummy byte for Config Words), middle byte
 *       srcData[2]: first instruction (or dummy byte for Config Words), high byte
 *       srcData[3]: second instruction (or second Config Word byte), low byte
 *       srcData[4]: second instruction (or dummy byte for Config Words), middle byte
 *       srcData[5]: second instruction (or dummy byte for Config Words), high byte
 *
 * @param dataByteCount
 *   Number of bytes to write to Flash or Config Words. If the data length is
 *   not an integer multiple of the native Flash word or Config Word write size,
 *   the bytes in the last partial word are padded with 0xFF bytes and still
 *   programmed. This occurs without causing RAM addresses beyond the specified
 *   length from being accessed.
 *
 *   No error checking is done to ensure the flashWriteAddress + effective number
 *   of addresses needed for dataByteCount is physically present on the device.
 *   Care should be taken to ensure an attempt to program outside the legal
 *   address range is not performed as the results are undefined and may brick
 *   the device (ex: possible address wraparound).
 *
 * @return
 *   Returns the next sequential Flash word's memory address. In other words:
 *   (flashWriteAddress + (dataByteCount+2)/3*2) or
 *   (flashWriteAddress + (dataByteCount+5)/6*4) depending on the device and
 *   destination memory range.
 *
 *   The value contained in EZBL_NVMKey is cleared to 0x0000.
 *
 *   NVMCON<WR> and NVMCON<WREN> are cleared upon return. Any other NVMCON bits
 *   (like NVMPIDL for NVM Power Down In Idle), if they exist, will likely be
 *   cleared and must be saved/restored outside this function.
 */
unsigned long EZBL_WriteROM(unsigned long destFlashAddress, void *srcData, unsigned int dataByteCount);


/**
 * unsigned long EZBL_WriteROMChecked(unsigned long destFlashAddress, void *srcData, unsigned int dataByteCount);
 *
 * Identical to EZBL_WriteROM() except that the address range is checked
 * first for overlap with any bootloader addresses. Like EZBL_WriteROM(),
 * set EZBL_NVMKey = 0x03DF to permit the write. Any other value in
 * EZBL_NVMKey will cause all function behavior to execute, but without
 * actually writing anything to Flash or Config Words.
 *
 * If any overlap in Flash memory is detected, the overlapped region data is
 * converted to all '1's which is then skipped when Flash programming to
 * preserve all bootloader memory region contents.
 *
 * If any overlap in Non-volatile Config Word memory forced by the bootloader is
 * detected (addresses in 0xF8xxxx range), the overlapped region data is
 * converted to match the values coded in the bootloader so as not to allow the
 * bootloader specified values to change. This is done instead of all '1's
 * masking since all '1's is a valid Configuration Word value and can't be
 * assumed to already exist due to a prior erase operation. I.e. writing all
 * '1's are not skipped during programming.
 *
 * @param destFlashAddress 24-bit unsigned long program memory address to start
 *                         writing to. The LSbit must always be '0'. When
 *                         programming Flash memory with a native 48-bit Flash
 *                         word size, the second LSbit must also be '0' to
 *                         align to the needed 0x4 boundary.
 *
 * @param *srcData Pointer to source data in RAM which should be copied to Flash
 *                 or Config Words. This data should be packed in 24-bit chunks
 *                 without phantom bytes. srcData must be writable. The data can
 *                 change while masking off overlapped bootloader regions. This
 *                 pointer does not have any alignment requirements.
 *
 * @param dataByteCount unsigned integer specifying how many bytes to
 *                      sequentially copy from *srcData to the Program Memory
 *                      address range. This value need not result in write
 *                      termination on a perfect Flash word or Config Word
 *                      address boundary. Automatic 0xFF padding will be done to
 *                      fill the word before programming.
 *
 * @return 24-bit unsigned long program memory address after the just programmed
 *         Flash Word or Config Word. *srcData RAM contents may also have
 *         changed according to the bootloader masking behavior.
 */
unsigned long EZBL_WriteROMChecked(unsigned long destFlashAddress, void *srcData, unsigned int dataByteCount);


/**
 * unsigned int EZBL_MaskBootloaderRegions(unsigned long flashWriteAddress, void *writeData, unsigned int writeByteLen);
 *
 * Determines if the given program memory address and length overlaps with any
 * bootloader reserved regions. If they do and the write destination is Flash
 * memory, then the writeData for the overlapping region is changed to all '1's
 * so as not to damage the bootloader when programmed with the EZBL_WriteROM()
 * function. If address overlap exists and the write destination is in a
 * Non-volatile Configuration Words area (addresses of 0xF8xxxx), then the
 * overlapping region is changed to match the bootloader specified values.
 *
 * @param flashWriteAddress Specifies the starting program memory address that
 *                          this write data is intended for.
 *
 * @param *writeData Specifies a pointer to the data planned for write to
 *                   program memory. If this parameter is null (0x0000), no data
 *                   masking is performed. However, the number of bytes that
 *                   would have been masked off is still computed and returned.
 *
 * @param writeByteLen Number of bytes of intended write data that needs to be
 *                     masked off or checked for overlap with bootloader
 *                     reserved regions.
 *
 * @return Returns the byte count that has been masked off (or would have been
 *         masked off if data were provided). A return value of 0 means the
 *         specified write address range did not overlap with any bootloader
 *         reserved memory address regions.
 */
unsigned int EZBL_MaskBootloaderRegions(unsigned long flashWriteAddress, void *writeData, unsigned int writeByteLen);


/**
 * unsigned int EZBL_IsPageErased(unsigned long programAddress);
 *
 * Quickly tests if all bytes on a given sector/page are erased. Returns 1 if
 * the entire Page has '1's on it, or 0 otherwise. No special address checking
 * or conditions are exempted.
 *
 * Performance should be very fast relative to other Flash reading routines. It
 * will still be quite slow, however. Ex: perhaps ~7800 cycles/0x400 sized
 * sector.
 *
 * @param programAddress Flash program memory address to check. This address
 *                       does not have to be page aligned. All bytes on the page
 *                       will still be checked if an unaligned (or odd) address
 *                       is provided.
 *
 * @return 1 - Page is erased and contains all 0xFF bytes
 *         0 - Page contains data
 */
unsigned int EZBL_IsPageErased(unsigned long programAddress);


/**
 * Computes or continues a CRC-32 (IEEE 802.3 Ethernet version) over RAM data
 *
 * The polynomial implemented is:
 * x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1
 * Commonly this is represented as 0xEDB88320 (or 0x04C11DB7 for big endian).
 *
 * @param crcSeed Starting value to use in the shift register. Set to 0x00000000
 *                when starting a new CRC32 computation.
 * @param data Pointer to RAM memory to start reading from. There are no
 *             alignment restrictions.
 * @param byteLength Count of how many bytes to include in this CRC computation
 *                   starting at the specified pointer address.
 *
 * @return 32-bit CRC computed
 */
unsigned long EZBL_CRC32(unsigned long crcSeed, void *data, unsigned int byteLength);


/**
 * Computes or continues a CRC-32 (IEEE 802.3 Ethernet version) over Flash data
 *
 * The polynomial implemented is:
 * x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1
 * Commonly this is represented as 0xEDB88320 (or 0x04C11DB7 for big endian).
 *
 * @param crcSeed starting value to use in the shift register. Set to 0x00000000
 *                when starting a new CRC32 computation.
 * @param programAddress Program memory address to start reading from. This must
 *                       be aligned to an instruction boundary (i.e. always even
 *                       numbered) on 16-bit PICs/dsPICs.
 * @param byteLength unsigned long count of how many bytes to include in this
 *                   CRC computation starting at the specified address. This
 *                   need not be an integral multiple of the underlying Flash
 *                   storage instruction or word size.
 *
 * @return 32-bit CRC computed
 */
unsigned long EZBL_CRC32Prog(unsigned long crcSeed, unsigned long programAddress, unsigned long byteLength);


/**
 * unsigned long EZBL_CRC32NVRegions(unsigned int typeOfRegions);
 * 
 * Computes a CRC-32 (IEEE 802.3 Ethernet version) over all Flash and 
 * non-volatile addresses containing the bootloader and reserved data,
 * the application (any non-bootloader area), or the entire device's implemented
 * non-volatile memories (both).
 *
 * The _EZBL_NV_GEOMETRY_TABLE_BASE, _EZBL_ROM_USE_TABLE_BASE, and
 * _EZBL_APP_SPACE_TABLE_BASE symbols and their associated table data must
 * be defined in your device linker script for this function to be used.
 * Otherwise a link-time error will be triggered (or if the symbols are present,
 * but incorrect table data, then run-time failure). The tables are generated by
 * ezbl_tools.jar.
 *
 * @param typeOfRegions unsigned integer specifying which regions to include in
 *                      the CRC:
 *                          0 - all non-volatile addresses present on the device
 *                          1 - all bootloader occupied or reserved addresses
 *                          2 - all application occupied or free space addresses
 *                              (everything except bootloader occupied and
 *                              reserved addresses)
 *
 * @return Computed CRC32 over all of the regions occupied by the
 *         specified type.
 */
unsigned long EZBL_CRC32NVRegions(unsigned int typeOfRegions);



/**
 * Decryption prototypes that a bootloader might need. Note that these are NOT
 * distributed with the Easy Bootloader for PIC24 and dsPIC33 at this time. In 
 * order to use bootloader encryption and these functions, you MUST first
 * download the Microchip Library of Applications (MLA) from
 * http://www.microchip.com/mla.
 * 
 * Once downloaded, install the MLA and import the aes_encrypt_16bit.s and
 * aes_128bit_16bit.s files into your bootloader project before initial
 * compilation. This bootloader was implemented and tested with MLA version
 * v2013-12-20 files; so if you have problems building with encryption, be sure
 * and obtain this exact same version. If aes_encrypt_16bit.s and
 * aes_128bit_16bit.s are removed or missing from the MPLAB X project, the
 * project will still build successfully, but bootloader decryption would then
 * be unavailable at run time. When building without encryption, you must also
 * remove the "-password=xyz" command line option from your bootloader and
 * bootloaded projects, as applicable.
 */
void __attribute__((weak)) AES128EncryptBlock(void *outputData, void *inputData, void *roundKeys);
void __attribute__((weak)) AES_RoundKeysCreate128(void *roundKeysStruct, void *baseKey);


/**
 * Encrypts or decrypts a .blob file data stream located in RAM and writes the
 * decrypted or encrypted stream bytes back to RAM.
 *
 * This function uses the AES 128-bit cipher in Counter (CTR) Mode with a
 * pre-stored key within the bootloader (statically defined at bootloader build
 * time) to either encrypt or decrypt the .blob. Because Counter Mode XORs a
 * key-stream with the plaintext/ciphertext to create the ciphertext/plaintext,
 * this single function can be used for both encryption and decryption while
 * only requiring the AES ECB encryption primitive.
 *
 * Treat the returned cryptState information as sensitive as it will contain
 * all AES key material and leaking it would be equivalent to leaking your
 * original encryption/decryption key/password.
 *
 * @param cryptState   Pointer to an externally allocated EZBL_CRYPT_STATE
 *                     structure that contains the current state of the
 *                     encryption/decryption engine. When starting a new blob
 *                     stream, the cryptState->offset value must be externally
 *                     pre-initialized to 0x00000000. When continuing a .blob
 *                     stream, pass in the same structure that was returned in
 *                     the prior EZBL_CryptBlob call.
 *
 * @param blobBytesIn  Pointer to an array of bytes in RAM that contains the
 *                     data to encrypt/decrypt.
 *
 *                     On first initialization at least 20 bytes of the .blob
 *                     must be provided.
 *
 * @param blobBytesOut Pointer to a memory buffer in RAM to write the
 *                     encrypted/decrypted data out. This pointer can be set
 *                     equal to blobBytesIn to cause encryption/decryption in
 *                     place. blobBytesOut can be set to a lower-but-overlapping
 *                     address range relative to blobBytesIn to move the data
 *                     simultaneously with the crypto operation (ex: useful for
 *                     repositioning the data to an earlier position in a FIFO
 *                     after creating a "hole").
 *
 *                     Do not set this to a higher-and-overlapping address range
 *                     to blobBytesIn.
 *
 * @param bytesToCrypt Value specifying how many bytes of .blob data
 *                     should be encrypted/decrypted from blobBytesIn to
 *                     blobBytesOut.
 *
 * @return Encrypted/decrypted data is written to *blobBytesOut. The contents of
 *         the cryptState structure are also updated to reflect the new position
 *         within the stream. The signed int return value is reserved and should
 *         not be used.
 *
 *         If the bootloader was built without an encryption key/password, this
 *         function will return immediately without altering the data or
 *         cryptState. However, any movement operation specified by differing
 *         pointers for blobBytesIn and blobBytesOut will still be performed.
 *
 *         Also, if the project is compiled without the required AES primitives
 *         present, this function will similarly return without doing anything
 *         except the possible movement operation.
 */
signed int EZBL_CryptBlob(EZBL_CRYPT_STATE *cryptState, unsigned char *blobBytesIn, unsigned char *blobBytesOut, unsigned int bytesToCrypt);


/**
 * extern volatile unsigned int __attribute__((near)) EZBL_NVMKey;
 * 
 * RAM NVM Unlock Key Storage so that only code that is executing correctly will 
 * be likely to be able to start a Flash erase/write operation. 
 * 
 * Set EZBL_NVMKey = 0x03DF to unlock EZBL Flash erase/write operations for:
 *      EZBL_ErasePage()
 *      EZBL_WriteROM()
 *      EZBL_WriteROMChecked()
 *      EZBL_WriteROMRow()
 *      EZBL_WriteROMRowChecked()
 *      EZBL_WriteROMOnlyOnce()
 *      EZBL_WriteNVMCON()
 * Set EZBL_NVMKey = 0xFC21 for:
 *      EZBL_EraseAll()
 *      EZBL_EraseAllPiecewise()
 * All other values will prohibit any EZBL Flash erase/write command from 
 * starting. EZBL_NVMKey will self-clear to 0x0000 immediately after
 * performing an NVMCON WR operation, so the proper key value must be set before 
 * each applicable function call.
 * 
 * Theory of operation:
 * When legal internal and external operational conditions exist for voltage, 
 * temperature and frequency, the PIC will execute instructions 100% correctly 
 * and reliably, allowing code to set EZBL_NVMKey to a needed value, execute a 
 * number of subsequent instructions, and then start the hardware NVMCON 
 * unlocking sequence to initiate a Flash erase or programming operation. 
 * 
 * When illegal but real life electrical conditions exist (such as: 
 * over-clocking the PIC, high EMI/EMC/ESD/other noise events exist near by, BOR 
 * disabled and VDD too low, extremely slow rising-falling-rising VDD ramp 
 * conditions preventing POR, device temperature extremely high), it is 
 * conceivable that the CPU will not stay in reset, but instead be attempting to 
 * execute instructions. Some instructions read from Flash could have bit errors 
 * and the ALU may compute wrong results, so a branch or call instruction
 * could branch to the wrong address (or perhaps the PC may increment by a value 
 * other than 0x000002 addresses after executing an instruction). If the PC ever 
 * ends up at the NVMCON manipulation code, having full unlock code there would 
 * allow self-bricking via an unintended erase or programming operation 
 * executing. By requiring this RAM state to be exactly right, using this 
 * key state to compute the actual NVMCON hardware unlock sequence values, and 
 * physically locating these pieces of code at different Flash addresses, a 
 * considerably greater number of instructions must execute correctly before a 
 * Flash command will succeed. The chance of field self-bricking failure is 
 * theoretically reduced as only correct program flow will enable setting 
 * NVMCON<WR>.
 */
extern volatile unsigned int __attribute__((near)) EZBL_NVMKey;

/**
 * extern volatile unsigned long __attribute__((near, persistent)) EZBL_ForwardBootloaderISR;
 * 
 * RAM bitfield controlling/indicating which interrupts that the bootloader 
 * implements should be passed to the application's interrupt handler or trapped 
 * by the bootloader.
 * 
 * Set individual bits to '1' to turn off a bootloader's ISR and forward the 
 * applicable interrupt to the application's ISR. Setting a bit to '0' causes 
 * the application's ISR to be ignored with execution of the bootloader's ISR 
 * occurring instead. The reset default state of this variable is 0x00000000, 
 * meaning all ISRs that the bootloader implements will be active and all 
 * non-bootloader-implemented interrupts will be sent to the application's 
 * ISR (or application's _DefaultInterrupt() function when unimplemented in both 
 * the bootloader and application).
 * 
 * The exact mapping of bits in this RAM variable to their Interrupt Vector is 
 * dynamically generated when the bootloader is compiled and depends on the 
 * IRQ's natural interrupt order and whether the bootloader uses the interrupt. 
 * Bits are implemented starting from the LSbit of EZBL_ForwardBootloaderISR and 
 * from the lowest Vector or IRQ Number, ascending towards the MSb and highest 
 * Vector and IRQ numbers.
 * 
 * Because it is possible to add or remove ISRs to your bootloader as you work 
 * on it, the flag bit ordering and positions within the 
 * EZBL_ForwardBootloaderISR variable could change. Therefore, the bootloader 
 * build sequence exports some global symbols containing the bitmask needed to 
 * set or clear individual interrupt flag bits in EZBL_ForwardBootloaderISR. 
 * 
 * Examples of some available symbol names (assuming the bootloader implements 
 * equivalent ISRs) are:
 *      EZBL_FORWARD_MASK_T1
 *      EZBL_FORWARD_MASK_U2TX
 *      EZBL_FORWARD_MASK_U2RX
 *      EZBL_FORWARD_MASK_SI2C1
 *
 * The symbol names are always of the form EZBL_FORWARD_MASK_[ISR_NAME], where 
 * [ISR_NAME] corresponds to an abbreviated form of the compiler recognized ISR 
 * name. For example, the ISR corresponding to EZBL_FORWARD_MASK_U2TX is called 
 * _U2TXInterrupt(). Mask symbol names are abbreviated from the ISR name by 
 * removing the leading underscore and trailing "Interrupt" token.
 * 
 * To gain access to these symbol values while writing your bootloader or 
 * application code, you can use the EZBL_GetSymbol() or EZBL_GetWeakSymbol() 
 * macros. EZBL_GetWeakSymbol() will return 0 if there is no forwarding mask bit 
 * for the given symbol name (i.e. no such ISR in the bootloader).
 * 
 * To forward the U2RX Interrupt to the application, you may write 
 * your "begin forwarding" code as:
 *      <code>
 *      #include "ezbl_integration/ezbl.h"
 *      ...
 *      {
 *          unsigned long isrForwardMaskBit;
 * 
 *          EZBL_GetWeakSymbol(isrForwardMaskBit, EZBL_FORWARD_MASK_U2RX);
 *          EZBL_ForwardBootloaderISR |= isrForwardMaskBit; 
 *          ...
 *      </code>
 * 
 * To terminate forwarding and let the bootloader handle the U2RX interrupt, 
 * you would clear the same bit without affecting the others:
 *      <code>
 *          EZBL_ForwardBootloaderISR &= ~isrForwardMaskBit;
 *      </code>
 */
extern volatile unsigned long __attribute__((near, persistent)) EZBL_ForwardBootloaderISR;


/**
 * Flash program location (address set in linker script) for keeping track of
 * erase/installed application success. This ROM memory address goes to 0xFFFF
 * (erased) when programming needed; Write to EZBL_APP_INSTALLED (0x12CD)
 * when application installed and ready. Other values besides EZBL_APP_ERASED 
 * (0xFFFF) should be treated as suspicious (possible erase interruption,
 * warranting another erase).
 */
extern __prog__ unsigned int __attribute__((space(prog))) EZBL_appBootloadState;
#define EZBL_APP_INSTALLED       0x12CDu
#define EZBL_APP_ERASED          0xFFFFu



/******************************************************************************/
/* The documentation and prototypes below are for functions typically defined */
/* and implemented in your bootloader project, NOT the ezbl_lib16.a or        */
/* ezbl_lib16ep.a archive libraries. These are included in ezbl.h for         */
/* convenience of retrieving the needed prototype and documentation when      */
/* calling them in a bootloaded application                                   */
/******************************************************************************/

/**
 * Flag for tracking if the current bootloader function was called from within
 * the bootloaded application, or from within the bootloader itself. Value is 0 
 * when the Bootloader starts up and is changed to 1 just before dispatching 
 * execution to the Application.
 */
extern unsigned int EZBL_appIsRunning;


/**
 * unsigned int EZBL_BootloaderComTask(unsigned int allowSwitchToAutoBaudMode);
 *
 * Bootloader function for processing incoming commands via the communications
 * interface. This automatically checks for any pending RX data, and if none,
 * returns immediately with the value 0x0100, doing nothing. This function
 * should be called from the running application if it wants to use the
 * bootloader without issuing a device reset.
 *
 * If a command is pending, the command is removed from the FIFO and executed.
 * This will normally automatically generate queued TX data in response. The TX
 * data is written into the TX COM FIFO for asynchronous transmission via the
 * communications ISR, so this function will not normally block. Blocking will
 * only occur when more data is generated than can currently fit in the TX FIFO.
 * In such cases, this function will block only until enough data is transmitted
 * via the ISR to allow all remaining generated data to fit in the TX FIFO.
 *
 * Some remote initiated commands, like the 'MCHPb' bootload request will cause
 * the application (if present) to be erased. If this function is called from
 * your application and this happens, then this EZBL_BootloaderComTask()
 * will gracefully not return and instead handle the full bootload operation
 * until complete. The device will then reset so the new application can run (if
 * successful), or so that the rest of the bootloader can take over for future
 * retries (if unsuccessful).
 *
 * This function should be called periodically to keep the bootloader alive and
 * always ready to receive new firmware. Max time between successive calls 
 * should be less than 1 second (if auto-baud is being used) as long delays will 
 * place the state machine back into an auto-baud state and interfere with 
 * bootloader discovery by an external host wishing to update us.
 *
 * If the bootloader's communications or Timer 1 interrupt are forwarded into
 * the application via the EZBL_ForwardBootloaderISR flags variable, this
 * function should no longer be called. This will put the bootloader into a
 * dormant state. Writing 0x00000000 to EZBL_ForwardBootloaderISR and
 * resuming periodic calls to this function will return the bootloader to a
 * ready state. If using this mechanism, be sure that the Timer 1 and
 * communications peripheral settings are still valid and that the associated
 * Interrupt Enables are still set.
 *
 * This function is not reentrant safe, so don't call it from an ISR and the
 * main() context of your application simultaneously. However, it should work
 * correctly if called from an ISR that is set to a lower priority than the
 * Timer 1 and communications channel interrupts.
 *
 * NOTE: This function is not in the ezbl_lib16.a or ezbl_lib16ep.a archive 
 * libraries. You must declare and implement this function in your bootloader
 * project in order to be able to call this function.
 * 
 * @param allowSwitchToAutoBaudMode Flag indicating if the bootloader is allowed 
 *                                  to use the auto-baud feature. When using 
 *                                  communications mediums that do not need or 
 *                                  support auto-baud, then this parameter is 
 *                                  ignored.
 *                                  0 = do not change auto-baud feature. If
 *                                      already enabled, it will remain enabled
 *                                      until completed by hardware. If not
 *                                      enabled, the baud rate will stay fixed
 *                                      at the same baud rate setting.
 *                                  1 = bootloader is allowed to turn on
 *                                      auto-baud. This will very likely
 *                                      interfere with application code if it
 *                                      needs to share the communications
 *                                      channel.
 *
 * @return 16-bit unsigned integer who's upper and lower bytes define the
 *         meaning:
 *         0x0100 = No commands waiting; nothing took place, except auto-baud
 *                  control, if allowed.
 *         0x80xx = Successfully read and executed a bootloader command
 *                  character. The lower 8-bits specify which command character
 *                  was processed.
 *         0x00xx = Unknown command code read. The lower 8-bits specify which
 *                  command character was received unexpectedly.
 *         Other values are undefined and will not be returned in this
 *                  bootloader version, but should be treated as reserved as
 *                  future versions could define them.
 *         No return = Generally means that a bootload took place and the device
 *                  will reset instead. The appearance of no return is also
 *                  possible if the the communications medium is synchronous
 *                  (ex: I2C slave) and the TX FIFO is full. As an I2C slave,
 *                  TX data can only be sent when the I2C master issues a read
 *                  request. Therefore, this case would actually indicate an
 *                  indefinite wait for the I2C master, which, when it occurs,
 *                  will then cause a return to occur normally.
 */
unsigned int EZBL_BootloaderComTask(unsigned int allowSwitchToAutoBaudMode);

/**
 * Copies the specified number of bytes out of the EZBL COM RX software FIFO and
 * into the specified destination memory buffer. Software protocol data 
 * byte requests, if applicable, are transmitted to implement flow control
 * and not overrun our hardware ability to receive.
 *
 * In the event no new characters arrive for a period longer than the specified
 * timeout (in milliseconds), the function aborts the read and returns all bytes
 * that were read until the timeout occurred.
 *
 * This function can optionally compute or update a CRC32 for each of the bytes
 * read.
 *
 * NOTE: This function is not in the ezbl_lib16.a or ezbl_lib16ep.a archive 
 * libraries. You must declare and implement this function in your bootloader
 * project in order to be able to call this function.
 * 
 * @param *dest pointer to RAM memory to write the incoming RX data to.
 *
 * @param bytesToRead Number of bytes that should be read from the RX channel
 *                    before returning. At least this many bytes of RAM must be
 *                    allocated at the *dest memory location.
 * @param *crc32      Pointer to a CRC32 value to which the data read should be
 *                    added. If starting a new CRC, the CRC's initial value
 *                    should generally be set to 0x00000000. 
 * 
 *                    This parameter is optional and can be set to null if no
 *                    CRC computation is needed.
 *
 *                    If the RX data stream is encrypted, this CRC is applied to
 *                    the actual RX data received, before performing decryption.
 * @millisecondTimeout Number of milliseconds of elapsed silence to wait for
 *                     more RX characters before giving up and returning
 *                     everything that was received.
 *
 *                     This timing parameter is with respect to the most
 *                     recently received character, not the time from when the
 *                     function was called. Therefore, on slow communications
 *                     mediums or when reading large blocks of data, this
 *                     function may block for a long time. The timeout need only
 *                     be adjusted for the medium's worst case latency, not for
 *                     the ratio of the baud rate/bandwidth to the bytesToRead.
 *
 * @return Number of characters written to *dest. For successful operation, the
 *         return value will match the bytesToRead parameter passed in. If
 *         timeout occurs, the return value will be smaller than the bytesToRead
 *         passed in.
 */
unsigned int EZBL_COMReadWithTimeout(void *dest, unsigned int bytesToRead, unsigned long *crc32, unsigned int millisecondTimeout);

/**
 * signed int EZBL_InstallBlobFromCOM(unsigned int millisecondTimeout);
 *
 * Reads a .blob file from the communications medium and decodes it, decrypts it
 * (if applicable), and then programs the device Flash with the decoded .blob
 * contents. This function must be called when the very next character that will
 * come out of the RX communications channel is the first byte of a .blob file.
 *
 * This function will block until either the whole .blob file is received and
 * successfully programmed or anytime earlier if something goes wrong.
 *
 * Before calling this function, the device (application space) Flash should be
 * in an erased state. Erasing can be handled externally via the
 * EZBL_EraseAll() API or EZBL_EraseAllPiecewise() if granular chunked
 * erases are desired (for status reporting, com timeout management, ISR
 * performance, etc.).
 *
 * If the Flash is not erased, but the .blob file only has data records for
 * Flash addresses that are all erased, then it is still legal to call this
 * function. Doing so will allow more data to be programmed independently of the
 * original application. Although no demo exists to mange this or help create
 * suitable .blob's for this purpose, this could be used as a basis to create a
 * multi-image project. For example: keep presently installed application
 * installed and unmodified while bootloading new firmware to unused Flash so
 * there is never a possibility of bootload failure causing a temporary brick.
 * (Bootload failure is very unlikely to damage the bootloader, however, so
 * application recovery will generally always be feasible without performing
 * multi-image operations).
 *
 * Because reception, decryption and programming of a whole .blob file could
 * take an extended period, this function internally calls the ClrWdt()
 * function to avoid possible unintended device reset. As communications
 * operations do not have deterministic timing, you must NOT enable the watchdog
 * timer in a Windowed mode. Only traditional watchdog timing will work. Also,
 * this function will not work if the watchdog timeout is too short. The
 * watchdog is only cleared when a chunk of data is received and processed,
 * which will be up to sizeof(programBuffer) bytes long (384 byte by default).
 * Additional time may be needed for the programming operations.
 * 
 * NOTE: This function is not in the ezbl_lib16.a or ezbl_lib16ep.a archive 
 * libraries. You must declare and implement this function in your bootloader
 * project in order to be able to call this function.
 * 
 * @param millisecondTimeout A timer to ensure communications interruptions
 *                           won't stall indefinitely so that a retry can be
 *                           attempted. This timeout is with respect to
 *                           contiguous com silence, not for the time it takes
 *                           to perform the full .blob reception and
 *                           programming. I.e. any time the internal state
 *                           advances, the timer is reset. Set this value from
 *                           1 to 65535 milliseconds such that the timeout is
 *                           longer than the worst case communications latency.
 *
 * @return Signed integer error code. Generally zero or greater is good, while
 *         negative values indicate a failure. In all failing cases, the
 *         application will not be flagged as runnable, so subsequent device
 *         reset will keep execution within the bootloader, available for retry.
 *         Anything programmed prior to a failure is not erased or otherwise
 *         clean up.
 *
 *         Specific return code meanings are:
 *          1 = Programming of the full .blob completed successfully. The .blob
 *              file and communications integrity was confirmed with a valid
 *              CRC32, and all data programmed passed read-back verification.
 *         -1 = Communications timeout waiting for the .blob header
 *         -2 = Communications timeout waiting for a record header
 *         -3 = .blob or record header contains illegal size information
 *              (.blob corrupt or incorrect RX data)
 *         -4 = Record header contained illegal address information
 *              (.blob corrupt or incorrect RX data)
 *         -5 = Communications timeout waiting for record data
 *         -6 = Communications timeout waiting for final 4 byte CRC
 *         -7 = CRC mismatch (.blob corrupt or incorrect RX data)
 *         -8 = Read back verify failure (.blob record targeted an address that
 *              wasn't erased, isn't implemented on this device, or wasn't
 *              otherwise correctly programmed). If the blob is known to be good
 *              and built for this bootloader, check device voltage, proper
 *              operating frequency, and that no ISR code bugs could potentially
 *              have corrupted RAM.
 *         No return = Unhandled exception or unknown. (.blob may have
 *              records targeting unimplemented memory, causing an Address
 *              Error Trap upon verification, or electrical operating parameters
 *              are incorrect.) Ensure the .blob file was compiled specifically
 *              for this device and bootloader. For unhandled exceptions, the
 *              device will automatically reset back into the bootloader.
 */
signed int EZBL_InstallBlobFromCOM(unsigned int millisecondTimeout);

// Common error code definitions used for communications and .blob file parsing. 
// These are typically returned from the EZBL_InstallBlobFromCOM() bootloader 
// function and passed back to the master host for display to a user or for 
// logging.
#define EZBL_ERROR_SUCCESS                  (1)     // Operation completed successfully.
#define EZBL_ERROR_TIMEOUT_IN_BLOB_HEADER   (-1)    // Communications timeout attempting to read the first 4 bytes of the .blob file (where the file's length is contained)
#define EZBL_ERROR_TIMEOUT_IN_RECORD_HEADER (-2)    // Communications timeout attempting to read a record header from the .blob file
#define EZBL_ERROR_ILLEGAL_LENGTH           (-3)    // Communications corruption occurred or the .blob file contains an illegally long length field in a data record or the overall .blob header. The PIC24/dsPIC architecture only has 24-program memory address bits, and no record should be anywhere near that big in practice due to unimplemented memory regions or unprogrammed locations causing data to split into two separate records.
#define EZBL_ERROR_ILLEGAL_RECORD_ADDRESS   (-4)    // Communications corruption occurred or the .blob file contains an illegally high record address. The PIC24/dsPIC architecture only has 24-program memory address bits.
#define EZBL_ERROR_TIMEOUT_IN_RECORD_DATA   (-5)    // Communications timeout trying to read .blob record data.
#define EZBL_ERROR_TIMEOUT_IN_CRC           (-6)    // Communications timeout reading last 4 byte CRC field.
#define EZBL_ERROR_BLOB_CRC                 (-7)    // CRC of received .blob contents mismatch with CRC contained in .blob. Probable communications corruption.
#define EZBL_ERROR_READ_BACK_VERIFICATION   (-8)    // Read-back verification mismatch. Probable configuration error or write protected memory.
#define EZBL_ERROR_BOOTLOADER_MISMATCH      (-9)    // Read-back verification mismatch. All programming completed, but data in the existing bootloader does not match the bootloader copy in the uploaded image. Make sure you transmitted a correct .hex/.blob file that exactly matches and was built for the installed bootloader. The Application must be compiled with _merge.s and _merge.gld files generated when the bootloader was originally built and deployed.


// 32-bit constant symbols that can change only when you change the target
// processor. Use EZBL_SYM32(&symbol) to obtain these, or more optimally, the
// EZBL_GetSymbol() macro.
extern __pack_upper_byte int EZBL_CONFIG_BASE;       // Ex: 0x0557EC
extern __pack_upper_byte int EZBL_CONFIG_END_ADDRESS;// Ex: 0x055800
extern __pack_upper_byte int EZBL_DEVID_MASK;        // Ex: 0xFFFF0000, mask for the DEVID portion of the DEVID register; Seems to be 16-bit endian swapped.
extern __pack_upper_byte int EZBL_DEVID_VALUE;       // Ex: 0x1BCF4000, value that should exist for the device selected in MPLAB when compiling the project; Seems to be 16-bit endian swapped.


/**
 * <code><b>
 * int NOW_PreSleep(unsigned long expectedSleepDuration);
 * </b></code>
 * 
 * User callback function that is called anytime NOW_Sleep() is called 
 * elsewhere in the system. Implement this function in order to do any shutdown 
 * or peripheral reconfiguration before the processor goes to sleep or 
 * optionally, reject the sleep request altogether. 
 * 
 * For example, you can implement this callback to ensure all UART/SPI/I2C 
 * communications are in an idle state so data doesn't get truncated nor do you 
 * stall any other devices that you were in the middle of transmitting to. You 
 * can also perform other application specific operations to reduce power, like 
 * turn off a power supply to other board level components.
 * 
 * This function is linked as an undefined weak symbol, so is optional. If you 
 * do not define this callback function, NOW_Sleep() will put the processor to 
 * sleep without calling anybody first.
 * 
 * This function executes in the same CPU IPL context as NOW_Sleep() was called 
 * in. This could be either main(), or any ISR level, depending on your code. 
 * Caution is advised as it could even be issued from both places at the same 
 * time in a unintentionally recursive or reentrant loop. 
 * Ex: - You call NOW_Sleep() from main()
 *     - NOW_Sleep() calls NOW_PreSleep() callback in the main() context
 *     - An Interrupt is triggered before you return from NOW_PreSleep()
 *     - ISR also calls NOW_Sleep()
 *     - NOW_Sleep() again calls NOW_PreSleep(), now in an Interrupt context
 * 
 * @param expectedSleepDuration
 *      Number of NOW counts the sleep request was for. Use NOW_second, 
 *      NOW_millisecond, or NOW_microsecond to decode this meaning.
 * 
 * @return 
 *      <p>Return >= 0 to approve the sleep operation.
 *      <p>Return < 0 to reject the sleep operation.
 */
extern int NOW_PreSleep(unsigned long expectedSleepDuration);

/**
 * <code><b>
 * extern void NOW_PostSleep(unsigned long timeAsleep);
 * </b></code>
 * 
 * User callback function that is called anytime NOW_Sleep() is called 
 * and we have awoken and are about to return. Implement this function in 
 * order to do any peripheral restart or reconfiguration in a central location.
 * 
 * This function is linked as an undefined weak symbol, so is optional. If you 
 * do not define this callback function, NOW_Sleep() will return without calling 
 * anybody first.
 * 
 * This function executes in the same CPU IPL context as NOW_Sleep() was called 
 * in, and is complementary to the NOW_PreSleep() callback function.
 * 
 * If NOW_PreSleep() returns a cancellation value for the sleep request, this 
 * callback is NOT called.
 * 
 * @param timeAsleep
 *      Number of NOW counts that we actually spent sleeping. Because interrupts 
 *      could have woken the CPU, executed asynchronously, and then NOW_Sleep() 
 *      put the CPU back to sleep for the requested duration, the timeAsleep 
 *      value will normally be shorter than the original expectedSleepDuration 
 *      given in the NOW_PreSleep() callback, or requested when NOW_Sleep() was 
 *      originally called. The minimum value is 0, which will occur if the 
 *      amount of cumulative time spent in the NOW_PreSleep() and NOW_Sleep() 
 *      functions executing overhead operations ends up being equal or longer 
 *      than the requested NOW_Sleep() duration.
 * 
 *      Use NOW_second, NOW_millisecond, or NOW_microsecond to decode 
 *      parameter's meaning.
 */
extern void NOW_PostSleep(unsigned long timeAsleep);

/**
 * NOW_second
 *
 * Number of NOW counts per second of physical time (assuming the NOW_Init()
 * function is called with the running device clock frequency).
 *
 * If run time clock switching occurs externally and you are using the system or 
 * peripheral clock for the timing reference to NOW, you must call the 
 * NOW_SetTimeIntervals() function to recompute this variable. Any NOW APIs 
 * which include clock switching internally do not carry this requirement since 
 * they will automatically update NOW_microsecond, NOW_millisecond, and 
 * NOW_second for you.
 *
 * This variable is the same value as the device clock frequency, so results in
 * no rounding or truncation error.
 *
 * Note that this is an unsigned long value. You must up-cast this to an
 * (unsigned long long) if you are running at a high clock frequency and want to
 * compute an interval that would overflow a 32-bit integer. For example, if the
 * device clock is 70.0MHz, NOW_second would be 70,000,000. If you want to
 * compute something that is two minutes (120 seconds), the correct number of
 * NOW counts would be 8,400,000,000, which can't fit in a 32-bit variable. For
 * this clock speed, any calculation (including intermediate ones) requiring more
 * than 61.356 seconds will be invalid without an initial cast to 64-bits.
 */
extern unsigned long NOW_second;

/**
 * NOW_millisecond
 *
 * Number of NOW counts per millisecond of physical time (0.001 seconds,
 * assuming the NOW_Init() function is called with the running device clock
 * frequency).
 *
 * If run time clock switching occurs externally and you are using the system or 
 * peripheral clock for the timing reference to NOW, you must call the 
 * NOW_SetTimeIntervals() function to recompute this variable. Any NOW APIs 
 * which include clock switching internally do not carry this requirement since 
 * they will automatically update NOW_microsecond, NOW_millisecond, and 
 * NOW_second for you.
 *
 * Note that this variable is an integral number, rounded to result in the least
 * amount of error, either positive or negative. Ex: if the clock
 * frequency is 32.768kHz, one NOW count would represent 1/32768Hz or
 * 0.030517578125 milliseconds. Therefore, NOW_millisecond, the number of NOW
 * counts/millisecond, would be 32.768 if precision were infinite. Since
 * rounding instead of truncation occurs, this will result in NOW_millisecond
 * returning 33 instead of 32 (-0.708% error instead of +2.344% error).
 */
extern unsigned long NOW_millisecond;

/**
 * <code><b>
 * unsigned short NOW_microsecond;
 * </b></code>
 *
 * Number of NOW counts per microsecond of physical time (0.000001 seconds,
 * assuming the NOW_Init() function is called with a suitably fast timer clock
 * such has microsecond resolution). 
 * 
 * 31/32kHz LPRC and 32.768kHz SOSC clocks have a period of over 30us, so DO NOT 
 * USE NOW_microsecond with such clocks. The value would always be zero! Use a 
 * less accurate NOW_millisecond or accurate NOW_second reference instead.
 *
 * If run time clock switching occurs externally and you are using the system or 
 * peripheral clock for the timing reference to NOW, you must call the 
 * NOW_SetTimeIntervals() function to recompute this variable. Any NOW APIs 
 * which include clock switching internally do not carry this requirement since 
 * they will automatically update NOW_microsecond, NOW_millisecond, and 
 * NOW_second for you.
 *
 * Note that this variable is an integral number, rounded to result in the least
 * amount of error, either positive or negative. Ex: if the clock
 * frequency is 1.500001MHz, one NOW count would represent 1/1500001Hz or 0.6667
 * microseconds. Therefore, NOW_microsecond, the number of NOW
 * counts/microsecond, would be 1.500001 if precision were infinite. Since
 * rounding instead of truncation occurs, this will result in NOW_microsecond
 * returning 2 instead of 1 (-33.33324% error instead of +33.33338%).
 *
 * Be very cautious using this value if your clock frequency is under 500kHz. In
 * this case, 0 will be returned, which could cause a divide by zero exception 
 * when divided or 100% error condition when multiplied.
 */
extern unsigned short NOW_microsecond;

/**
 * <code><b>
 * unsigned long NOW_systemFrequency;
 * </b></code>
 *
 * System instruction clock frequency, in Hz. Normally this will match 
 * NOW_second, but this value could differ if the NOW Timer clock source is 
 * different from the system execution clock. Value is set when NOW_Init() is 
 * called.
 */
extern unsigned long NOW_systemFrequency;



/**
 * <code><b>
 * void NOW_Init(unsigned long timerInputFrequency);
 * </b></code>
 *
 * Initializes a Timer for time keeping with the NOW_* API. Timing is 
 * implemented relative to the timer's selected input clock frequency. This 
 * function uses this value to sets the values stored in NOW_second, 
 * NOW_millisecond and NOW_microsecond for direct use in calling code to 
 * translate run-time adjustable NOW counts to physical seconds and other human 
 * meaningful times.
 *
 * This function defaults to enabling the Timer/CCP Interrupt at priority 
 * level 4. You can modify this value. The interrupt will fire every 65536 
 * instructions and need around ~15 or 30 cycles per interrupt event on 16-bit 
 * processors. i.e. less than 0.05% CPU use.
 *
 * Most of the NOW APIs should only be called at IPL levels below this ISR
 * priority if accurate and monotonic NOW counts are needed. If you wish to use
 * these APIs in higher priority ISRs, increase the timer ISR priority level
 * to one higher than the highest caller.
 *
 * @param timerInputFrequency 
 *          Number of input timer clock cycles per second of real time. For 
 *          example, use 70000000 if your device is operating at 70 MIPS and you 
 *          are using the system instruction clock to clock the timer. LPRC's 
 *          will need ~31000 for 31kHz, and of course 32.768kHz SOSC crystals 
 *          are also normally used with 32768. 
 * 
 *          However, if you know your crystal or clock input is biased above or 
 *          below the optimal crystal spec, you can deliberately provide a value 
 *          slightly faster or slower to automatically correct for this 
 *          oscillation frequency error in software computations. At room 
 *          temperature, crystals will oscillate slower than intended when you 
 *          have oversized crystal loading capacitors (ex: more parasitic pin 
 *          capacitance than you calculated for), and similarly, run fast when 
 *          there is below optimal capacitance.
 */
void NOW_Init(unsigned long timerInputFrequency);


/**
 * <code><b>
 * void NOW_SetTimeIntervals(unsigned long timerInputFrequency);
 * </b></code>
 *
 * Updates the NOW_second, NOW_millisecond and NOW_microsecond variables to
 * correspond to the specified timer input clock frequency.
 *
 * @param timerInputFrequency 
 *          Number of timer input clock cycles per second. For example, use 
 *          70000000 if your device is operating at 70 MIPS and using the system 
 *          clock to clock the timer.
 */
void NOW_SetTimeIntervals(unsigned long timerInputFrequency);


/**
 * <code><b>
 * unsigned long NOW_MeasureAbsoluteClock(unsigned int targetClockType, void *targetClock, unsigned int refClockType, void *refClock, unsigned long refClockFrequency, unsigned int desiredPPMPrecision, unsigned int milliTimeout);
 * </b></code>
 * 
 * Measures an unknown-frequency clock source against another known-frequency 
 * reference clock source and computes the unknown frequency.
 * 
 * @param targetClockType
 *        <p>0x0 = 16-bit Timer/CCP/Counter in an SFR or RAM 
 *                 location. *refClock will be dereferenced as a 
 *                 volatile unsigned short to get "tick" 
 *                 values. In the case of a RAM location, an 
 *                 external ISR of higher priority than the present 
 *                 CPU IPL must write the new values into the given 
 *                 address as this function does not return until 
 *                 the DCO measurements are complete.
 *        <p>0x1 = 32-bit Timer/CCP/Counter in an SFR or RAM. 
 *                 *refClock will be dereferenced as a 
 *                 volatile unsigned long to get "tick" values.
 *        <p>0x2 = 16-bit "tick" via function pointer return.
 *                 *refClock must point to an executable function 
 *                 that takes no parameters and will return an 
 *                 incrementing 16-bit integer type (unsigned int, 
 *                 signed int, unsigned short, unsigned short int, 
 *                 signed short, etc.). Generally, it is possible 
 *                 to use this value even for 32-bit return valued 
 *                 functions, but in such a case, the upper 16-bits 
 *                 of information will always be discarded.
 *        <p>0x3 = 32-bit "tick" via function pointer return.
 *                 *refClock must point to an executable function 
 *                 that takes no parameters and will return an 
 *                 incrementing 16-bit integer type (unsigned int, 
 *                 signed int, unsigned short, unsigned short int, 
 *                 signed short, etc.).
 * 
 * @param targetClock       Pointer to the specified clock resource. See 
 *                          targetClockType parameter definition for exact 
 *                          meaning of this pointer.
 * 
 * @param refClockType      Same as targetClockType, but for the known-frequency 
 *                          reference clock.
 * 
 * @param refClock          Same as targetClock, but for the known-frequency 
 *                          reference clock.
 * 
 * @param refClockFrequency Frequency of the known-frequency refClock, in Hz.
 * 
 * @param desiredPPMPrecision   
 *                          Desired precision in measuring the target clock
 *                          frequency, in units of PPM (Parts Per Million). 
 *                          1000 PPM is equivalent to +/-0.1% clock error 
 *                          (1000/1000000 * 100%). 
 * 
 *                          Higher precision takes longer to measure, 
 *                          potentially with a Gaussian tail, meaning a LOT 
 *                          longer. 
 *                          
 *                          NOTE: if either the reference, and to a lesser 
 *                          extent, target clock frequency is unstable and 
 *                          drifts faster than the desired precision can be 
 *                          achieved, this function could block forever. To 
 *                          avoid this, specify a timeout.
 * 
 * @param milliTimeout      Maximum number of milliseconds allowed to try and 
 *                          achieve the desiredPPMPrecision criteria. To disable 
 *                          the timeout, specify 0.
 * 
 * @return  Present frequency of the targetClock, in cycles-per-second (Hz). If an 
 *          unrecognized parameter is given, 0xFFFFFFFF is returned.
 */
unsigned long NOW_MeasureAbsoluteClock(unsigned int targetClockType, void *targetClock, unsigned int refClockType, void *refClock, unsigned long refClockFrequency, unsigned int desiredPPMPrecision, unsigned int milliTimeout);


/**
 * <code><b>
 * unsigned short NOW_16(void);
 * </b></code>
 *
 * Atomically returns the least significant 16 bits of the current NOW counter
 * value. This function is safe to be called from all ISRs (and main context), 
 * but only when the ISR priority is at least one higher than the Timer's ISR 
 * priority. This function is reentrant capable.
 *
 * Execution time is 5 or 10 cycles on 16-bit PIC24 or dsPIC processors, 
 * depending on device family. This includes the call and return branches.
 *
 * @return 16-bit NOW counter value, where 1 NOW count is equal to clock cycle 
 *         of the reference timer input. Use the NOW_second, NOW_millisecond, 
 *         NOW_microsecond variable contents, or the NOW_Diff_to_ms() and 
 *         NOW_Diff_to_us() functions in order to make physical sense of how 
 *         long a NOW count is.
 *
 *         Because this return value is only 16-bits wide and could increment as 
 *         at up to the system device frequency, some clock sources will 
 *         overflow 16-bits very easily. Ex: at 70MIPS and the CPU clock used 
 *         for the NOW timer, the longest interval you could correctly measure 
 *         with this function is 936us. Use the NOW_32() or NOW_64() functions 
 *         if longer durations are being measured.
 */
unsigned short NOW_16(void);


/**
 * <code><b>
 * unsigned long NOW_32(void);
 * </b></code>
 *
 * Atomically returns the least significant 32 bits of the current NOW counter
 * value. This function is safe to be called from the main contexts and ISRs
 * of lower priority than the TMR1 ISR priority (IPL=1,2 or 3 by default).
 * Calling from a higher or equal ISR priority context will succeed and cause
 * no harm, but the value returned may be off by up to 0x10000 counts if the
 * lower 16 bit count rolls over.
 *
 * Execution time is 12 or 20 cycles best (and normal) case, depending on device
 * family, which includes the call and return branches. If you are very unlucky
 * and the lower 16-bits rolls over during the read (less than 0.013% chance if
 * no interrupt occurs), the final return value will still be atomic, but
 * execution time will approximately double.
 *
 * @return 32-bit NOW counter value, where 1 NOW count is equal to 1 instruction
 *         cycle. Use the NOW_second, NOW_millisecond, NOW_microsecond
 *         variable contents, or the NOW_Diff_to_ms() and NOW_Diff_to_us()
 *         functions in order to make physical sense of how long a NOW count is.
 *
 *         Because this return value is only 32-bits wide and increments at the
 *         system device frequency, it is subject to overflow when working with
 *         longer intervals . Ex: at 70MIPS, the longest interval you could
 *         correctly measure with this function is 61.356 seconds. Use the
 *         NOW_64() function if longer durations are being measured.
 */
unsigned long NOW_32(void);

/**
 * <code><b>
 * unsigned long long NOW_64(void);
 * </b></code>
 *
 * Atomically returns the entire 64 bits of the current NOW counter value
 * value. This function is safe to be called from the main context and ISRs
 * of lower priority than the TMRx ISR priority (IPL=1,2 or 3 by default).
 * Calling from a higher or equal ISR priority context will succeed and cause
 * no harm, but the value returned may be off by up to 0x100000000 counts if the
 * lower 32 bit count rolls over.
 *
 * Execution time is 20 or 32 cycles best (and normal) case, depending on device
 * family, which includes the call and return branches. If you are very unlucky
 * and the lower 16-bits rolls over during the read (less than 0.013% chance if 
 * no interrupt occurs), the final return value will still be atomic, but
 * execution time will approximately double.
 *
 * @return 64-bit NOW counter value, where 1 NOW count is equal to 1 instruction
 *         cycle. Use the NOW_second, NOW_millisecond, NOW_microsecond
 *         variable contents, or the NOW_Diff_to_ms() and NOW_Diff_to_us()
 *         functions in order to make physical sense of how long a NOW count is.
 *
 *         Because this return value is a whopping 64-bits, the returned value
 *         will be virtually immune to overflow. Ex: at 1GHz (I want that PIC!),
 *         overflow won't occur for over 584 years. As a result, this NOW count
 *         can also be used for absolute time measurement. Do do so, just
 *         initialize NOW_internalCount to the present time (relative to
 *         whatever date/time you consider Time T=0) before calling NOW_Init().
 *         If you do not initialize NOW_internalCount, the default will be
 *         0x0000000000000000, so NOW_64() will still be usable as an absolute
 *         timestamp, but values returned will be relative to the last device
 *         reset.
 */
unsigned long long NOW_64(void);

/**
 * <code><b>
 * unsigned long NOW_Diff_to_ms(unsigned long nowCounts);
 * </b></code>
 *
 * Converts the difference between two NOW counts to absolute time in
 * milliseconds. This function is primarily intended for instances where you
 * wish to display a time interval to a human in decimal forms. It is, however,
 * not subject to the integral approximations that NOW_millisecond represent, so
 * can be used in other cases when absolute accuracy is critical and the device
 * operating frequency is very low (ex: 32.768kHz).
 *
 * nowCounts must be measured against the same clock frequency in use during
 * invocation of this function. In other words, a wrong result will be returned
 * if your collect two NOW counts at one clock frequency, do a run-time clock 
 * switch, call NOW_SetTimeInterval(), and then pass the difference of the
 * original counts in for the nowCounts parameter.
 *
 * This function can be called from any CPU IPL and is reentrant capable.
 *
 * This function requires two 32x32 unsigned integer divide operations and
 * therefore requires a fair amount of execution time relative to most other
 * NOW APIs. If you just need to compare two timestamps where one value
 * represents the present and another value represents a constant timeout, it is
 * likely more efficient to use the NOW_second and NOW_millisecond variables.
 *
 * @param nowCounts The number of NOW counts to convert. Acceptable range is 0
 *                  to 536,870,911 counts (or up to 2,684 ms @ 200MHz; longer
 *                  for slower clock frequencies). Specifying
 *                  nowCounts > 536,870,911 will return an undefined value. The
 *                  function will safely succeed, however.
 *
 * @return 32-bit unsigned long representing complete milliseconds elapsed.
 *         Rounding is not performed; partial milliseconds are truncated off.
 */
unsigned long NOW_Diff_to_ms(unsigned long nowCounts);

/**
 * <code><b>
 * unsigned long NOW_Diff_to_us(unsigned long nowCounts);
 * </b></code>
 *
 * Converts the difference between two NOW counts to absolute time in
 * microseconds. This function is primarily intended for instances where you
 * wish to display a time interval to a human in decimal forms. It is, however,
 * not subject to the integral approximations that NOW_millisecond and
 * NOW_microsecond represent, so can be used in other cases when absolute
 * accuracy is critical.
 *
 * nowCounts must be measured against the same clock frequency in use during 
 * invocation of this function. In other words, a wrong result will be returned 
 * if your collect two NOW counts at one clock frequency, do a run-time clock 
 * switch, call NOW_SetTimeInterval(), and then pass the difference of the 
 * original counts in for the nowCounts parameter.
 *
 * This function can be called from any CPU IPL and is reentrant capable.
 *
 * This function requires one 32x16 unsigned integer divide plus one 32x32 
 * unsigned integer divide operations and therefore requires a fair amount of 
 * execution time relative to most other NOW APIs. If you just need to compare 
 * two timestamps where one value represents the present and another value 
 * represents a constant timeout, it may be more efficient to use the 
 * NOW_millisecond and NOW_microsecond variables. Using these variables can,
 * however, result in greater error for certain clock frequencies, so this
 * function may still be needed when absolute accuracy has to be minimized.
 *
 * @param nowCounts The number of NOW counts to convert. Acceptable range is 0 
 *                  to 67,108,863 counts (or up to 335,544 microseconds @ 
 *                  200MHz; longer for slower clock frequencies). Specifying
 *                  nowCounts > 67,108,863 will return an undefined value. The
 *                  function will safely succeed, however.
 *
 * @return 32-bit unsigned long representing complete microseconds elapsed.
 *         Rounding is not performed; partial microseconds are truncated off.
 */
unsigned long NOW_Diff_to_us(unsigned long nowCounts);

/**
 * <code><b>
 * unsigned long NOW_Sleep(unsigned long blockTime);
 * </b></code>
 *
 * Configures the NOW timer to operate asynchronously in Sleep mode and then 
 * puts the CPU to Sleep. An optional blocking time can be given to have the CPU 
 * automatically wake up and return after the given interval elapses.
 * 
 * If an enabled Interrupt wakes the CPU before the specified blockTime elapses, 
 * this function will allow the wake-up ISR to execute. When the ISR is 
 * complete, if there is still blocking time required to meet the original 
 * count, the CPU is placed back into sleep mode. The function always ensures 
 * that the at least the given blocking interval is met before returning 
 * (independent of how long the CPU cumulatively actually spent asleep).
 *
 * @param blockTime 
 *          Number of NOW counts this function must block before returning. The 
 *          blocking duration will be spent optimally sleeping, but allowing 
 *          ISRs to still wake the CPU and execute as needed. If one of the 
 *          asynchronous ISRs calls NOW_WakeUp() before the given blockTime is 
 *          met the sleep call is aborted and returns early.
 * 
 *          To sleep indefinitely unless/until signaled by NOW_WakeUp(), specify 
 *          0xFFFFFFFF for blockTime. This special value is treated as infinite 
 *          rather than an actual NOW count.
 *
 * @return 
 *          Number of NOW counts that the CPU actually spent sleeping. This 
 *          value will always be less than or equal to the given blockTime. 
 *          Unless commanded to stop sleeping early from NOW_WakeUp(), dividing 
 *          this return value and the provided blockTime and subtracting from 1 
 *          therefore represents a CPU utilization ratio, directly usable for 
 *          doing power consumption estimates.
 * 
 *          To be able to identify when the blocking interval is ended early 
 *          from NOW_WakeUp(), you should get the current NOW counter 
 *          immediately before calling NOW_Sleep(), immediately recapture the 
 *          NOW counter after this function returns, and then subtract the two. 
 *          For example:
 *          <code>
 *          unsigned long   sleepStart, sleepCount, trueBlockingCount;
 *          double          cpuUtilizationPercent;
 * 
 *          sleepStart = NOW_32();                      // Get starting timestamp
 *          sleepCount = NOW_Sleep(NOW_Second/4);       // Sleep for 250ms
 *          trueBlockingCount = NOW_32() - sleepStart;  // Compute time difference
 *          cputUtilizationPercent = ((double)(sleepCount*100u))/trueBlockingCount;
 *          </code>
 */
unsigned long NOW_Sleep(unsigned long blockTime);




/******************************************************************************/
/* Everything below this line is deprecated and not documented or actively    */
/* tested. Use at your own risk!                                              */
/******************************************************************************/

// Old multi-store bootloading method functions. Do not use these. These are included only for compatibility with existing project, but may be removed in the future if existing uses are ported over to use the normal programming APIs.
void EZBL_EraseAll(void);                                        // Reserved, do not call in new code: Prototype for old programming code and may be removed in the future. To erase all Flash contents, see EZBL_EraseAllPiecewise(). Must set EZBL_NVMKey = 0xFC21 (NOT 0x03DF!) before calling this function.
signed int EZBL_SectorIsClear(unsigned long programAddress);     // Reserved, do not call in new code: Slower instruction-by-instruction sector clear checking; Most likely, you should call EZBL_IsPageErased() instead. This function, however, does return clear for a page with only the Reserved Bit programmed, or the goto @ address 0 on page 0 programmed.
void __attribute__((weak)) EZBL_MultistoreReset(void);                          // Reserved, do not call in new code: Prototype for old programming code and may be removed in the future. Specify EXTERN(_EZBL_MultistoreReset); in linker script to enable the multi-store bootloader function
unsigned int EZBL_ReadStoreBytes(void *dest, unsigned int storeID, unsigned long readByteOffset, unsigned int readLength);  // Reserved, do not call in new code: Prototype for old programming code and may be removed in the future.
signed short EZBL_Command(unsigned short operation, unsigned short storeID, void *inOutData, unsigned long inOutDataLen);   // Reserved, do not call in new code: Prototype for old programming code and may be removed in the future.


// storeID 0 is defined as the active program store, which is never encrypted, and always at a constant, known address. It is the one intended for program execution and the target of INSTALL_STORE operations.
// storeID 1 to n are inactive file stores, which can contain anything, but normally will be encrypted program images

#define EZBL_OP_ADD_STORE            1u // Reserved, do not use in new code: 'operation' parameter constant that can be passed to the EZBL_Command() function. Create a new program store. The new storeID is returned on success. Input parameters storeID, *inOutData, and inOutDataLen are ignored.
#define EZBL_OP_DELETE_STORE         2u // Reserved, do not use in new code: 'operation' parameter constant that can be passed to the EZBL_Command() function. Deletes a program store. *inOutData and inOutDataLength are ignored.
#define EZBL_OP_INSTALL_STORE        3u // Reserved, do not use in new code: 'operation' parameter constant that can be passed to the EZBL_Command() function. Signals to install the store contents as the active program image and reset to begin execution of it. *inOutData and inOutDataLength are ignored.
#define EZBL_OP_WRITE                4u // Reserved, do not use in new code: 'operation' parameter constant that can be passed to the EZBL_Command() function. Passes data to be written to the given store. *inOutData points to the data to write, inOutDataLength is the number of bytes to write. Note that due to underlying minimum Flash write sizes, not all data may get written. Some if it may be cached to RAM and be written later on a subsequent WRITE op. It must not be assumed that the data was successfully written since power cycling the PIC will result in loss of the unwritten cached data, as well as the absense of a journal record, if applicable.
#define EZBL_OP_GET_HASH             5u // Reserved, do not use in new code: 'operation' parameter constant that can be passed to the EZBL_Command() function. Returns a Hash of all data in the given program store. *inOutData must point to a RAM location to return the computed hash value. inOutDataLen must be 20 retrieve all bytes of the SHA-1 hash. Smaller values are legal if hash truncation is desired (such as when using the hash as a CRC). The address need-not be aligned. On success, returns the number of bytes written to *inOutData. Negative return values indicate failure.
#define EZBL_OP_ENUM_STORES          7u // Reserved, do not use in new code: 'operation' parameter constant that can be passed to the EZBL_Command() function. Enumerates all valid storeIDs programmed on the device. For the first call, set storeID = 0xFFFF (will return store0). A single storeID will be returned, or -1 if no (more) stores exist. To get the next storeID, call EZBL_OP_ENUM_STORES again, this time passing the prior return value in for the storeID parameter. The storeIDs are returned in numerical sequential order, which will differ from cronological programming order if you, for example, program three stores, erase store1, and program a 4th store, which will reuse storeID of 1. inOutData and inOutDataLen are ignored.
#define EZBL_OP_GET_ATTRIBUTES       8u // Reserved, do not use in new code: 'operation' parameter constant that can be passed to the EZBL_Command() function. Returns a store attributes bitmask for the given storeID. *inOutData must point to memory where the retrieved attributes should be written. inOutDataLen specifies the max number of bytes that can be written to *inOutData, and should be set to 4 (i.e. 32-bits) in the current EZBL version to retrieve all attributes. Values less than 4 are legal and will return a truncated bitfield. Returns number of attribute bytes written to *inOutData on sucess, or a negative error code on failure. If inOutDataLen is set greater than the number of attribute bytes that exist, the successful return result will be a count of the maximum number of attributes that exist in this EZBL version (4 bytes).
#define EZBL_OP_SET_ATTRIBUTES       9u // Reserved, do not use in new code: 'operation' parameter constant that can be passed to the EZBL_Command() function. Sets a store attributes bitmask for the given storeID. *inOutData must point to memory containing all attributes to write. Prior attributes are replaced, so read them first if only modifing one or a limited set of attributes. For the current bootloader version, set inOutDataLen to 4 (and ensure 4 bytes of memory exist at *inOutData). Returns the number of bytes written to *inOutData on success (always 4 in this version), or a negative error code on failure. Error codes include {-1 - invalid storeID; -2 - incorrect input parameters).

// Store attribute bitmasks pertaining to the EZBL_Command() EZBL_OP_GET_ATTRIBUTES and EZBL_OP_SET_ATTRIBUTES operations
#define EZBL_ATTRIBUTE_ARCHIVE       0x00000100ul   // Reserved, do not use in new code: Flag indicating a "golden" image, or one that is preferential for programming failure recovery. If store0 is ever invalid, stores with the archive flag set will be checked sequentially (by storeID) for installation into store0. It is legal to set more than one store with this attribute, but only the lowest ID version (that passes all checks) will be installed into store0 for recovery.
#define EZBL_ATTRIBUTE_ERASABLE      0x00000200ul   // Reserved, do not use in new code: Flag indicating if the store can be erased (1 = erasable; 0 = any operation that needs to erase one of this store's sectors will fail instead)
#define EZBL_ATTRIBUTE_EXECUTABLE    0x00000400ul   // Reserved, do not use in new code: Not implemented.
#define EZBL_ATTRIBUTE_INSTALLED     0x00000800ul   // Reserved, do not use in new code: Not implemented.
#define EZBL_ATTRIBUTE_STORE_INVALID 0x80000000ul   // Reserved, do not use in new code: Flag for negative failure testing. Always write as 0.


#endif	/* EZBL_H */

