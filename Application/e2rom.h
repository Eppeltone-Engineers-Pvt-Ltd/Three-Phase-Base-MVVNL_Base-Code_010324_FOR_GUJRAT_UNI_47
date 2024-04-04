#ifndef E2ROM_H
#define E2ROM_H

#define CHIP_SIZE 65536

//    P6.1 = SDA for EEPROM
//    P6.0 = SCL for EEPROM

// E2ROM definitions start-----
#define SDA_DIR PM6
#define SDA_PIN P6
#define SDA_OUT P6
#define SDA_BIT 0x02	// Bit 1

#define SCL_DIR PM6
#define SCL_OUT P6
#define SCL_BIT 0x01	// Bit 0

// E2ROM definitions end-----


// harware specific (e2rom)


#define MY_SDA_e2rom (SDA_PIN & SDA_BIT)		
#define SDA_e2rom_MAKEOUT SDA_DIR &= ~SDA_BIT	// PM register - 0 is output
#define SDA_e2rom_MAKEINP SDA_DIR |= SDA_BIT	// PM register - 1 is input
#define SDA_e2rom_LOW SDA_OUT &= ~SDA_BIT
#define SDA_e2rom_HI SDA_OUT |= SDA_BIT

#define SCLK_e2rom_MAKEINP SCL_DIR |= SCL_BIT
#define SCLK_e2rom_MAKEOUT SCL_DIR &= ~SCL_BIT
#define SCLK_e2rom_LOW SCL_OUT &= ~SCL_BIT
#define SCLK_e2rom_HI SCL_OUT |= SCL_BIT

void EPR_Init(void);
uint8_t EPR_Read(uint32_t addr, uint8_t* buf, uint16_t size);
uint8_t EPR_Write(uint32_t addr, uint8_t* buf, uint16_t size);

// This is only for testing - dummy routine
void EPR_Write_Helper_TEST(uint32_t addr, uint8_t* buf, uint16_t size);


#endif
