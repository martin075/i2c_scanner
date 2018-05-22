#include <compat/twi.h>  // TW_STATUS, TW_START, TW_REP_START, TW_MT_SLA_ACK, TW_MR_SLA_ACK, TW_MT_DATA_ACK

// I2C Functions
uint8_t i2cStart(uint8_t address){
    uint8_t twiStatus;
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  // send START condition
	while(!(TWCR & (1<<TWINT)));                 // wait until transmission completed
	twiStatus = TW_STATUS & 0xF8;                // check value of TWI Status Register. Mask prescaler bits.
	if((twiStatus != TW_START) && (twiStatus != TW_REP_START))
		return 0;
	TWDR = address;                 // load slave address into buffer
	TWCR = (1<<TWINT) | (1<<TWEN);  // send slave address
	while(!(TWCR & (1<<TWINT)));    // wait until transmission completed and ACK/NACK has been received
	twiStatus = TW_STATUS & 0xF8;   // check value of TWI Status Register. Mask prescaler bits.
	if((twiStatus != TW_MT_SLA_ACK) && (twiStatus != TW_MR_SLA_ACK))
		return 0;
	return 1;
}

void i2cStop(void){                              // terminates the data transfer and releases the I2C bus
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);  // send stop condition
	while(TWCR & (1<<TWSTO));                    // wait until stop condition is executed and bus released
}

uint8_t i2cWrite(uint8_t data){     // send one byte to I2C device
	uint8_t twiStatus;
	TWDR = data;                    // send data to the previously addressed device
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));    // wait until transmission completed
	twiStatus = TW_STATUS & 0xF8;   // check value of TWI Status Register. Mask prescaler bits
	if( twiStatus != TW_MT_DATA_ACK)
		return 0;
	return 1;
}

uint8_t i2cReadAck(void){  // read one byte from the I2C device, request more data from device
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
    return TWDR;
}

uint8_t i2cReadNak(void){  // read one byte from the I2C device, read is followed by a stop condition
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
    return TWDR;
}